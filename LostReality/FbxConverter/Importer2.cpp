/*
* file Importer.cpp
*
* author luoxw
* date 2017/03/10
*
*
*/

#include "stdafx.h"
#include "Importer.h"

using namespace Importer;

static const string SSeperator("--------------------------------");
static const string SIndent("\t\t");

static void ParsePolygons(std::function<FJson&()> outputGetter, FbxMesh* mesh, const map<int, vector<int>>& controlPointToVertexMap)
{
	if (mesh == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	int polygonCount = mesh->GetPolygonCount();
	output[K_COUNT] = polygonCount;
	for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		int polygonSize = mesh->GetPolygonSize(polygonIndex);
		for (int posInPolygon = 0; posInPolygon < polygonSize; ++posInPolygon)
		{
			int vertexIndex = mesh->GetPolygonVertex(polygonIndex, posInPolygon);
			output[K_INDEX].push_back(vertexIndex);
		}
	}
}

static void ParseLink(function<FJson&()> outputGetter, FJson& vertices, FbxMesh* mesh, const map<int, vector<int>>& controlPointToVertexMap)
{
	if (mesh == nullptr)
	{
		return;
	}

	int skinIndex = 0;
	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);

	assert(skinCount <= 1 && "skinCount is bigger than 1");

	FJson& output = outputGetter();
	for (int i = 0; i < skinCount; ++i)
	{
		auto deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);
		FJson & vertj = output[K_VERTEX];
		int clusterCount = ((FbxSkin*)deformer)->GetClusterCount();
		for (int j = 0; j < clusterCount; ++j)
		{
			auto cluster = ((FbxSkin*)deformer)->GetCluster(j);
			if (cluster->GetLink() != nullptr)
			{
				output[K_SKIN].push_back(cluster->GetLink()->GetName());
			}
			else
			{
				output[K_SKIN].push_back(string("unamed").append(to_string(skinIndex)).c_str());
			}

			int cpIndexCount = cluster->GetControlPointIndicesCount();
			int* indices = cluster->GetControlPointIndices();
			double* weights = cluster->GetControlPointWeights();
			for (int k = 0; k < cpIndexCount; ++k)
			{
				int cpIndex = indices[k];
				if (controlPointToVertexMap.empty())
				{
					vertj[cpIndex][K_SKIN].push_back(FJson());
					FJson& j1 = *(vertj[cpIndex][K_SKIN].end() - 1);
					j1[K_BONE] = skinIndex;
					j1[K_WEIGHT] = weights[k];
				}
				else if (controlPointToVertexMap.find(cpIndex) != controlPointToVertexMap.end())
				{
					for (auto vertexIndex : controlPointToVertexMap.at(cpIndex))
					{
						vertj[vertexIndex][K_SKIN].push_back(FJson());
						FJson& j1 = *(vertj[vertexIndex][K_SKIN].end() - 1);
						j1[K_BONE] = skinIndex;
						j1[K_WEIGHT] = weights[k];
					}
				}
			}

			++skinIndex;
		}
	}
}

static void ParsePose(FJson& output, FbxScene* scene)
{
	int count = scene->GetPoseCount();
	for (int i = 0; i < count; ++i)
	{
		output[K_POSE].push_back(FJson());
		auto it = output[K_POSE].end() - 1;
		auto pose = scene->GetPose(i);
		(*it)[K_NAME] = pose->GetName();
		(*it)[K_ISBIND] = pose->IsBindPose() ? 1 : 0;
		(*it)[K_COUNT] = pose->GetCount();

		for (int j = 0; j < pose->GetCount(); ++j)
		{
			(*it)[K_POSE].push_back(FJson());
			auto it2 = (*it)[K_POSE].end() - 1;
			(*it2)[K_NAME] = pose->GetNodeName(j).GetCurrentName();
			WriteFloat4x4((*it2)[K_MATRIX], pose->GetMatrix(j));
		}
	}

	count = scene->GetCharacterPoseCount();
	for (int i = 0; i < count; ++i)
	{
		output[K_CHARACTER_POSE].push_back(FJson());
		auto it = output[K_CHARACTER_POSE].end() - 1;
		auto pose = scene->GetCharacterPose(i);
		auto character = pose->GetCharacter();
		if (character == nullptr)
		{
			break;
		}

		FbxCharacterLink charLink;
		FbxCharacter::ENodeId nodeid = FbxCharacter::eHips;
		while (character->GetCharacterLink(nodeid, &charLink))
		{
			(*it)[K_MATRIX].push_back(FJson());
			auto it2 = (*it)[K_MATRIX].end() - 1;
			auto& pos = charLink.mNode->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);
			WriteFloat4x4(*it2, pos);
		}
	}
}

