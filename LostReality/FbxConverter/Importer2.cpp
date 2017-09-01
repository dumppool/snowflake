/*
* file Importer2.cpp
*
* author luoxw
* date 2017/06/29
*
* 
*/

#include "stdafx.h"

#define EXPORT_ORIGINAL_GEOMETRY 0
#define TEST_SOFT_SKINNED 0
#define IMPORT_ANIM_CURVE 0

using namespace Importer;
using namespace LostCore;

static void FindExtraSkeletonRecursively(const FPoseTreeAlias& tree, const set<string>& nameSet, vector<FPoseTreeAlias>& extra)
{
	if (nameSet.find(tree.Data.Name) == nameSet.end())
	{
		extra.push_back(tree);
	}

	auto numChildren = tree.GetChildCount();
	for (int32 childIndex = 0; childIndex < numChildren; ++childIndex)
	{
		FindExtraSkeletonRecursively(tree.GetChild(childIndex), nameSet, extra);
	}
}

static FbxTimeSpan GetAnimTimeSpan(FbxNode* node, FbxAnimStack* animStack)
{
	FbxTimeSpan timeSpan(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
	FbxTimeSpan timeSpan2(FBXSDK_TIME_INFINITE, FBXSDK_TIME_MINUS_INFINITE);
	timeSpan = animStack->GetLocalTimeSpan();
	node->GetAnimationInterval(timeSpan2, animStack);
	LVMSG("GetAnimTimeSpan", "Node:%s, Anim:%s, FbxAnimStack::GetLocalTimeSpan: %f, FbxNode::GetAnimationInterval: %f",
		node->GetName(), animStack->GetName(), timeSpan.GetDuration().GetSecondDouble(), timeSpan2.GetDuration().GetSecondDouble());

	return timeSpan;
}

static int32 GetAnimCurveSampleRate(FbxAnimCurve* curve)
{
	if (curve == nullptr)
	{
		return 0;
	}

	int32 numKeys = curve->KeyGetCount() - 1;
	if (numKeys < 1)
	{
		return 0;
	}

	FbxTimeSpan timeSpan;
	if (curve->GetTimeInterval(timeSpan))
	{
		auto seconds = timeSpan.GetDuration().GetSecondDouble();
		if (seconds == 0.0)
		{
			return 0;
		}

		return numKeys / seconds;
	}

	return 0;
}

static int32 GetMaxSampleRate(FbxAnimStack* animStack, const FbxTimeSpan& timeSpan, const vector<FbxNode*> sortedLinks)
{
	int32 sampleRate = 30;
	double animStart = timeSpan.GetStart().GetSecondDouble();
	double animStop = timeSpan.GetStop().GetSecondDouble();
	assert(animStack->GetMemberCount() > 0);
	FbxAnimLayer* animLayer = (FbxAnimLayer*)animStack->GetMember(0);
	for (int32 linkIndex = 0; linkIndex < sortedLinks.size(); ++linkIndex)
	{
		auto link = sortedLinks[linkIndex];
		FbxAnimCurve* curves[9];
		curves[0] = link->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		curves[1] = link->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		curves[2] = link->LclTranslation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		curves[3] = link->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		curves[4] = link->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		curves[5] = link->LclRotation.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		curves[6] = link->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_X, false);
		curves[7] = link->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Y, false);
		curves[8] = link->LclScaling.GetCurve(animLayer, FBXSDK_CURVENODE_COMPONENT_Z, false);
		for (int32 i = 0; i < 9; ++i)
		{
			if (curves[i] == nullptr)
			{
				continue;
			}

			sampleRate = max(sampleRate, GetAnimCurveSampleRate(curves[i]));
		}
	}

	return sampleRate;
}

static string GetProperName(const string& inName)
{
	string outName = inName;
	LostCore::ReplaceChar(outName, ":", "+");
	LostCore::ReplaceChar(outName, " ", "+");
	return outName;
}

struct FTempMesh
{
	FbxMesh*				Mesh;

	bool					bIsSkeletal;

	// link mode
	FbxCluster::ELinkMode	LinkMode; 
	vector<FbxNode*>		SortedLinks;

	FMeshDataAlias			MeshData;

	// 矩阵[node -> scene root]
	FFloat4x4					MeshToSceneRoot;

	// MeshToSceneRoot的旋转部分
	FFloat4x4					MeshToSceneRootDir;

	// geometry space
	vector<FFloat3>			ControlPoints;

	// Vertex index to polygon map
	// map<[control point index], map<[polygon index], [verex index]>>

#if TEST_SOFT_SKINNED
	// skinned points
	vector<FFloat3>			SkinnedPts;
#endif

#ifdef _DEBUG
	string					Name;
	string					ParentName;
	string					ParentTypeName;
#endif

	explicit FTempMesh(FbxMesh* mesh);

	bool IsValid() const;

	bool IsSkeletal() const;

	bool HasElementNormal() const;

	bool HasElementTangent() const;

	bool HasElementVertexColor() const;

	// 不考虑import整个场景的需求
	void ExtractVertex();

	// 标准化蒙皮混合权重
	void ProcessBlendWeight();

	// Geometry space >>> bone local space
	// 输出本地到骨骼的变换矩阵
	void ProcessSkeletalVertex(vector<FFloat4x4>& vecLocalToBone);

	void GenerateNormal(const vector<FFloat4x4>& localToBones, bool generateTangent = false);

	void ExtractSkeleton();

	FbxPose* RetrieveBindPose();

	bool BuildSkeletonBindPoseRecursively(FbxAMatrix* parentMatrix, FbxNode* link, FbxPose* pose, FPoseTreeAlias& treeNode);
	void CheckSkeletonRecursively(FbxNode* link, FbxNode* meshNode);
};

// Real curve data
struct FTempAnimStack
{
	FbxAnimStack* AnimStack;
	FAnimCurveData AnimData;
	string NameOverride;

	float SampleRate;
	float Length;

	explicit FTempAnimStack(FbxAnimStack* stack);
	~FTempAnimStack();

	void Extract(const vector<FbxNode*>& sortedLinks);
	void ParseLayer(FbxAnimLayer* layer, FbxNode* node);
	void ParseLayerInternal(FbxAnimLayer* layer, FbxNode* node);
	void ParseCurve(FRealCurveAlias& output, FbxAnimCurve* curve);
};

