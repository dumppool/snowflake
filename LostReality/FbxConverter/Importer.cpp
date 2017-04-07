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

// improt sample code includes
#include "FbxSamples/ImportScene/DisplayCommon.h"
#include "FbxSamples/ImportScene/DisplayHierarchy.h"
#include "FbxSamples/ImportScene/DisplayAnimation.h"
#include "FbxSamples/ImportScene/DisplayMarker.h"
#include "FbxSamples/ImportScene/DisplaySkeleton.h"
#include "FbxSamples/ImportScene/DisplayMesh.h"
#include "FbxSamples/ImportScene/DisplayNurb.h"
#include "FbxSamples/ImportScene/DisplayPatch.h"
#include "FbxSamples/ImportScene/DisplayLodGroup.h"
#include "FbxSamples/ImportScene/DisplayCamera.h"
#include "FbxSamples/ImportScene/DisplayLight.h"
#include "FbxSamples/ImportScene/DisplayGlobalSettings.h"
#include "FbxSamples/ImportScene/DisplayPose.h"
#include "FbxSamples/ImportScene/DisplayPivotsAndLimits.h"
#include "FbxSamples/ImportScene/DisplayUserProperties.h"
#include "FbxSamples/ImportScene/DisplayGenericInfo.h"


static const string SSeperator("--------------------------------");
static const string SIndent("\t\t");

/************************************************************************************/
// parser