static void InterpolationFlagToIndex(std::function<FJson&()> outputGetter, int flags)
{
	if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) outputGetter() = (int)EInterpolation::Constant;
	if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) outputGetter() = (int)EInterpolation::Linear;
	if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) outputGetter() = (int)EInterpolation::Cubic;
}

static void ConstantModeFlagToIndex(std::function<FJson&()> outputGetter, int flags)
{
	if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) outputGetter() = (int)EConstantMode::Standard;
	if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) outputGetter() = (int)EConstantMode::Next;
}

static void TangentModeFlagToIndex(std::function<FJson&()> outputGetter, int flags)
{
	if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) outputGetter() = (int)ECubicMode::Auto;
	if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) outputGetter() = (int)ECubicMode::AutoBreak;
	if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) outputGetter() = (int)ECubicMode::Tcb;
	if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) outputGetter() = (int)ECubicMode::User;
}

static void TangentWeightFlagToIndex(std::function<FJson&()> outputGetter, int flags)
{
	//if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) outputGetter() = (int)ETangentWVMode::None;
	if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) outputGetter() = (int)ETangentWVMode::Right;
	if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) outputGetter() = (int)ETangentWVMode::NextLeft;
}

static void TangentVelocityFlagToIndex(std::function<FJson&()> outputGetter, int flags)
{
	//if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) outputGetter() = (int)ETangentWVMode::None;
	if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) outputGetter() = (int)ETangentWVMode::Right;
	if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) outputGetter() = (int)ETangentWVMode::NextLeft;
}

static void ParseCurveKeys(std::function<FJson&()> outputGetter, FbxAnimCurve* curve)
{
	if (curve == nullptr)
	{
		return;
	}

	FJson& out = outputGetter();
	int count = curve->KeyGetCount();
	for (int i = 0; i < count; ++i)
	{
		out[K_CURVE_KEY].push_back(FJson());
		auto it = out[K_CURVE_KEY].end() - 1;
		float value = static_cast<float>(curve->KeyGetValue(i));
		FbxTime time = curve->KeyGetTime(i);
		(*it)[K_KEY_TIME] = time.GetSecondDouble();
		(*it)[K_VALUE] = value;
		InterpolationFlagToIndex([&]()->FJson& {return (*it)[K_INTERPOLATION]; }, curve->KeyGetInterpolation(i));
		int flags = curve->KeyGetInterpolation(i);
		if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
		{
			ConstantModeFlagToIndex([&]()->FJson& {return (*it)[K_CONSTANT_MODE]; }, flags);
		}
		else if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
		{
			TangentModeFlagToIndex([&]()->FJson& {return (*it)[K_CUBIC_MODE]; }, flags);
			TangentWeightFlagToIndex([&]()->FJson& {return (*it)[K_TANGENT_WEIGHT]; }, curve->KeyGet(i).GetTangentWeightMode());
			TangentVelocityFlagToIndex([&]()->FJson& {return (*it)[K_TANGENT_VELOCITY]; }, curve->KeyGet(i).GetTangentVelocityMode());
		}
	}
}

static void ParseListCurveKeys(std::function<FJson&()> outputGetter, FbxAnimCurve* curve, FbxProperty* prop)
{
	if (curve == nullptr || prop == nullptr)
	{
		return;
	}

	FJson& out = outputGetter();
	int count = curve->KeyGetCount();
	for (int i = 0; i < count; ++i)
	{
		out[K_CURVE_LIST].push_back(FJson());
		auto it = out[K_CURVE_LIST].end() - 1;
		int value = static_cast<int>(curve->KeyGetValue(i));
		FbxTime keyTime = curve->KeyGetTime(i);
		(*it)[K_KEY_TIME] = keyTime.GetSecondDouble();
		(*it)[K_VALUE] = value;
		(*it)[K_PROPERTY] = prop->GetEnumValue(value);
	}
}