// Matrix curve data
struct FTempAnimStack2
{
	FbxAnimStack* AnimStack;
	vector<FbxNode*> SortedLinks;

	FAnimKeyFrameData AnimData;
	string NameOverride;

	explicit FTempAnimStack2(FbxAnimStack* stack);
	~FTempAnimStack2();

	void Extract(const vector<FbxNode*>& sortedLinks);
	void ParseLayerInternal(FbxAnimLayer* layer, FbxNode* node, const FbxTimeSpan& timeSpan, FMatrixCurveAlias& curve);
	void RetrieveSortedLinks();
};

class FFbxImporter2
{
public:
	static FFbxImporter2* Get()
	{
		static FFbxImporter2 Inst;
		return &Inst;
	}

public:
	FFbxImporter2();

	// 解析scene root节点，开始导入数据
	bool ImportSceneMeshes();

	// Import node
	void ImportNode(FbxNode* node);

	// Import mesh
	void ImportMesh(FbxMesh* mesh);

	// Import anim
	void ImportAnim();

private:
	FbxManager*			SdkManager;
	FbxScene*			SdkScene;
	vector<FFbxSkeleton>	Skeletons;

	string				DestDirectory;

	vector<FTempMesh>	TempMeshArray;
#if IMPORT_ANIM_CURVE
	vector<FTempAnimStack> TempAnimArray;
#else
	vector<FTempAnimStack2> TempAnimArray;
#endif
};

bool Importer::Import()
{
	return FFbxImporter2::Get()->ImportSceneMeshes();
}

FTempMesh::FTempMesh(FbxMesh * mesh)
	: Mesh(mesh)
	, bIsSkeletal(false)
#ifdef _DEBUG
	, Name("uninitialized")
	, ParentName("uninitialized")
	, ParentTypeName("uninitialized")
#endif
{
	ControlPoints.clear();
}

bool FTempMesh::IsValid() const
{
	return Mesh != nullptr;
}

bool FTempMesh::IsSkeletal() const
{
	return IsValid() && Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
}

bool FTempMesh::HasElementNormal() const
{
	return (MeshData.VertexFlags & EVertexElement::Normal) == EVertexElement::Normal;
}