static void ParseControlsPoints(std::function<FJson&()> outputGetter, FbxMesh* mesh)
{
	if (mesh == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	int count = mesh->GetControlPointsCount();
	FbxVector4* controlPoints = mesh->GetControlPoints();
	for (int i = 0; i < count; ++i)
	{
		output.push_back(FJson());
		auto it = output.end() - 1;
		(*it)[K_INDEX] = i;
		WriteFloat3((*it)[K_COORDINATE], controlPoints[i]);
		for (int j = 0; j < mesh->GetElementNormalCount(); ++j)
		{
			FbxGeometryElementNormal* normal = mesh->GetElementNormal(j);
			if (normal->GetMappingMode() == FbxGeometryElement::eByControlPoint &&
				normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			{
				FJson vec;
				WriteFloat3(vec, normal->GetDirectArray().GetAt(i));
				(*it)[K_NORMAL].push_back(vec);
			}
		}
	}
}

static void ParsePolygons(std::function<FJson&()> outputGetter, FbxMesh* mesh)
{
	if (mesh == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	int count = mesh->GetPolygonCount();
	FbxVector4* controlPoints = mesh->GetControlPoints();
	FbxGeometryElementUV* uv0 = mesh->GetElementUV(0);
	int uvCount0 = uv0->GetDirectArray().GetCount();
	int vertexid = 0;
	for (int i = 0; i < count; ++i)
	{
		output.push_back(FJson());
		auto it = output.end() - 1;

		(*it)[K_INDEX] = i;
		for (int j = 0; j < mesh->GetElementPolygonGroupCount(); ++j)
		{
			FbxGeometryElementPolygonGroup* pg = mesh->GetElementPolygonGroup(j);
			switch (pg->GetMappingMode())
			{
			case FbxGeometryElement::eByPolygon:
			{
				if (pg->GetReferenceMode() == FbxGeometryElement::eIndex)
				{
					(*it)[K_GROUPID].push_back(pg->GetIndexArray().GetAt(i));
				}

				break;
			}
			}
		}

		int polygonSize = mesh->GetPolygonSize(i);
		for (int j = 0; j < polygonSize; ++j)
		{
			(*it)[K_POLYGON].push_back(FJson());
			auto it2 = (*it)[K_POLYGON].end() - 1;

			int index = mesh->GetPolygonVertex(i, j);
			WriteFloat3((*it2)[K_COORDINATE], controlPoints[index]);
			for (int k = 0; k < mesh->GetElementVertexColorCount(); ++k)
			{
				(*it2)[K_RGBA].push_back(FJson());
				auto it3 = (*it2)[K_RGBA].end() - 1;

				FbxGeometryElementVertexColor* vertexColor = mesh->GetElementVertexColor(k);
				switch (vertexColor->GetMappingMode())
				{
				case FbxGeometryElement::eByControlPoint:
				{
					switch (vertexColor->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						WriteRGBA(*it3, vertexColor->GetDirectArray().GetAt(index));
						break;
					case FbxGeometryElement::eIndexToDirect:
						int id = vertexColor->GetIndexArray().GetAt(index);
						WriteRGBA(*it3, vertexColor->GetDirectArray().GetAt(id));
						break;
					}

					break;
				}

				case FbxGeometryElement::eByPolygonVertex:
				{
					switch (vertexColor->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						WriteRGBA(*it3, vertexColor->GetDirectArray().GetAt(vertexid));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = vertexColor->GetIndexArray().GetAt(vertexid);
						WriteRGBA(*it3, vertexColor->GetDirectArray().GetAt(id));
						break;
					}
					default:
						break;
					}

					break;
				}

				case FbxGeometryElement::eByPolygon:
				case FbxGeometryElement::eAllSame:
				case FbxGeometryElement::eNone:
					break;
				}
			}

			int kcount = mesh->GetElementUVCount();
			for (int k = 0; k < kcount; ++k)
			{
				(*it2)[K_UV].push_back(FJson());
				auto it3 = (*it2)[K_UV].end() - 1;
				FbxGeometryElementUV* uv = mesh->GetElementUV(k);
				switch (uv->GetMappingMode())
				{
				case FbxGeometryElement::eByControlPoint:
				{
					switch (uv->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						WriteFloat2((*it3), uv->GetDirectArray().GetAt(index));
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = uv->GetIndexArray().GetAt(index);
						WriteFloat2(*it3, uv->GetDirectArray().GetAt(id));
						break;
					}
					default:
						break;
					}

					break;
				}
				case FbxGeometryElement::eByPolygonVertex:
				{
					int uvindex = mesh->GetTextureUVIndex(i, j);
					switch (uv->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						WriteFloat2(*it3, uv->GetDirectArray().GetAt(uvindex));
						break;
					}
					default:
						break;
					}

					break;
				}
				case FbxGeometryElement::eByPolygon:
				case FbxGeometryElement::eAllSame:
				case FbxGeometryElement::eNone:
					break;
				}
			}

			for (int k = 0; k < mesh->GetElementNormalCount(); ++k)
			{
				(*it2)[K_NORMAL].push_back(FJson());
				auto it3 = (*it2)[K_NORMAL].end() - 1;

				auto normal = mesh->GetElementNormal(k);
				if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (normal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						WriteFloat3(*it3, normal->GetDirectArray().GetAt(vertexid));
						break;
					case FbxGeometryElement::eIndexToDirect:
						WriteFloat3(*it3, normal->GetDirectArray().GetAt(normal->GetIndexArray().GetAt(vertexid)));
						break;
					default:
						break;
					}
				}
			}

			for (int k = 0; k < mesh->GetElementTangentCount(); ++k)
			{
				(*it2)[K_TANGENT].push_back(FJson());
				auto it3 = (*it2)[K_TANGENT].end() - 1;

				auto tangent = mesh->GetElementTangent(k);
				if (tangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (tangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						WriteFloat3(*it3, tangent->GetDirectArray().GetAt(vertexid));
						break;
					case FbxGeometryElement::eIndexToDirect:
						WriteFloat3(*it3, tangent->GetDirectArray().GetAt(tangent->GetIndexArray().GetAt(vertexid)));
						break;
					default:
						break;
					}
				}
			}

			for (int k = 0; k < mesh->GetElementBinormalCount(); ++k)
			{
				(*it2)[K_BINORMAL].push_back(FJson());
				auto it3 = (*it2)[K_BINORMAL].end() - 1;

				auto binormal = mesh->GetElementBinormal(k);
				if (binormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (binormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						WriteFloat3(*it3, binormal->GetDirectArray().GetAt(vertexid));
						break;
					case FbxGeometryElement::eIndexToDirect:
						WriteFloat3(*it3, binormal->GetDirectArray().GetAt(binormal->GetIndexArray().GetAt(vertexid)));
						break;
					default:
						break;
					}
				}
			}

			++vertexid;
		}
	}

	for (int i = 0; i < mesh->GetElementVisibilityCount(); ++i)
	{
		output[K_VISIBILITY].push_back(FJson());
		auto it = output[K_VISIBILITY].end() - 1;
		auto vis = mesh->GetElementVisibility(i);
		switch (vis->GetMappingMode())
		{
		case FbxGeometryElement::eByEdge:
			for (int j = 0; j != mesh->GetMeshEdgeCount(); ++j)
			{
				(*it)[K_EDGE].push_back(j);
				(*it)[K_EDGE_VISIBILITY].push_back(vis->GetDirectArray().GetAt(j)?1:0);
			}
		}
	}
}