static void ParseChannels(FJson& output,
	FbxNode* node,
	FbxAnimLayer* layer,
	std::function<void(std::function<FJson&()>, FbxAnimCurve*)> DisplayCurve,
	std::function<void(std::function<FJson&()>, FbxAnimCurve*, FbxProperty*)> DisplayListCurve,
	bool bIsSwitcher)
{
	if (!bIsSwitcher)
	{
		DisplayCurve([&]()->FJson& {return output[K_TRANSLATE_X]; }, node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X));
		DisplayCurve([&]()->FJson& {return output[K_TRANSLATE_Y]; }, node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y));
		DisplayCurve([&]()->FJson& {return output[K_TRANSLATE_Z]; }, node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z));
		DisplayCurve([&]()->FJson& {return output[K_ROTATE_X]; }, node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X));
		DisplayCurve([&]()->FJson& {return output[K_ROTATE_Y]; }, node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y));
		DisplayCurve([&]()->FJson& {return output[K_ROTATE_Z]; }, node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z));
		DisplayCurve([&]()->FJson& {return output[K_SCALE_X]; }, node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X));
		DisplayCurve([&]()->FJson& {return output[K_SCALE_Y]; }, node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y));
		DisplayCurve([&]()->FJson& {return output[K_SCALE_Z]; }, node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z));
	}


	FbxNodeAttribute* attr = node->GetNodeAttribute();
	if (attr != nullptr)
	{
		DisplayCurve([&]()->FJson& {return output[K_RED]; }, attr->Color.GetCurve(layer, FBXSDK_CURVENODE_COLOR_RED));
		DisplayCurve([&]()->FJson& {return output[K_GREEN]; }, attr->Color.GetCurve(layer, FBXSDK_CURVENODE_COLOR_GREEN));
		DisplayCurve([&]()->FJson& {return output[K_BLUE]; }, attr->Color.GetCurve(layer, FBXSDK_CURVENODE_COLOR_BLUE));

		auto light = node->GetLight();
		if (light != nullptr)
		{
			DisplayCurve([&]()->FJson& {return output[K_INTENSITY]; }, light->Intensity.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_OUTER_ANGLE]; }, light->OuterAngle.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_FOG]; }, light->Fog.GetCurve(layer));
		}

		auto camera = node->GetCamera();
		if (camera != nullptr)
		{
			DisplayCurve([&]()->FJson& {return output[K_FOV]; }, camera->FieldOfView.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_FOV_X]; }, camera->FieldOfViewX.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_FOV_Y]; }, camera->FieldOfViewY.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_OPTICAL_CENTERX]; }, camera->OpticalCenterX.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_OPTICAL_CENTERY]; }, camera->OpticalCenterY.GetCurve(layer));
			DisplayCurve([&]()->FJson& {return output[K_ROLL]; }, camera->Roll.GetCurve(layer));
		}

		if (attr->GetAttributeType() == FbxNodeAttribute::eMesh ||
			attr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			attr->GetAttributeType() == FbxNodeAttribute::ePatch)
		{
			FbxGeometry* geo = (FbxGeometry*)attr;
			int count = geo->GetDeformerCount(FbxDeformer::eBlendShape);
			for (int i = 0; i < count; ++i)
			{
				FbxBlendShape* shape = (FbxBlendShape*)geo->GetDeformer(i, FbxDeformer::eBlendShape);
				int channelCount = shape->GetBlendShapeChannelCount();
				for (int j = 0; j < channelCount; ++j)
				{
					auto channel = shape->GetBlendShapeChannel(j);
					output[K_SHAPE_CHANNEL][K_NAME] = channel->GetName();
					DisplayCurve([&]()->FJson& {return output[K_SHAPE_CHANNEL]; }, geo->GetShapeChannel(i, j, layer, true));
				}
			}
		}
	}

	auto prop = node->GetFirstProperty();
	while (prop.IsValid())
	{
		if (prop.GetFlag(FbxPropertyFlags::eUserDefined))
		{
			output[K_PROPERTY].push_back(FJson());
			auto it = output[K_PROPERTY].end() - 1;
			auto name = prop.GetName();
			auto curveNode = prop.GetCurveNode(layer);
			if (curveNode == nullptr)
			{
				prop = node->GetNextProperty(prop);
				continue;
			}

			auto dataType = prop.GetPropertyDataType();
			if ((dataType.GetType() == eFbxBool) ||
				(dataType.GetType() == eFbxDouble) ||
				(dataType.GetType() == eFbxFloat) ||
				(dataType.GetType() == eFbxInt))
			{
				(*it)[K_NAME] = prop.GetName().Buffer();
				(*it)[K_LABEL] = prop.GetLabel().Buffer();
				(*it)[K_COUNT] = curveNode->GetCurveCount(0U);
				for (int i = 0; i < curveNode->GetCurveCount(0U); ++i)
				{
					(*it)[K_CURVE].push_back(FJson());
					auto it2 = (*it)[K_CURVE].end() - 1;
					DisplayCurve([&]()->FJson& {return *it2; }, curveNode->GetCurve(0U, i));
				}
			}
			else if (
				(dataType.GetType() == eFbxDouble3) ||
				(dataType.GetType() == eFbxDouble4) ||
				(dataType.Is(FbxColor3DT)) ||
				(dataType.Is(FbxColor4DT)))
			{
				bool isDT = dataType.Is(FbxColor3DT) || dataType.Is(FbxColor4DT);
				const char* name1 = isDT ? FBXSDK_CURVENODE_COLOR_RED : "X";
				const char* name2 = isDT ? FBXSDK_CURVENODE_COLOR_GREEN : "Y";
				const char* name3 = isDT ? FBXSDK_CURVENODE_COLOR_BLUE : "Z";

				(*it)[K_NAME] = prop.GetName().Buffer();
				(*it)[K_LABEL] = prop.GetLabel().Buffer();
				(*it)[K_COUNT] = curveNode->GetCurveCount(0U) +
					curveNode->GetCurveCount(1U) +
					curveNode->GetCurveCount(2U);

				for (int i = 0; i < curveNode->GetCurveCount(0U); ++i)
				{
					(*it)[K_CURVE].push_back(FJson());
					auto it2 = (*it)[K_CURVE].end() - 1;
					(*it2)[K_NAME] = name1;
					DisplayCurve([&]()->FJson& {return *it2; }, curveNode->GetCurve(0U, i));
				}

				for (int i = 0; i < curveNode->GetCurveCount(1U); ++i)
				{
					(*it)[K_CURVE].push_back(FJson());
					auto it2 = (*it)[K_CURVE].end() - 1;
					(*it2)[K_NAME] = name2;
					DisplayCurve([&]()->FJson& {return *it2; }, curveNode->GetCurve(1U, i));
				}

				for (int i = 0; i < curveNode->GetCurveCount(2U); ++i)
				{
					(*it)[K_CURVE].push_back(FJson());
					auto it2 = (*it)[K_CURVE].end() - 1;
					(*it2)[K_NAME] = name3;
					DisplayCurve([&]()->FJson& {return *it2; }, curveNode->GetCurve(2U, i));
				}
			}
			else if (dataType.GetType() == eFbxEnum)
			{
				(*it)[K_NAME] = prop.GetName().Buffer();
				(*it)[K_LABEL] = prop.GetLabel().Buffer();
				(*it)[K_COUNT] = curveNode->GetCurveCount(0U);
				for (int i = 0; i < curveNode->GetCurveCount(0U); ++i)
				{
					DisplayListCurve([&]()->FJson& {return (*it)[K_CURVE_LIST]; }, curveNode->GetCurve(0U, i), &prop);
				}
			}
		}

		prop = node->GetNextProperty(prop);
	}
}