bool FTempMesh::HasElementTangent() const
{
	return (MeshData.VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent;
}

bool FTempMesh::HasElementVertexColor() const
{
	return (MeshData.VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor;
}

void FTempMesh::ExtractVertex()
{
	const char* head = "ExtractVertex";

	if (!IsValid())
	{
		return;
	}

	auto scene = Mesh->GetScene();
	if (scene == nullptr)
	{
		LVERR(head, "mesh[%s] 's scene is not exist", Mesh->GetName());
		return;
	}

	if (!Mesh->IsTriangleMesh())
	{
		LVERR(head, "mesh[%s] is not a triangle mesh");
	}

	auto meshNode = Mesh->GetNode();
	auto parentNode = meshNode->GetParent();

#ifdef _DEBUG
	Name = GetProperName(Mesh->GetName());

	if (parentNode == nullptr)
	{
		ParentName = "non-exist";
	}
	else
	{
		ParentName = parentNode->GetName();
		ParentTypeName = parentNode->GetTypeName();
	}
#endif

	MeshData.Name = GetProperName(Mesh->GetName());

	MeshData.VertexFlags = 0;
	if (IsSkeletal())
	{
		MeshData.VertexFlags |= EVertexElement::Skin;
	}

	if (MeshData.Name.empty() && !MeshData.Skeleton.Data.Name.empty())
	{
		MeshData.Name = MeshData.Skeleton.Data.Name;
	}

	auto layer0 = Mesh->GetLayer(0);
	if (layer0 == nullptr)
	{
		LVERR(head, "mesh[%s] has no layer", Mesh->GetName());
		return;
	}

	// Element heads
	auto vcHead = FConvertOptions::Get()->bImportVertexColor ? layer0->GetVertexColors() : nullptr;
	auto normalHead = FConvertOptions::Get()->bImportNormal ? layer0->GetNormals() : nullptr;
	auto tangentHead = FConvertOptions::Get()->bImportTangent ? layer0->GetTangents() : nullptr;
	auto binormalHead = FConvertOptions::Get()->bImportTangent ? layer0->GetBinormals() : nullptr;
	//auto uvHead = layer0->GetUVSets();
	auto uvHead = FConvertOptions::Get()->bImportTexCoord ? Mesh->GetElementUV(0) : nullptr;
	auto coordHead = Mesh->GetControlPoints();
	auto coordCount = Mesh->GetControlPointsCount();
	auto polygonCount = Mesh->GetPolygonCount();

	FbxLayerElement::EReferenceMode normalRM, tangentRM, binormalRM, uvRM, vcRM;
	FbxLayerElement::EMappingMode normalMM, tangentMM, binormalMM, uvMM, vcMM;

	if (normalHead != nullptr && (normalMM = normalHead->GetMappingMode()) != FbxLayerElement::eNone)
	{
		normalRM = normalHead->GetReferenceMode();
	}
	else
	{
		normalHead = nullptr;
	}

	if (tangentHead != nullptr && (tangentMM = tangentHead->GetMappingMode()) != FbxLayerElement::eNone)
	{
		tangentRM = tangentHead->GetReferenceMode();
	}
	else
	{
		tangentHead = nullptr;
	}

	if (binormalHead != nullptr && (binormalMM = binormalHead->GetMappingMode()) != FbxLayerElement::eNone)
	{
		binormalRM = binormalHead->GetReferenceMode();
	}
	else
	{
		binormalHead = nullptr;
	}

	if (uvHead != nullptr && (uvMM = uvHead->GetMappingMode()) != FbxLayerElement::eNone)
	{
		uvRM = uvHead->GetReferenceMode();
		FbxStringList uvNames;
		Mesh->GetUVSetNames(uvNames);
		MeshData.TexCoordName = uvNames[0];
		if (uvNames.GetCount() != 1)
		{
			LVERR(head, "This mesh has %d uv set.", uvNames.GetCount());
		}
	}
	else
	{
		uvHead = nullptr;
	}

	if (vcHead != nullptr && (vcMM = vcHead->GetMappingMode()) != FbxLayerElement::eNone)
	{
		vcRM = vcHead->GetReferenceMode();
	}
	else
	{
		vcHead = nullptr;
	}

	MeshData.VertexFlags |= (coordHead != nullptr ? EVertexElement::Coordinate : 0);
	MeshData.VertexFlags |= (normalHead != nullptr ? EVertexElement::Normal : 0);
	MeshData.VertexFlags |= (tangentHead != nullptr  && binormalHead != nullptr ? EVertexElement::Tangent : 0);
	MeshData.VertexFlags |= (uvHead != nullptr ? EVertexElement::UV : 0);
	MeshData.VertexFlags |= (vcHead != nullptr ? EVertexElement::VertexColor : 0);

	if (coordHead == nullptr)
	{
		LVERR(head, "mesh[%s] has no control points", Mesh->GetName());
		return;
	}

	MeshData.VertexCount = coordCount;
	auto mat = scene->GetAnimationEvaluator()->GetNodeGlobalTransform(meshNode);
	mat = mat * GetGeometry(meshNode);
	bool oddNegativeScale = IsOddNegativeScale(mat);
	MeshToSceneRoot = ToMatrix(mat);
	auto rotMat = mat.Inverse().Transpose();
	MeshToSceneRootDir = ToMatrix(rotMat);
	for (int i = 0; i < coordCount; ++i)
	{
		//mat.MultT(coordHead[i])
		ControlPoints.push_back(ToVector3(mat.MultT(coordHead[i])));
	}

	for (int i = 0; i < coordCount; ++i)
	{
		if (normalHead != nullptr && normalMM == FbxLayerElement::eByControlPoint)
		{
			int valIndex = normalRM == FbxLayerElement::eDirect ? i : normalHead->GetIndexArray().GetAt(i);
			MeshData.Normals.push_back(ToVector3(rotMat.MultT(normalHead->GetDirectArray().GetAt(valIndex))).GetNormal());
		}

		if (tangentHead != nullptr && tangentMM == FbxLayerElement::eByControlPoint)
		{
			int valIndex = tangentRM == FbxLayerElement::eDirect ? i : tangentHead->GetIndexArray().GetAt(i);
			MeshData.Tangents.push_back(ToVector3(tangentHead->GetDirectArray().GetAt(valIndex)).GetNormal());
		}

		if (binormalHead != nullptr && binormalMM == FbxLayerElement::eByControlPoint)
		{
			int valIndex = binormalRM == FbxLayerElement::eDirect ? i : binormalHead->GetIndexArray().GetAt(i);
			MeshData.Binormals.push_back(ToVector3(binormalHead->GetDirectArray().GetAt(valIndex)).GetNormal());
		}

		if (uvHead != nullptr && uvMM == FbxLayerElement::eByControlPoint)
		{
			int valIndex = uvRM == FbxLayerElement::eDirect ? i : uvHead->GetIndexArray().GetAt(i);
			MeshData.TexCoords.push_back(ToVector2(uvHead->GetDirectArray().GetAt(valIndex)));
		}
	}

	if (Mesh->IsTriangleMesh())
	{
		MeshData.Triangles.resize(polygonCount);
		for (int i = 0; i < polygonCount; ++i)
		{
			auto& tri = MeshData.Triangles[i];
			for (int j = 0; j < 3; ++j)
			{
				int idx = oddNegativeScale ? (2 - j) : j;
				int cpIndex = Mesh->GetPolygonVertex(i, j);
				int tmpIndex = i * 3 + j;

				auto& vert = tri.Vertices[idx];
				vert.Index = cpIndex;

				if (MeshData.VertexPolygonMap.find(cpIndex) == MeshData.VertexPolygonMap.end())
				{
					MeshData.VertexPolygonMap[cpIndex] = map<uint32, uint32>();
				}

				MeshData.VertexPolygonMap[cpIndex][i] = idx;

				if (normalHead != nullptr && normalMM != FbxLayerElement::eByControlPoint)
				{
					int valIndex = normalRM == FbxLayerElement::eDirect ? tmpIndex : normalHead->GetIndexArray().GetAt(tmpIndex);
					vert.Normal = ToVector3(normalHead->GetDirectArray().GetAt(valIndex)).GetNormal();

					if (normalRM != FbxLayerElement::eDirect)
					{
						auto n0 = normalHead->GetDirectArray().GetAt(valIndex);
						bool ttt;
						FbxVector4 n1;
						Mesh->GetPolygonVertexNormal(i, idx, n1);
						assert(n0 == n1);
						assert(0);
					}
				}

				if (tangentHead != nullptr && tangentMM != FbxLayerElement::eByControlPoint)
				{
					int valIndex = tangentRM == FbxLayerElement::eDirect ? tmpIndex : tangentHead->GetIndexArray().GetAt(tmpIndex);
					vert.Tangent = ToVector3(tangentHead->GetDirectArray().GetAt(valIndex)).GetNormal();
				}

				if (binormalHead != nullptr && binormalMM != FbxLayerElement::eByControlPoint)
				{
					int valIndex = binormalRM == FbxLayerElement::eDirect ? tmpIndex : binormalHead->GetIndexArray().GetAt(tmpIndex);
					vert.Binormal = ToVector3(binormalHead->GetDirectArray().GetAt(valIndex)).GetNormal();
				}

				if (uvHead != nullptr && uvMM != FbxLayerElement::eByControlPoint)
				{
					int valIndex = uvRM == FbxLayerElement::eDirect ? tmpIndex : uvHead->GetIndexArray().GetAt(tmpIndex);
					vert.TexCoord = ToVector2(uvHead->GetDirectArray().GetAt(valIndex));
				}

				//if (vcHead != nullptr && vcMM != FbxLayerElement::eByControlPoint)
				//{
					//int valIndex = vcRM == FbxLayerElement::eDirect ? tmpIndex : vcHead->GetIndexArray().GetAt(tmpIndex);
					//vert.Color =
				//}
			}
		}

		if ((ControlPoints.size() != MeshData.BlendIndices.size() || ControlPoints.size() != MeshData.BlendWeights.size()))
		{
			LVERR(head, "Size of control points mismatch in mesh[%s]: cp[%d], bi[%d], bw[%d]", Mesh->GetName(),
				ControlPoints.size(), MeshData.BlendIndices.size(), MeshData.BlendWeights.size());
		}

		MeshData.Coordinates = ControlPoints;
		vector<FFloat4x4> vecLocalToBone(0);
		if (IsSkeletal())
		{
			// 标准化蒙皮权重
			ProcessBlendWeight();

			// 顶点坐标转换到本地空间
			//ProcessSkeletalVertex(vecLocalToBone);
		}

		bool genNormal = (normalHead == nullptr
			&& FConvertOptions::Get()->bGenerateNormalIfNotFound)
			|| (FConvertOptions::Get()->bForceRegenerateNormal);

		bool genTangent = (tangentHead == nullptr
			&& FConvertOptions::Get()->bGenerateTangentIfNotFound)
			|| (FConvertOptions::Get()->bForceRegenerateTangent);

		genTangent &= genNormal || (normalHead != nullptr);
		genTangent &= (MeshData.VertexFlags & EVertexElement::UV) == EVertexElement::UV;

		if (genNormal)
		{
			GenerateNormal(vecLocalToBone, genTangent);
		}

		MeshData.VertexFlags |= genNormal ? EVertexElement::Normal : 0;
		MeshData.VertexFlags |= genTangent ? EVertexElement::Tangent : 0;
	}
}

void FTempMesh::ProcessBlendWeight()
{
	for (uint32 i = 0; i < MeshData.BlendWeights.size(); ++i)
	{
		auto& indices = MeshData.BlendIndices[i];
		auto& weights = MeshData.BlendWeights[i];
		float w = 0.f;
		for (uint32 j = 0; j < 4; ++j)
		{
			if (indices[j] != -1)
			{
				w += weights[j];
			}
			else
			{
				break;
			}
		}

		assert(!IsZero(w));

		for (uint32 j = 0; j < 4; ++j)
		{
			if (indices[j] != -1)
			{
				weights[j] /= w;
			}
			else
			{
				break;
			}
		}
	}
}

void FTempMesh::ProcessSkeletalVertex(vector<FFloat4x4>& vecLocalToBone)
{
	if (ControlPoints.size() == 0)
	{
		return;
	}

	//MeshData.Coordinates = ControlPoints;
	//return;

#if TEST_SOFT_SKINNED
	vector<FFloat3> skinned0(ControlPoints.size());
	vector<FFloat3> skinned1(ControlPoints.size());
	SkinnedPts.resize(ControlPoints.size());
#endif

	FPoseMapAlias globalPose;

	LostCore::FFloat4x4 globalMat;
	globalMat.SetIdentity();
	//GetLocalPoseMap(globalMat, MeshData.Skeleton, globalPose, initialPose);
	//GetGlobalPoseMap(globalMat, MeshData.Skeleton, MeshData.BindPose, globalPose);

	map<int32, string> inverseMap;
	for_each(MeshData.SkeletonIndexMap.begin(), MeshData.SkeletonIndexMap.end(),
		[&](const pair<string, int32>& elem) {inverseMap[elem.second] = elem.first; });

	vecLocalToBone.resize(ControlPoints.size());
	MeshData.Coordinates.resize(ControlPoints.size());
	for (uint32 vertIndex = 0; vertIndex < ControlPoints.size(); ++vertIndex)
	{
		auto& indices = MeshData.BlendIndices[vertIndex];
		auto& weights = MeshData.BlendWeights[vertIndex];
		FFloat4x4 mat2;
		memset(&mat2, 0, sizeof(mat2));
		float w = 0.f;

		for (uint32 j = 0; j < MAX_BONES_PER_VERTEX; ++j)
		{
			auto index = indices[j];
			auto val = weights[j];

			if (index == -1)
			{
				break;
			}

			w += val;
			const auto& n = inverseMap[index];
			auto tmp2 = globalPose[n];
			mat2 = mat2 + globalPose[n] * val;
		}

		assert(IsEqual(w, 1.f, LostCore::SSmallFloat2));

		auto invMat2 = mat2;
		invMat2.Invert();
		vecLocalToBone[vertIndex] = invMat2;
		MeshData.Coordinates[vertIndex] = invMat2.ApplyPoint(ControlPoints[vertIndex]);

#if TEST_SOFT_SKINNED
		skinned0[vertIndex] = mat2.ApplyPoint(MeshData.Coordinates[vertIndex]);
#endif

		if (LinkMode == FbxCluster::eNormalize)
		{
			MeshData.Coordinates[vertIndex] *= 1.f / w;
		}
		else if (LinkMode == FbxCluster::eTotalOne)
		{
			MeshData.Coordinates[vertIndex] += ControlPoints[vertIndex] * (1.f - w);
		}
	}

#if TEST_SOFT_SKINNED
	SkinnedPts = skinned0;
	//for (uint32 i = 0; i < ControlPoints.size(); ++i)
	//{
	//	auto& indices = MeshData.BlendIndices[i];
	//	auto& weights = MeshData.BlendWeights[i];
	//	FFloat4x4 totalMat;
	//	memset(&totalMat, 0, sizeof(totalMat));
	//	float totalWeight = 0;
	//	for (uint32 j = 0; j < MAX_BONES_PER_VERTEX; ++j)
	//	{
	//		if (indices[j] == -1)
	//		{
	//			break;
	//		}

	//		auto index = (int32)floor(indices[j] + SSmallFloat2);
	//		totalMat = totalMat + globalPose[inverseMap[index]] * weights[j];
	//		totalWeight += weights[j];

	//		skinned1[i] += globalPose[inverseMap[index]].ApplyPoint(MeshData.Coordinates[i]) * weights[j];
	//	}

	//	//totalMat = totalMat * (1.f / totalWeight);
	//	skinned0[i] = totalMat.ApplyPoint(MeshData.Coordinates[i]);
	//}
	//if (skinned0[0] != skinned1[0])
	//	printf("");
#endif
}

void FTempMesh::GenerateNormal(const vector<FFloat4x4>& vecLocalToBone, bool generateTangent)
{
	//assert((vecLocalToBone.size() > 0) == IsSkeletal());

	vector<FMeshDataAlias::FTriangle> triangles(MeshData.Triangles.size());
	for (uint32 i = 0; i < MeshData.Triangles.size(); ++i)
	{
		auto& tri = MeshData.Triangles[i];
		auto& p0 = ControlPoints[tri.Vertices[0].Index];
		auto& p1 = ControlPoints[tri.Vertices[1].Index];
		auto& p2 = ControlPoints[tri.Vertices[2].Index];

		FFloat3 nx = (p2 - p0).GetNormal();
		FFloat3 nz = (p1 - p0).GetNormal();
		FFloat3 ny = nz.Cross(nx).GetNormal();

		auto& tri2 = triangles[i];
		tri2.Vertices[0].Normal = tri2.Vertices[1].Normal = tri2.Vertices[2].Normal = ny;

		if (generateTangent)
		{
			FFloat4x4 ntb;
			ntb.SetRow(0, FFloat4(nx, 0.0));
			ntb.SetRow(1, FFloat4(ny, 0.0));
			ntb.SetRow(2, FFloat4(nz, 0.0));
			ntb.SetRow(3, FFloat4(0.0, 0.0, 0.0, 0.0));

			bool splitUV = MeshData.TexCoords.size() == 0;
			auto& uv0 = splitUV ? tri.Vertices[0].TexCoord : MeshData.TexCoords[tri.Vertices[0].Index];
			auto& uv1 = splitUV ? tri.Vertices[1].TexCoord : MeshData.TexCoords[tri.Vertices[1].Index];
			auto& uv2 = splitUV ? tri.Vertices[2].TexCoord : MeshData.TexCoords[tri.Vertices[2].Index];
			FFloat2 uvx = uv2 - uv0;
			FFloat2 uvz = uv1 - uv0;
			FFloat4x4 tex;
			tex.SetRow(0, FFloat4(uvx.X, 0.0, uvx.Y, 0.0));
			tex.SetRow(1, FFloat4(0.0, 1.0, 0.0, 0.0));
			tex.SetRow(2, FFloat4(uvz.X, 0.0, uvz.Y, 0.0));
			tex.SetRow(3, FFloat4(0.0, 0.0, 0.0, 1.0));

			ntb = tex.Invert() * ntb;
			nx = ntb.ApplyVector(FFloat3(1.0, 0.0, 0.0)).GetNormal();
			ny = ntb.ApplyVector(FFloat3(0.0, 1.0, 0.0)).GetNormal();
			nz = ntb.ApplyVector(FFloat3(0.0, 0.0, 1.0)).GetNormal();

			tri2.Vertices[0].Binormal = tri2.Vertices[1].Tangent = tri2.Vertices[2].Tangent = nx;
			tri2.Vertices[0].Normal = tri2.Vertices[1].Normal = tri2.Vertices[2].Normal = ny;
			tri2.Vertices[0].Tangent = tri2.Vertices[1].Binormal = tri2.Vertices[2].Binormal = nz;
		}
	}

	if (FConvertOptions::Get()->bMergeNormalTangentAll)
	{
		MeshData.Normals.resize(ControlPoints.size());
		MeshData.Tangents.resize(ControlPoints.size());
		MeshData.Binormals.resize(ControlPoints.size());
		for (uint32 i = 0; i < ControlPoints.size(); ++i)
		{
			FFloat3 normal(0.0, 0.0, 0.0);
			FFloat3 tangent(0.0, 0.0, 0.0);
			FFloat3 binormal(0.0, 0.0, 0.0);
			auto& indices = MeshData.VertexPolygonMap[i];
			for (auto& polygonIndex : indices)
			{
				normal += triangles[polygonIndex.first].Vertices[polygonIndex.second].Normal;
				if (generateTangent)
				{
					tangent += triangles[polygonIndex.first].Vertices[polygonIndex.second].Tangent;
					binormal += triangles[polygonIndex.first].Vertices[polygonIndex.second].Binormal;
				}
			}

			normal *= (1.0 / indices.size());
			if (generateTangent)
			{
				tangent *= (1.0 / indices.size());
				binormal *= (1.0 / indices.size());
			}

			MeshData.Normals[i] = (normal.GetNormal());

			if (generateTangent)
			{
				MeshData.Tangents[i] = (tangent.GetNormal());
				MeshData.Binormals[i] = (binormal.GetNormal());
			}

			//MeshData.Normals[i] = vecLocalToBone[i].ApplyVector(normal.GetNormal());

			//if (generateTangent)
			//{
			//	MeshData.Tangents[i] = vecLocalToBone[i].ApplyVector(tangent);
			//	MeshData.Binormals[i] = vecLocalToBone[i].ApplyVector(binormal.GetNormal());
			//}
		}
	}
	else // 不合并法线，同一点上在不同三角面的顶点都有自己的法线.
	{
		for (uint32 i = 0; i < triangles.size(); ++i)
		{
			for (uint32 j = 0; j < 3; ++j)
			{
				auto& srcVert = triangles[i].Vertices[j];
				auto& dstVert = MeshData.Triangles[i].Vertices[j];
				//dstVert.Normal = vecLocalToBone[dstVert.Index].ApplyVector(srcVert.Normal.GetNormal());
				//if (generateTangent)
				//{
				//	dstVert.Tangent = vecLocalToBone[dstVert.Index].ApplyVector(srcVert.Tangent.GetNormal());
				//	dstVert.Binormal = vecLocalToBone[dstVert.Index].ApplyVector(srcVert.Binormal.GetNormal());
				//}
				dstVert.Normal = (srcVert.Normal.GetNormal());
				if (generateTangent)
				{
					dstVert.Tangent = (srcVert.Tangent.GetNormal());
					dstVert.Binormal = (srcVert.Binormal.GetNormal());
				}
			}
		}

		MeshData.Normals.clear();
		if (generateTangent)
		{
			MeshData.Tangents.clear();
			MeshData.Binormals.clear();
		}
	}
}

void FTempMesh::ExtractSkeleton()
{
	const char* head = "FTempMesh::ExtractSkeleton";

	FbxPose* bindPose = nullptr;
	if ((bindPose = RetrieveBindPose()) == nullptr)
	{
		Mesh->GetFbxManager()->CreateMissingBindPoses(Mesh->GetScene());
		if ((bindPose = RetrieveBindPose()) == nullptr)
		{
			LVERR(head, "%s: recreating bind pose failed", Mesh->GetName());
		}
		else
		{
			LVMSG(head, "%s: recreating bind pose succeeded", Mesh->GetName());
		}
	}

	assert(bindPose != nullptr && "use anim's first frame as bind pose???");

	auto skinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
	assert(skinCount == 1);

	// fill blend indices & weights
	int cpCount = Mesh->GetControlPointsCount();

	MeshData.BlendIndices.resize(cpCount);
	for_each(MeshData.BlendIndices.begin(), MeshData.BlendIndices.end(), [](FSInt4& arr) {arr = FSInt4(-1, -1, -1, -1); });

	MeshData.BlendWeights.resize(cpCount);
	for_each(MeshData.BlendWeights.begin(), MeshData.BlendWeights.end(), [](FFloat4& arr) {arr = FFloat4(INFINITY, INFINITY, INFINITY, INFINITY); });

	auto skin = (FbxSkin*)Mesh->GetDeformer(0, FbxDeformer::eSkin);
	int clusterCount = skin->GetClusterCount();
	for (int i = 0; i < clusterCount; ++i)
	{
		auto cluster = skin->GetCluster(i);

		auto link = cluster->GetLink();
		if (IsBone(link))
		{
			MeshData.SkeletonIndexMap[GetProperName(link->GetName())] = i;
			LVMSG(head, "Processing cluster: %s", link->GetName());
		}
		else
		{
			LVERR(head, "Cluster[%s] is not a bone", cluster->GetName());
			continue;
		}

		LinkMode = cluster->GetLinkMode();
		if (LinkMode == FbxCluster::eAdditive)
		{
			LVERR(head, "mesh[%s] link[%s] mode: additive", Mesh->GetName(), link->GetName());
		}

		int cpIndexCount = cluster->GetControlPointIndicesCount();
		auto indices = cluster->GetControlPointIndices();
		auto weights = cluster->GetControlPointWeights();
		for (int j = 0; j < cpIndexCount; ++j)
		{
			auto index = indices[j];
			auto& outputIndices = MeshData.BlendIndices[index];
			auto& outputWeights = MeshData.BlendWeights[index];

			auto weight = weights[j];

			float totalWeight = 0.f;
			for (int k = 0; k < 4; ++k)
			{
				if (outputIndices[k] < 0)
				{
					outputIndices[k] = (float)i;
					outputWeights[k] = (float)weight;
					break;
				}

				totalWeight += outputWeights[k];
			}
		}
	} //for (int i = 0; i < clusterCount; ++i)

	SortSkeletalMeshLink(Mesh, SortedLinks);
	BuildSkeletonBindPoseRecursively(nullptr, SortedLinks[0], bindPose, MeshData.Skeleton);
}

FbxPose * FTempMesh::RetrieveBindPose()
{
	const char* head = "FTempMesh::RetrieveBindPose";
	auto scene = Mesh->GetScene();
	auto numPoses = scene->GetPoseCount();
	FbxPose* validPose = nullptr;
	for (int32 poseIndex = 0; poseIndex < numPoses; ++poseIndex)
	{
		auto pose = scene->GetPose(poseIndex);
		if (pose != nullptr && pose->IsBindPose())
		{
			auto poseName = pose->GetName();
			auto meshName = Mesh->GetName();
			FbxStatus status;
			NodeList missingAncestors, missingDeformers, missingDeformersAncestors, wrongMatrices;
			if (pose->IsValidBindPoseVerbose(Mesh->GetNode(), missingAncestors,
				missingDeformers, missingDeformersAncestors, wrongMatrices, 0.0001, &status))
			{
				validPose = pose;
				break;
			}
			else
			{
				for (int32 i = 0; i < missingAncestors.GetCount(); ++i)
				{
					auto mat = missingAncestors.GetAt(i)->EvaluateGlobalTransform(FBXSDK_TIME_ZERO);
					pose->Add(missingAncestors.GetAt(i), mat);
				}

				missingAncestors.Clear();
				missingDeformers.Clear();
				missingDeformersAncestors.Clear();
				wrongMatrices.Clear();
				if (pose->IsValidBindPose(Mesh->GetNode()))
				{
					validPose = pose;
					break;
				}
				else
				{
					auto parent = Mesh->GetNode()->GetParent();
					while (parent != nullptr)
					{
						auto attr = parent->GetNodeAttribute();
						if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eNull)
						{
							break;
						}

						parent = parent->GetParent();
					}

					if (parent != nullptr && pose->IsValidBindPose(parent))
					{
						validPose = pose;
					}
					else
					{
						LVERR(head, "Failed to find valid bind pose for mesh: %s", Mesh->GetName());
					}
				}
			}
		}
	}

	return validPose;
}

bool FTempMesh::BuildSkeletonBindPoseRecursively(FbxAMatrix* parentMatrix, FbxNode * link, FbxPose* pose, FPoseTreeAlias & treeNode)
{
	if (pose == nullptr)
	{
		return false;
	}

	FbxAMatrix* globalmat = nullptr;
	FbxAMatrix localmat;
	globalmat = parentMatrix;
	localmat.SetIdentity();

	if (IsBone(link))
	{
		auto linkIndex = pose->Find(link);
		if (linkIndex >= 0)
		{
			auto fbxmat = pose->GetMatrix(linkIndex);
			globalmat = (FbxAMatrix*)(&fbxmat);
			localmat = *globalmat;
			if (parentMatrix != nullptr)
			{
				localmat = parentMatrix->Inverse() * (*globalmat);
			}

			LVMSG("FTempMesh::BuildSkeletonBindPoseRecursively", "Skeleton %s is added", link->GetName());
		}
	}
	else
	{
		LVMSG("FTempMesh::BuildSkeletonBindPoseRecursively", "%s: link[%s] is not a bone", Mesh->GetName(), link->GetName());
	}

	treeNode.Data.Matrix = ToMatrix(localmat);
	treeNode.Data.Name = GetProperName(link->GetName());

	auto numChildren = link->GetChildCount();
	for (auto childIndex = 0; childIndex < numChildren; ++childIndex)
	{
		FPoseTreeAlias childNode;
		auto childLink = link->GetChild(childIndex);
		BuildSkeletonBindPoseRecursively(globalmat, childLink, pose, childNode);
		treeNode.AddChild(childNode);
	}

	return true;
}

void FTempMesh::CheckSkeletonRecursively(FbxNode * link, FbxNode * meshNode)
{
	if (link == nullptr)
	{
		return;
	}

	auto numChildren = link->GetChildCount();
	for (int32 childIndex = 0; childIndex < numChildren; ++childIndex)
	{
		CheckSkeletonRecursively(link->GetChild(childIndex), meshNode);
	}

	if (!IsBone(link))
	{
		FbxSkeleton* skel = FbxSkeleton::Create(meshNode->GetFbxManager(), "");
		link->SetNodeAttribute(skel);
		skel->SetSkeletonType(FbxSkeleton::eLimbNode);
	}
}

FFbxImporter2::FFbxImporter2()
	: SdkManager(nullptr)
	, SdkScene(nullptr)
{
	InitializeSdkObjects(SdkManager, SdkScene);
}

bool FFbxImporter2::ImportSceneMeshes()
{
	const char* head = "ImportSceneMeshes";
	bool result = LoadScene(SdkManager, SdkScene, FConvertOptions::Get()->InputPath.c_str());
	if (!result || SdkManager == nullptr || SdkScene == nullptr)
	{
		LVERR(head, "LoadScene failed");
		return false;
	}

	/*************************************************************/
#if !CONVERT_FROM_RIGHTHAND
	const FbxAxisSystem targetAxis(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityEven, FbxAxisSystem::eRightHanded);
	const FbxAxisSystem sourceAxis = SdkScene->GetGlobalSettings().GetAxisSystem();

	if (sourceAxis != targetAxis)
	{
		FbxRootNodeUtility::RemoveAllFbxRoots(SdkScene);
		targetAxis.ConvertScene(SdkScene);
	}

	if (SdkScene->GetGlobalSettings().GetSystemUnit() != FbxSystemUnit::cm)
	{
		FbxSystemUnit::cm.ConvertScene(SdkScene);
	}
#endif

	FbxSystemUnit sysUnit = SdkScene->GetGlobalSettings().GetSystemUnit();
	auto p0 = sysUnit.GetMultiplier();
	auto p1 = sysUnit.GetScaleFactorAsString();
	//if (sysUnit.GetScaleFactor() != 100.0)
	{
		//The unit in this example is centimeter.
		//FbxSystemUnit::m.ConvertScene(SdkScene);
		//FbxSystemUnit::km.ConvertScene(SdkScene);
	}
	/*************************************************************/

	//DisplayHierarchy(SdkScene);
	//DisplayAnimation(SdkScene);

	auto formatDst = FConvertOptions::Get()->OutputPath;
	ReplaceChar(formatDst, "/", "\\");
	GetDirectory(DestDirectory, formatDst);

	ImportNode(SdkScene->GetRootNode());

	if (FConvertOptions::Get()->bImportAnimation)
	{
		ImportAnim();
	}

	FJson j;
	FJson& meshSection = j[K_MESH];
	FJson& animSection = j[K_ANIMATION];

	for (auto& mesh : TempMeshArray)
	{
		mesh.ExtractSkeleton();
		mesh.ExtractVertex();
	}

	for (const auto& mesh : TempMeshArray)
	{
		meshSection.push_back(FJson());
		FJson& meshJson = *(meshSection.end() - 1);
		meshJson[K_PATH] = mesh.MeshData.Save(DestDirectory);
		meshJson[K_VERTEX_ELEMENT] = mesh.MeshData.VertexFlags;

		FMeshDataAlias tm;
		tm.Load(meshJson[K_PATH]);
	}

	//for (const auto& anim : TempAnimArray)
	//{
	//	animSection.push_back(FJson());
	//	FJson& animJson = *(animSection.end() - 1);
	//	animJson[K_PATH] = anim.AnimData.Save(DestDirectory);
	//	animJson[K_NAME] = anim.AnimData.Name;
	//}

	for (auto& anim : TempAnimArray)
	{
		if (Skeletons.size() > 0)
		{
			vector<FbxNode*> sortedLinks;
			SortLinkRecursively(Skeletons[0].Data, sortedLinks);
			anim.Extract(sortedLinks);

			animSection.push_back(FJson());
			FJson& animJson = *(animSection.end() - 1);
			animJson[K_PATH] = anim.AnimData.Save(DestDirectory);
			animJson[K_NAME] = anim.AnimData.Name;
		}
	}

	ofstream stream(FConvertOptions::Get()->OutputPath);
	stream << j;
	stream.close();


	return true;
}

void FFbxImporter2::ImportNode(FbxNode * node)
{
	const char* head = "ImportNode";

	if (node == nullptr)
	{
		return;
	}

	if (node != SdkScene->GetRootNode())
	{
		auto attr = node->GetNodeAttribute();
		if (attr != nullptr)
		{
			auto attrType = attr->GetAttributeType();
			switch (attrType)
			{
			case FbxNodeAttribute::eMesh:
			{
				auto mesh = node->GetMesh();
				if (mesh != nullptr)
				{
					ImportMesh(mesh);
				}

				break;
			}
			case FbxNodeAttribute::eSkeleton:
			{
				if (node->GetParent() == nullptr || !IsBone(node->GetParent()))
				{
					FFbxSkeleton skelTree;
					BuildFbxSkeleton(node, nullptr, skelTree);
					Skeletons.push_back(skelTree);
				}

				break;
			}
			default:
			{
				LVERR(head, " %s[type: %s] hasnt been considered.", attr->GetName(), SAttributeTypeString[attrType]);

				break;
			}
			}
		}
	}

	auto numChildren = node->GetChildCount();
	for (int childIndex = 0; childIndex < numChildren; ++childIndex)
	{
		ImportNode(node->GetChild(childIndex));
	}
}

void FFbxImporter2::ImportMesh(FbxMesh * mesh)
{
	if (mesh == nullptr)
	{
		return;
	}

	TempMeshArray.push_back(FTempMesh(mesh));
}

void FFbxImporter2::ImportAnim()
{
	auto numAnimStacks = SdkScene->GetSrcObjectCount<FbxAnimStack>();
	for (uint32 i = 0; i < numAnimStacks; ++i)
	{
#if IMPORT_ANIM_CURVE
		FTempAnimStack stack(SdkScene->GetSrcObject<FbxAnimStack>(i));
#else
		FTempAnimStack2 stack(SdkScene->GetSrcObject<FbxAnimStack>(i));
#endif
		TempAnimArray.push_back(stack);
	}

	// UE4导出的动画FbxAnimStack::GetName总是"Unreal Take"，不能用作转换输出动画名，
	// 但是如果动画fbx文件包含不止一个FbxAnimStack时，这样会覆盖原本应该合适的名字.
	if (TempAnimArray.size() == 1)
	{
		TempAnimArray[0].NameOverride = FConvertOptions::Get()->InputFileNameNoExt;
	}
}

FTempAnimStack::FTempAnimStack(FbxAnimStack * stack)
	: AnimStack(stack)
{
}

FTempAnimStack::~FTempAnimStack()
{
}

void FTempAnimStack::Extract(const vector<FbxNode*>& sortedLinks)
{
	assert(AnimStack != nullptr);

	AnimData.Name = GetProperName(NameOverride.empty() ? AnimStack->GetName() : NameOverride);
	auto numAnimLayers = AnimStack->GetMemberCount<FbxAnimLayer>();
	for (uint32 i = 0; i < numAnimLayers; ++i)
	{
		ParseLayer(AnimStack->GetMember<FbxAnimLayer>(i), AnimStack->GetScene()->GetRootNode());
	}
}

void FTempAnimStack::ParseLayer(FbxAnimLayer * layer, FbxNode * node)
{
	ParseLayerInternal(layer, node);
	auto numChildren = node->GetChildCount();
	for (uint32 i = 0; i < numChildren; ++i)
	{
		ParseLayer(layer, node->GetChild(i));
	}
}

void FTempAnimStack::ParseLayerInternal(FbxAnimLayer * layer, FbxNode * node)
{
	FbxAnimCurve* curve = nullptr;
	string nodeName = GetProperName(node->GetName());
	AnimData.CurveMap[nodeName] = map<string, FRealCurveAlias>();

	curve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	if (curve != nullptr)
	{
		string compName(K_ROTATE_X);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (curve != nullptr)
	{
		string compName(K_ROTATE_Y);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclRotation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (curve != nullptr)
	{
		string compName(K_ROTATE_Z);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	if (curve != nullptr)
	{
		string compName(K_TRANSLATE_X);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (curve != nullptr)
	{
		string compName(K_TRANSLATE_Y);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclTranslation.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (curve != nullptr)
	{
		string compName(K_TRANSLATE_Z);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X);
	if (curve != nullptr)
	{
		string compName(K_SCALE_X);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y);
	if (curve != nullptr)
	{
		string compName(K_SCALE_Y);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}

	curve = node->LclScaling.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if (curve != nullptr)
	{
		string compName(K_SCALE_Z);
		AnimData.CurveMap[nodeName][compName] = FRealCurveAlias();
		ParseCurve(AnimData.CurveMap[nodeName][compName], curve);
	}
}

void FTempAnimStack::ParseCurve(FRealCurveAlias & output, FbxAnimCurve * curve)
{
	if (curve == nullptr)
	{
		return;
	}

	uint32 numKeys = curve->KeyGetCount();
	if (numKeys == 0)
	{
		return;
	}

	FbxTimeSpan timeSpan;
	curve->GetTimeInterval(timeSpan);
	auto sec = timeSpan.GetDuration().GetSecondDouble();
	for (uint32 i = 0; i < numKeys; ++i)
	{
		auto keyTime = curve->KeyGetTime(i).GetSecondDouble() * sec;
		auto value = curve->KeyGetValue(i);
		output.AddKey(keyTime, value);
	}

	auto curveWrapMode = FRealCurveAlias::EWrap::Wrap;
	auto curveInterpolationMode = FRealCurveAlias::EInterpolation::Constant;
	auto interpolation = curve->KeyGetInterpolation(0);
	if ((interpolation & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
	{
		curveInterpolationMode = FRealCurveAlias::EInterpolation::Constant;
	}
	else if ((interpolation & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
	{
		curveInterpolationMode = FRealCurveAlias::EInterpolation::CatmullRom;
	}
	else if ((interpolation & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear)
	{
		curveInterpolationMode = FRealCurveAlias::EInterpolation::Linear;
	}

	if (curve->KeyGetValue(0) == curve->KeyGetValue(numKeys - 1))
	{
		curveWrapMode = FRealCurveAlias::EWrap::Wrap;
	}

	output.SetMode(curveWrapMode, curveInterpolationMode);
}

FTempAnimStack2::FTempAnimStack2(FbxAnimStack * stack) : AnimStack(stack)
{
}

FTempAnimStack2::~FTempAnimStack2()
{
}

void FTempAnimStack2::Extract(const vector<FbxNode*>& sortedLinks)
{
	SortedLinks = sortedLinks;
	auto timeSpan = AnimStack->GetLocalTimeSpan();
	auto scene = AnimStack->GetScene();
	AnimData.Name = GetProperName(NameOverride.empty()?AnimStack->GetName():NameOverride);
	AnimData.SampleRate = GetMaxSampleRate(AnimStack, timeSpan, SortedLinks);
	AnimData.Length = timeSpan.GetDuration().GetSecondDouble();
	AnimData.NumKeys = AnimData.Length * AnimData.SampleRate;

	scene->SetCurrentAnimationStack(AnimStack);
	AnimStack->BakeLayers(scene->GetAnimationEvaluator(), timeSpan.GetStart(), timeSpan.GetStop(), timeSpan.GetDuration() / AnimData.NumKeys);
	assert(AnimStack->GetMemberCount() > 0);
	FbxAnimLayer* layer = (FbxAnimLayer*)AnimStack->GetMember(0);
	for (int32 linkIndex = 0; linkIndex < SortedLinks.size(); ++linkIndex)
	{
		FMatrixCurveAlias curve;
		curve.SetMode(FMatrixCurveAlias::EWrap::Wrap, FMatrixCurveAlias::EInterpolation::Constant);
		ParseLayerInternal(layer, SortedLinks[linkIndex], timeSpan, curve);
		AnimData.KeyFrameMap[GetProperName(SortedLinks[linkIndex]->GetName())] = curve;
	}
}

void FTempAnimStack2::ParseLayerInternal(FbxAnimLayer * layer, FbxNode * node, const FbxTimeSpan& timeSpan, FMatrixCurveAlias& curve)
{
	FbxNode* parent = node->GetParent();
	const FbxTime interval = timeSpan.GetDuration() / AnimData.NumKeys;
	for (FbxTime cur = timeSpan.GetStart(); cur < timeSpan.GetStop(); cur += interval)
	{
		FbxAMatrix parentGlobal, local;
		parentGlobal.SetIdentity();
		if (parent != nullptr && IsBone(parent))
		{
			parentGlobal = parent->EvaluateGlobalTransform(cur);
		}

		local = node->EvaluateGlobalTransform(cur);
		local = parentGlobal.Inverse() * local;
		curve.AddKey(cur.GetSecondDouble(), ToMatrix(local));
	}
}

void FTempAnimStack2::RetrieveSortedLinks()
{
	//SortSkeletalMeshLink()
}