static void ParseLink(std::function<FJson&()> outputGetter, FbxMesh* mesh)
{
	if (mesh == nullptr)
	{
		return;
	}

	int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
	FJson& output = outputGetter();
	output[K_COUNT] = skinCount;
	for (int i = 0; i < skinCount; ++i)
	{
		output[K_SKIN].push_back(FJson());
		auto it = output[K_SKIN].end() - 1;
		int clusterCount = ((FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
		for (int j = 0; j < clusterCount; ++j)
		{
			static const char* SModes[] = {K_NORMALIZE, K_ADDITIVE, K_TOTAL};
			(*it)[K_CLUSTER].push_back(FJson());
			auto it2 = (*it)[K_CLUSTER].end() - 1;

			auto cluster = ((FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);
			if (cluster->GetLink() != nullptr)
			{
				(*it2)[K_NAME] = cluster->GetLink()->GetName();
			}

			int cpIndexCount = cluster->GetControlPointIndicesCount();
			int* indices = cluster->GetControlPointIndices();
			double* weights = cluster->GetControlPointWeights();
			for (int k = 0; k < cpIndexCount; ++k)
			{
				(*it2)[K_LINK_INDICES].push_back(indices[k]);
				(*it2)[K_WEIGHT].push_back(weights[k]);
			}
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
	out[K_COUNT] = count;
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
	out[K_COUNT] = count;
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
					DisplayCurve([&]()->FJson& {return output[K_SHAPE_CHANNEL];}, geo->GetShapeChannel(i, j, layer, true));
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
				const char* name1 = isDT ? FBXSDK_CURVENODE_COLOR_RED		: "X";
				const char* name2 = isDT ? FBXSDK_CURVENODE_COLOR_GREEN		: "Y";
				const char* name3 = isDT ? FBXSDK_CURVENODE_COLOR_BLUE		: "Z";

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

static void ParseAnimationLayer(FJson& output, FbxAnimLayer* layer, FbxNode* node, bool bIsSwitcher)
{
	output[K_NAME] = node->GetName();

	ParseChannels(output, node, layer, ParseCurveKeys, ParseListCurveKeys, bIsSwitcher);

	for (int i = 0; i < node->GetChildCount(); ++i)
	{
		output[K_LAYER].push_back(FJson());
		auto it = output[K_LAYER].end() - 1;
		ParseAnimationLayer(*it, layer, node->GetChild(i), bIsSwitcher);
	}
}

static void ParseAnimationStack(FJson& output, FbxAnimStack* stack, FbxNode* node, bool bIsSwitcher)
{
	output[K_NAME] = stack->GetName();
	int count = stack->GetMemberCount<FbxAnimLayer>();
	output[K_COUNT] = count;
	for (int i = 0; i < count; ++i)
	{
		output[K_LAYER].push_back(FJson());
		auto it = output[K_LAYER].end() - 1;
		(*it)[K_INDEX] = i;
		auto layer = stack->GetMember<FbxAnimLayer>(i);
		ParseAnimationLayer(*it, layer, node, bIsSwitcher);
	}
}

static void ParseAnimation(FJson& output, FbxScene* scene)
{
	int count = scene->GetSrcObjectCount<FbxAnimStack>();
	output[K_COUNT] = count;
	for (int i = 0; i < count; ++i)
	{
		output[K_STACK].push_back(FJson());
		auto it = output[K_STACK].end() - 1;
		auto stack = scene->GetSrcObject<FbxAnimStack>(i);
		//ParseAnimationStack(*it, stack, scene->GetRootNode(), false);
		ParseAnimationStack(*it, stack, scene->GetRootNode(), false);
	}
}

static void ParseMetaDataConnections(std::function<FJson&()> outputGetter, FbxObject* object)
{
	if (object == nullptr)
	{
		return;
	}

	FJson& output = outputGetter();
	int count = object->GetSrcObjectCount<FbxObjectMetaData>();
	vector<string> names;
	for (int i = 0; i < count; ++i)
	{
		FbxObjectMetaData* data = object->GetSrcObject<FbxObjectMetaData>(i);
		names.push_back(data->GetName());
	}

	output[K_NAME] = names;
}

static void ParseMarker(FJson& output, FbxNode* node)
{
	FbxMarker* marker = (FbxMarker*)node->GetNodeAttribute();
	if (marker == nullptr)
	{
		return;
	}

	output[K_NAME] = node->GetName();
	ParseMetaDataConnections([&]()->FJson& {return output[K_METADATA_CONNECTION]; }, marker);

	switch (marker->GetType())
	{
	case FbxMarker::eStandard:		output[K_TYPE] = (int)EMarkerType::Standard; break;
	case FbxMarker::eOptical:		output[K_TYPE] = (int)EMarkerType::Optical; break;
	case FbxMarker::eEffectorIK:	output[K_TYPE] = (int)EMarkerType::IK_Effector; break;
	case FbxMarker::eEffectorFK:	output[K_TYPE] = (int)EMarkerType::FK_Effector; break;
	}

	switch (marker->Look.Get())
	{
	case FbxMarker::eCube:			output[K_LOOK] = (int)EMarkerLook::Cube; break;
	case FbxMarker::eSphere:		output[K_LOOK] = (int)EMarkerLook::Sphere; break;
	case FbxMarker::eHardCross:		output[K_LOOK] = (int)EMarkerLook::HardCross; break;
	case FbxMarker::eLightCross:	output[K_LOOK] = (int)EMarkerLook::LightCross; break;
	}

	output[K_SIZE] = marker->Size.Get();
	WriteFloat3(output[K_RGB], marker->Color.Get());
	WriteFloat3(output[K_IKPIVOT], marker->IKPivot.Get());
}

static void ParseSkeleton(FJson& output, FbxNode* node)
{
	FbxSkeleton* skeleton = (FbxSkeleton*)node->GetNodeAttribute();
	output[K_NAME] = node->GetName();
	ParseMetaDataConnections([&]()->FJson& {return output[K_METADATA_CONNECTION]; }, skeleton);

	const char* skeletonTypes[] = { K_ROOT, K_LIMB, K_LIMBNODE, K_EFFECTOR };
	output[K_TYPE] = skeletonTypes[skeleton->GetSkeletonType()];

	switch (skeleton->GetSkeletonType())
	{
	case FbxSkeleton::eLimb:			output[K_LENGTH] = skeleton->LimbLength.Get(); break;
	case FbxSkeleton::eLimbNode:		output[K_NODESIZE] = skeleton->Size.Get(); break;
	case FbxSkeleton::eRoot:			output[K_ROOTSIZE] = skeleton->Size.Get(); break;
	}

	WriteRGB(output[K_RGB], skeleton->GetLimbNodeColor());
}

static void ParseMesh(FJson& output, FbxNode* node)
{
	FbxMesh* mesh = (FbxMesh*)node->GetNodeAttribute();
	output[K_NAME] = node->GetName();
	ParseMetaDataConnections([&]()->FJson& {return output[K_METADATA_CONNECTION]; }, mesh);
	ParseControlsPoints([&]()->FJson& {return output[K_CONTROLPOINT]; }, mesh);
	ParsePolygons([&]()->FJson& {return output[K_POLYGON]; }, mesh);
	ParseLink([&]()->FJson& {return output[K_LINK]; }, mesh);
}

static void ParseNurbs(FJson& output, FbxNode* node)
{

}

static void ParsePatch(FJson& output, FbxNode* node)
{

}

static void ParseCamera(FJson& output, FbxNode* node)
{

}

static void ParseLight(FJson& output, FbxNode* node)
{

}

static void ParseLODGroup(FJson& output, FbxNode* node)
{

}


/************************************************************************************/


class FFbxImporter
{
public:
	static FFbxImporter* Get()
	{
		static FFbxImporter Inst;
		return &Inst;
	}

public:
	FFbxImporter()
		: SdkManager(nullptr)
		, SdkScene(nullptr)
	{
		InitializeSdkObjects(SdkManager, SdkScene);
	}

	void OutputToStream()
	{
		ofstream file;
		file.open(ConvertFile, ios::out);

		FJson json;
		json[K_META] = MetaData;
		json[K_NODE] = NodeData;
		json[K_POSE] = PoseData;
		json[K_ANIMATION] = AnimData;

		file << json << endl;

		file.close();
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

	void ImportNode(FbxNode* node)
	{
		NodeData.push_back(FJson());
		auto it = NodeData.end() - 1;

		FbxNodeAttribute::EType attrType;
		if (node->GetNodeAttribute() != nullptr)
		{
			attrType = node->GetNodeAttribute()->GetAttributeType();
			switch (attrType)
			{
			case FbxNodeAttribute::eMarker:
				(*it)[K_TYPE] = K_MARKER;
				ParseMarker(*it, node);
				break;
			case FbxNodeAttribute::eSkeleton:
				(*it)[K_TYPE] = K_SKELETON;
				ParseSkeleton(*it, node);
				break;
			case FbxNodeAttribute::eMesh:
				(*it)[K_TYPE] = K_MESH;
				ParseMesh(*it, node);
				break;
			case FbxNodeAttribute::eNurbs:
				(*it)[K_TYPE] = K_NURBS;
				ParseNurbs(*it, node);
				break;
			case FbxNodeAttribute::ePatch:
				(*it)[K_TYPE] = K_PATCH;
				ParsePatch(*it, node);
				break;
			case FbxNodeAttribute::eCamera:
				(*it)[K_TYPE] = K_CAMERA;
				ParseCamera(*it, node);
				break;
			case FbxNodeAttribute::eLight:
				(*it)[K_TYPE] = K_LIGHT;
				ParseLight(*it, node);
				break;
			case FbxNodeAttribute::eLODGroup:
				(*it)[K_TYPE] = K_LODGROUP;
				ParseLODGroup(*it, node);
				break;
			default:
				break;
			}
		}

		for (int i = 0; i < node->GetChildCount(); ++i)
		{
			ImportNode(node->GetChild(i));
		}
	}

	void ImportContent()
	{
		FbxNode* node = SdkScene->GetRootNode();
		if (node != nullptr)
		{
			for (int i = 0; i < node->GetChildCount(); ++i)
			{
				ImportNode(node->GetChild(i));
			}
		}
	}

	void ImportPose()
	{
		ParsePose(PoseData, SdkScene);
	}

	void ImportAnimation()
	{
		ParseAnimation(AnimData, SdkScene);
	}

	bool ImportScene(const string& importSrc, const string& convertDst)
	{
		ImportPath = importSrc;
		ConvertFile = convertDst;
		bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());

		if (result)
		{
			ImportMetaData();
			ImportContent();
			ImportPose();
			ImportAnimation();
			//DisplayGlobalLightSettings(&SdkScene->GetGlobalSettings());
		}

		OutputToStream();

		return false;
	}

private:
	FbxManager*			SdkManager;
	FbxScene*			SdkScene;

	string				ImportPath;
	string				ConvertFile;

	FJson				MetaData;
	FJson				NodeData;
	FJson				PoseData;
	FJson				AnimData;
};

bool Importer::ImportScene(const string& importSrc, const string& convertDst)
{
	return FFbxImporter::Get()->ImportScene(importSrc, convertDst);
}