static void ParseAnimationLayer(function<FJson&()> outputGetter, FbxAnimLayer* layer, FbxNode* node, bool bIsSwitcher)
{
	if (layer == nullptr || node == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	output[K_NAME] = node->GetName();

	ParseChannels(output, node, layer, ParseCurveKeys, ParseListCurveKeys, bIsSwitcher);

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		output[K_LAYER].push_back(FJson());
		auto it = output[K_LAYER].end() - 1;
		ParseAnimationLayer([&]()->FJson& {return *it; }, layer, node->GetChild(i), bIsSwitcher);
	}
}

static void ParseAnimationStack(function<FJson&()> outputGetter, FbxAnimStack* stack, FbxNode* node, bool bIsSwitcher)
{
	if (stack == nullptr || node == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();

	output[K_NAME] = stack->GetName();
	int count = stack->GetMemberCount<FbxAnimLayer>();
	for (int i = 0; i < count; ++i)
	{
		output[K_LAYER].push_back(FJson());
		auto it = output[K_LAYER].end() - 1;
		auto layer = stack->GetMember<FbxAnimLayer>(i);
		ParseAnimationLayer([&]()->FJson& {return *it; }, layer, node, bIsSwitcher);
	}
}

static void ParseAnimation(std::function<FJson&()> outputGetter, FbxScene* scene)
{
	if (scene == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	int animStackCount = scene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < animStackCount; ++i)
	{
		output[K_STACK].push_back(FJson());
		FJson& stackJson = *(output[K_STACK].end() - 1);
		ParseAnimationStack([&]()->FJson& {return stackJson; }, scene->GetSrcObject<FbxAnimStack>(i), scene->GetRootNode(), false);
	}
}

static void ParsePose(std::function<FJson&()> outputGetter, FbxScene* scene)
{
	if (scene == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	for (int i = 0; i < scene->GetPoseCount(); ++i)
	{
		output[K_POSES].push_back(FJson());
		FJson& poseJson = *(output[K_POSES].end() - 1);
		auto pose = scene->GetPose(i);
		poseJson[K_NAME] = pose->GetName();
		poseJson[K_ISBIND] = pose->IsBindPose() ? 1 : 0;
		for (int j = 0; j < pose->GetCount(); ++j)
		{
			poseJson[K_POSE].push_back(FJson());
			FJson& json0 = *(poseJson[K_POSE].end() - 1);
			json0[K_NAME] = pose->GetNodeName(j).GetCurrentName();
			WriteFloat4x4(json0[K_MATRIX], pose->GetMatrix(j));
		}
	}

	for (int i = 0; i < scene->GetCharacterPoseCount(); ++i)
	{
		output[K_CHARACTER_POSE].push_back(FJson());
		FJson& poseJson = *(output[K_CHARACTER_POSE].end() - 1);
		auto pose = scene->GetCharacterPose(i);
		auto character = scene->GetCharacter(i);
		if (character == nullptr)
		{
			continue;
		}

		FbxCharacterLink link;
		FbxCharacter::ENodeId nodeid = FbxCharacter::eHips;
		while (character->GetCharacterLink(nodeid, &link))
		{
			poseJson[K_MATRIX].push_back(FJson());
			FJson& matJson = *(poseJson[K_MATRIX].end() - 1);
			auto& pos = link.mNode->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);
			WriteFloat4x4(matJson, pos);
		}
	}
}

static void ParseSkeleton(std::function<FJson&()> outputGetter, FbxNode* node)
{

}

static void ParseVertices(function<FJson&()> outputGetter, FbxMesh* mesh, map<int, vector<int>>& controlPointToVertexMap)
{
	if (mesh == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	auto cpHead = mesh->GetControlPoints();
	int cpCount = mesh->GetControlPointsCount();
	auto normalHead = mesh->GetElementNormal(0);
	auto binormalHead = mesh->GetElementBinormal(0);
	auto tangentHead = mesh->GetElementTangent(0);
	auto uvHead = mesh->GetElementUV(0);
	auto vertexColorHead = mesh->GetElementVertexColor(0);

	unsigned int flags = EVertexElement::Coordinate;
	flags |= (normalHead != nullptr ? EVertexElement::Normal : 0);
	flags |= (binormalHead != nullptr ? EVertexElement::Binormal : 0);
	flags |= (tangentHead != nullptr ? EVertexElement::Tangent : 0);
	flags |= (uvHead != nullptr ? EVertexElement::UV : 0);
	flags |= (vertexColorHead != nullptr ? EVertexElement::VertexColor : 0);
	output[K_VERTEX_ELEMENT] = (unsigned int)flags;

	bool bUseControlPointAsVertex = true;
	if (uvHead != nullptr && uvHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		bUseControlPointAsVertex = false;
		output[K_MESSAGE] = string("| uv is eByPolygonVertex ") + to_string(uvHead->GetDirectArray().GetCount()) +
			string(" + ") + to_string(uvHead->GetIndexArray().GetCount());
	}

	if (normalHead != nullptr && normalHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		bUseControlPointAsVertex = false;
		output[K_MESSAGE2] = string("| normal is eByPolygonVertex ") + to_string(normalHead->GetDirectArray().GetCount()) +
			string(" + ") + to_string(uvHead->GetIndexArray().GetCount());
	}

	if (binormalHead != nullptr && binormalHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		bUseControlPointAsVertex = false;
		output[K_MESSAGE3] = ("| binormal is eByPolygonVertex ");
	}

	if (tangentHead != nullptr && tangentHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		bUseControlPointAsVertex = false;
		output[K_MESSAGE4] = ("| tangent is eByPolygonVertex ");
	}

	if (vertexColorHead != nullptr && vertexColorHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		bUseControlPointAsVertex = false;
		output[K_MESSAGE5] = string("| vertex color is eByPolygonVertex ") + to_string(vertexColorHead->GetDirectArray().GetCount()) +
			string(" + ") + to_string(vertexColorHead->GetIndexArray().GetCount());
	}

	if (bUseControlPointAsVertex)
	{
		for (int i = 0; i < cpCount; ++i)
		{
			output[K_VERTEX].push_back(FJson());
			FJson& j0 = *(output[K_VERTEX].end() - 1);

			// coordinate
			WriteFloat3(j0[K_COORDINATE], cpHead[i]);

			// element uv
			if (uvHead != nullptr)
			{
				if (uvHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					if (uvHead->GetReferenceMode() == FbxGeometryElement::eDirect)
					{
						WriteFloat2(j0[K_UV], uvHead->GetDirectArray().GetAt(i));
					}
					else if (uvHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					{
						WriteFloat2(j0[K_UV], uvHead->GetDirectArray().GetAt(uvHead->GetIndexArray().GetAt(i)));
					}
				}
			}

			// element normal
			if (normalHead != nullptr)
			{
				if (normalHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					if (normalHead->GetReferenceMode() == FbxGeometryElement::eDirect)
					{
						WriteFloat3(j0[K_NORMAL], normalHead->GetDirectArray().GetAt(i));
					}
					else if (normalHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					{
						WriteFloat3(j0[K_NORMAL], normalHead->GetDirectArray().GetAt(normalHead->GetIndexArray().GetAt(i)));
					}
				}
			}

			// element tangent
			if (tangentHead != nullptr)
			{
				if (tangentHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					if (tangentHead->GetReferenceMode() == FbxGeometryElement::eDirect)
					{
						WriteFloat3(j0[K_TANGENT], tangentHead->GetDirectArray().GetAt(i));
					}
					else if (tangentHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					{
						WriteFloat3(j0[K_TANGENT], tangentHead->GetDirectArray().GetAt(tangentHead->GetIndexArray().GetAt(i)));
					}
				}
			}

			// element binormal
			if (binormalHead != nullptr)
			{
				if (binormalHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					if (binormalHead->GetReferenceMode() == FbxGeometryElement::eDirect)
					{
						WriteFloat3(j0[K_BINORMAL], binormalHead->GetDirectArray().GetAt(i));
					}
					else if (binormalHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					{
						WriteFloat3(j0[K_BINORMAL], binormalHead->GetDirectArray().GetAt(binormalHead->GetIndexArray().GetAt(i)));
					}
				}
			}

			// element vertex color
			if (vertexColorHead != nullptr)
			{
				if (vertexColorHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					if (vertexColorHead->GetReferenceMode() == FbxGeometryElement::eDirect)
					{
						WriteRGB(j0[K_RGB], vertexColorHead->GetDirectArray().GetAt(i));
					}
					else if (vertexColorHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
					{
						WriteRGB(j0[K_RGB], vertexColorHead->GetDirectArray().GetAt(vertexColorHead->GetIndexArray().GetAt(i)));
					}
				}
			}
		}
	}
	else
	{
		int vertexID = 0;
		int polygonCount = mesh->GetPolygonCount();
		for (int i = 0; i < polygonCount; ++i)
		{
			int polygonSize = mesh->GetPolygonSize(i);
			for (int j = 0; j < polygonSize; ++j)
			{
				output[K_VERTEX].push_back(FJson());
				FJson& j0 = *(output[K_VERTEX].end() - 1);

				int controlPointIndex = mesh->GetPolygonVertex(i, j);
				controlPointToVertexMap[controlPointIndex].push_back(vertexID);

				WriteFloat3(j0[K_COORDINATE], cpHead[controlPointIndex]);

				// element uv
				if (uvHead != nullptr)
				{
					if (uvHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
					{
						if (uvHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat2(j0[K_UV], uvHead->GetDirectArray().GetAt(controlPointIndex));
						}
						else if (uvHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat2(j0[K_UV], uvHead->GetDirectArray().GetAt(uvHead->GetIndexArray().GetAt(controlPointIndex)));
						}
					}
					else if (uvHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
					{
						if (uvHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat2(j0[K_UV], uvHead->GetDirectArray().GetAt(vertexID));
						}
						else if (uvHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat2(j0[K_UV], uvHead->GetDirectArray().GetAt(uvHead->GetIndexArray().GetAt(vertexID)));
						}
					}
				}

				// element normal
				if (normalHead != nullptr)
				{
					if (normalHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
					{
						if (normalHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat3(j0[K_NORMAL], normalHead->GetDirectArray().GetAt(controlPointIndex));
						}
						else if (normalHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat3(j0[K_NORMAL], normalHead->GetDirectArray().GetAt(normalHead->GetIndexArray().GetAt(controlPointIndex)));
						}
					}
					else if (normalHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
					{
						if (normalHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat3(j0[K_NORMAL], normalHead->GetDirectArray().GetAt(vertexID));
						}
						else if (normalHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat3(j0[K_NORMAL], normalHead->GetDirectArray().GetAt(normalHead->GetIndexArray().GetAt(vertexID)));
						}
					}
				}

				// element tangent
				if (tangentHead != nullptr)
				{
					if (tangentHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
					{
						if (tangentHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat3(j0[K_TANGENT], tangentHead->GetDirectArray().GetAt(controlPointIndex));
						}
						else if (tangentHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat3(j0[K_TANGENT], tangentHead->GetDirectArray().GetAt(tangentHead->GetIndexArray().GetAt(controlPointIndex)));
						}
					}
					else if (tangentHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
					{
						if (tangentHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat3(j0[K_TANGENT], tangentHead->GetDirectArray().GetAt(vertexID));
						}
						else if (tangentHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat3(j0[K_TANGENT], tangentHead->GetDirectArray().GetAt(tangentHead->GetIndexArray().GetAt(vertexID)));
						}
					}
				}

				// element binormal
				if (binormalHead != nullptr)
				{
					if (binormalHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
					{
						if (binormalHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat3(j0[K_BINORMAL], binormalHead->GetDirectArray().GetAt(controlPointIndex));
						}
						else if (binormalHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat3(j0[K_BINORMAL], binormalHead->GetDirectArray().GetAt(binormalHead->GetIndexArray().GetAt(controlPointIndex)));
						}
					}
					else if (binormalHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
					{
						if (binormalHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteFloat3(j0[K_BINORMAL], binormalHead->GetDirectArray().GetAt(vertexID));
						}
						else if (binormalHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteFloat3(j0[K_BINORMAL], binormalHead->GetDirectArray().GetAt(binormalHead->GetIndexArray().GetAt(vertexID)));
						}
					}
				}

				// element vertex color
				if (vertexColorHead != nullptr)
				{
					if (vertexColorHead->GetMappingMode() == FbxGeometryElement::eByControlPoint)
					{
						if (vertexColorHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteRGB(j0[K_VERTEXCOLOR], vertexColorHead->GetDirectArray().GetAt(controlPointIndex));
						}
						else if (vertexColorHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteRGB(j0[K_VERTEXCOLOR], vertexColorHead->GetDirectArray().GetAt(vertexColorHead->GetIndexArray().GetAt(controlPointIndex)));
						}
					}
					else if (vertexColorHead->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
					{
						if (vertexColorHead->GetReferenceMode() == FbxGeometryElement::eDirect)
						{
							WriteRGB(j0[K_VERTEXCOLOR], vertexColorHead->GetDirectArray().GetAt(vertexID));
						}
						else if (vertexColorHead->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
						{
							WriteRGB(j0[K_VERTEXCOLOR], vertexColorHead->GetDirectArray().GetAt(vertexColorHead->GetIndexArray().GetAt(vertexID)));
						}
					}
				}

				++vertexID;
			}
		}
	}
}

static void ParseMesh(std::function<FJson&()> outputGetter, FbxNode* node)
{
	if (node == nullptr)
	{
		return;
	}

	FbxMesh* mesh = (FbxMesh*)node->GetNodeAttribute();
	if (mesh == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	output[K_NAME] = node->GetName();

	map<int, vector<int>> ControlPointToVertexMap;
	ParseVertices([&]()->FJson& {return output; }, mesh, ControlPointToVertexMap);
	ParseLink([&]()->FJson& {return output; }, output, mesh, ControlPointToVertexMap);
	ParsePolygons([&]()->FJson& {return output[K_POLYGONS]; }, mesh, ControlPointToVertexMap);
}

class FFbxImporter2
{
public:
	static FFbxImporter2* Get()
	{
		static FFbxImporter2 Inst;
		return &Inst;
	}

public:
	FFbxImporter2()
		: SdkManager(nullptr)
		, SdkScene(nullptr)
		, bExportScene(false)
	{
		InitializeSdkObjects(SdkManager, SdkScene);
	}

	void OutputToStream()
	{
		if (bExportScene)
		{
			ofstream file;
			file.open(ConvertPath, ios::out);

			FJson json;
			json[K_META] = MetaData;
			json[K_NODE] = NodeData;
			json[K_POSE] = PoseData;
			json[K_ANIMATION] = AnimData;

			file << json << endl;

			file.close();
		}
		else
		{
			for (auto it = MeshData.begin(); it != MeshData.end(); ++it)
			{
				char head[] = "lost reality resource file\n";
				ofstream file;
				file.open(ConvertPath + it.key() + ".xpt", ios::out);
				file.width(1);
				file << it.value();
				file.close();
			}
		}
	}

	void ImportMetaData()
	{
		MetaData.clear();
		FbxDocumentInfo* info = SdkScene->GetSceneInfo();
		if (info != nullptr)
		{
			MetaData[K_TITLE] = info->mTitle.Buffer();
			MetaData[K_SUBJECT] = info->mSubject.Buffer();
			MetaData[K_AUTHOR] = info->mAuthor.Buffer();
			MetaData[K_COMMENT] = info->mComment.Buffer();
			MetaData[K_KEYWORDS] = info->mKeywords.Buffer();
			MetaData[K_REVISION] = info->mRevision.Buffer();

			FbxThumbnail* thumbnail = info->GetSceneThumbnail();
			if (thumbnail != nullptr)
			{
				switch (thumbnail->GetDataFormat())
				{
				case FbxThumbnail::eRGB_24:
					MetaData[K_THUMBNAIL][K_FORMAT] = (int)EThumbnailFormat::RGB;
					break;
				case FbxThumbnail::eRGBA_32:
					MetaData[K_THUMBNAIL][K_FORMAT] = (int)EThumbnailFormat::RGBA;
				default:
					break;
				}

				switch (thumbnail->GetSize())
				{
				case FbxThumbnail::eNotSet:
					MetaData[K_THUMBNAIL][K_DIM_UNKNOW] = thumbnail->GetSizeInBytes();
					break;
				case FbxThumbnail::e64x64:
					MetaData[K_THUMBNAIL][K_DIM_64] = thumbnail->GetSizeInBytes();
					break;
				case FbxThumbnail::e128x128:
					MetaData[K_THUMBNAIL][K_DIM_128] = thumbnail->GetSizeInBytes();
					break;
				default:
					break;
				}
			}
		}
	}

	void ImportNode(std::function<FJson&()> outputGetter, FbxNode* node)
	{
		if (node == nullptr)
		{
			return;
		}

		FJson& output = outputGetter();

		FbxNodeAttribute::EType attrType;
		if (node->GetNodeAttribute() != nullptr)
		{
			attrType = node->GetNodeAttribute()->GetAttributeType();
			switch (attrType)
			{
			case FbxNodeAttribute::eSkeleton:
				ParseSkeleton([&]()->FJson& {output[K_TYPE] = K_SKELETON; return output; }, node);
				break;
			case FbxNodeAttribute::eMesh:
				if (bExportScene)
				{
					ParseMesh([&]()->FJson& {output[K_TYPE] = K_MESH; return output; }, node);
				}
				else
				{
					ParseMesh([&]()->FJson& {return output[node->GetName()]; }, node);
				}
				break;
			default:
				break;
			}
		}

		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			ImportNode(outputGetter, node->GetChild(i));
		}
	}

	void ImportContent()
	{
		FbxNode* node = SdkScene->GetRootNode();
		if (node != nullptr)
		{
			for (int i = 0; i < node->GetChildCount(); ++i)
			{
				if (bExportScene)
				{
					ImportNode([&]()->FJson& {NodeData[K_CHILDREN].push_back(FJson()); return *(NodeData[K_CHILDREN].end() - 1); }, node->GetChild(i));
				}
				else
				{
					ImportNode([&]()->FJson& {return MeshData; }, node->GetChild(i));
				}
			}
		}
	}

	void ImportPose()
	{
		if (bExportScene)
		{
			ParsePose([&]()->FJson& {return PoseData; }, SdkScene);
		}
		else
		{
			ParsePose([&]()->FJson& {return MeshData; }, SdkScene);
		}
	}

	void ImportAnimation()
	{
		ParseAnimation([&]()->FJson& {return AnimData; }, SdkScene);
	}

	bool ImportScene(const string& importSrc, const string& convertDst)
	{
		ImportPath = importSrc;
		ConvertPath = convertDst;
		bExportScene = true;
		bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());

		if (result)
		{
			ImportMetaData();
			ImportContent();
			ImportPose();
			ImportAnimation();
		}

		OutputToStream();

		return true;
	}

	bool ImportSceneMeshes(const string& importSrc, const string& convertDst)
	{
		ImportPath = importSrc;

		// format path, find out the correct directory path
		ConvertPath = convertDst;
		auto pos = std::string::npos;
		while ((pos = ConvertPath.find("/")) != std::string::npos)
		{
			ConvertPath.replace(pos, 1, "\\");
		}

		auto lastSlash = ConvertPath.rfind("\\");
		auto lastDot = ConvertPath.rfind(".");
		bool isDir = (lastDot == string::npos) || (lastDot < lastSlash);
		if (!isDir)
		{
			ConvertPath.resize(lastSlash + 1);
		}

		bExportScene = false;

		bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());

		if (result)
		{
			ImportMetaData();
			ImportContent();
			ImportPose();
			ImportAnimation();
		}

		OutputToStream();
		return true;
	}

private:
	FbxManager*			SdkManager;
	FbxScene*			SdkScene;

	string				ImportPath;
	string				ConvertPath;

	FJson				MetaData;
	FJson				NodeData;
	FJson				PoseData;
	FJson				AnimData;
	FJson				MeshData;

	bool				bExportScene;
	vector<string>		MeshFiles;
};

bool Importer::ImportScene2(const string& importSrc, const string& convertDst)
{
	return FFbxImporter2::Get()->ImportScene(importSrc, convertDst);
}

bool Importer::ImportSceneMeshes(const string& importSrc, const string& convertDst)
{
	return FFbxImporter2::Get()->ImportSceneMeshes(importSrc, convertDst);
}