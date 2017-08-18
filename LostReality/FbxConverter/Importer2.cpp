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

using namespace Importer;
using namespace LostCore;


struct FTempMesh
{
	FbxScene*				Scene;
	FbxMesh*				Mesh;

	bool					bIsSkeletal;

	// link mode
	FbxCluster::ELinkMode	LinkMode;

	FPoseMapAlias			DefaultPose;

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

	FTempMesh(FbxScene* scene, FbxMesh* mesh);

	bool IsValid() const;

	bool IsSkeletal() const;

	bool HasElementNormal() const;

	bool HasElementTangent() const;

	bool HasElementVertexColor() const;

	// 不考虑import整个场景的需求
	void Extract();

	// 标准化蒙皮混合权重
	void ProcessBlendWeight();

	// Geometry space >>> bone local space
	// 输出本地到骨骼的变换矩阵
	void ProcessSkeletalVertex(vector<FFloat4x4>& vecLocalToBone);

	void GenerateNormal(const vector<FFloat4x4>& localToBones, bool generateTangent = false);
};

struct FTempAnimStack
{
	FbxScene* Scene;
	FbxAnimStack* AnimStack;
	FAnimData AnimData;

	FTempAnimStack(FbxScene* scene, FbxAnimStack* stack);
	~FTempAnimStack();

	void Extract();

	void ParseLayer(FbxAnimLayer* layer, FbxNode* node);
	void ParseLayerInternal(FbxAnimLayer* layer, FbxNode* node);
	void ParseCurve(FRealCurveAlias& output, FbxAnimCurve* curve);
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

	string				DestDirectory;

	vector<FTempMesh>	TempMeshArray;
	vector<FTempAnimStack> TempAnimArray;
};

bool Importer::Import()
{
	return FFbxImporter2::Get()->ImportSceneMeshes();
}


FTempMesh::FTempMesh(FbxScene * scene, FbxMesh * mesh)
	: Scene(scene)
	, Mesh(mesh)
	, bIsSkeletal(false)
#ifdef _DEBUG
	, Name("uninitialized")
	, ParentName("uninitialized")
	, ParentTypeName("uninitialized")
#endif
{
	ControlPoints.clear();

	Extract();
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

void FTempMesh::Extract()
{
	const char* head = "Extract";

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
	Name = Mesh->GetName();

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

	MeshData.Name = Mesh->GetName();

	MeshData.VertexFlags = 0;
	if (IsSkeletal())
	{
		bIsSkeletal = true;
		MeshData.VertexFlags |= EVertexElement::Skin;

		int skinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
		assert(skinCount == 1);

		// fill blend indices & weights
		int cpCount = Mesh->GetControlPointsCount();

		MeshData.BlendIndices.resize(cpCount);
		for_each(MeshData.BlendIndices.begin(), MeshData.BlendIndices.end(), [](FSInt4& arr) {arr = FSInt4(-1, -1, -1, -1); });

		MeshData.BlendWeights.resize(cpCount);
		for_each(MeshData.BlendWeights.begin(), MeshData.BlendWeights.end(), [](FFloat4& arr) {arr = FFloat4(INFINITY, INFINITY, INFINITY, INFINITY); });

		MeshData.Poses[K_INITIAL_POSE] = FPoseMap();

		set<FbxNode*> rootLinks;
		auto skin = (FbxSkin*)Mesh->GetDeformer(0, FbxDeformer::eSkin);
		int clusterCount = skin->GetClusterCount();
		for (int i = 0; i < clusterCount; ++i)
		{
			auto cluster = skin->GetCluster(i);

			auto link = cluster->GetLink();
			if (IsBone(link))
			{
				MeshData.SkeletonIndexMap[link->GetName()] = i;
			}
			else
			{
				LVERR(head, "cluster[%s] is not a bone", cluster->GetName());
				continue;
			}

			LinkMode = cluster->GetLinkMode();
			if (LinkMode == FbxCluster::eAdditive)
			{
				LVERR(head, "mesh[%s] link[%s] mode: additive", Mesh->GetName(), link->GetName());
			}

			DefaultPose[link->GetName()] = ToMatrix(GetNodeLocalMatrix(link));

			auto rootLink = GetRootLink(link);
			if (rootLink != nullptr && rootLinks.find(rootLink) == rootLinks.end())
			{
				rootLinks.insert(rootLink);
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

		if (rootLinks.size() != 1)
		{
			LVERR(head, "mesh[%s] has %d root", Mesh->GetName(), rootLinks.size());
			return;
		}

		// Use the first link only
		auto link0 = *(rootLinks.begin());

		// Get skeleton
		BuildSkeletonTree(link0, MeshData.Skeleton);

		// Get the default

		// Get all the poses from fbx
		for (int i = 0; i < scene->GetPoseCount(); ++i)
		{
			auto pose = scene->GetPose(i);
			if (pose == nullptr)
			{
				continue;
			}

			int linkIndex = pose->Find(link0);
			bool bValid = linkIndex >= 0;

			NodeList missingAncestors, missingDeformers, missingDeformersAncestors, wrongMatrices;
			if (bValid && pose->IsValidBindPoseVerbose(meshNode, missingAncestors,
				missingDeformers, missingDeformersAncestors, wrongMatrices))
			{
				bValid = true;
			}
			else
			{
				bValid = false;

				// TODO: 修复
			}

			if (bValid)
			{
				pose->GetMatrix(linkIndex);
				FPoseTreeAlias poseTree;
				FPoseTreeAlias poseTree2;
				FPoseMapAlias poseMap;
				BuildPoseTree(pose, link0, poseTree, poseTree2);
				GetPoseMapFromTree(poseTree, poseMap);
				MeshData.Poses[pose->GetName()] = poseMap;
			}
		}
	}

	if (MeshData.Name.empty() && !MeshData.Skeleton.Data.empty())
	{
		MeshData.Name = MeshData.Skeleton.Data;
		LostCore::ReplaceChar(MeshData.Name, ":", "+");
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
	auto mat = ComputeMatrixLocalToParent(meshNode, scene->GetRootNode());
	bool oddNegativeScale = IsOddNegativeScale(mat);
	MeshToSceneRoot = ToMatrix(mat);
	MeshToSceneRootDir = ToMatrix(mat.Inverse().Transpose());
	for (int i = 0; i < coordCount; ++i)
	{
		//mat.MultT(coordHead[i])
		ControlPoints.push_back(ToVector3(coordHead[i]));
	}

	for (int i = 0; i < coordCount; ++i)
	{
		if (normalHead != nullptr && normalMM == FbxLayerElement::eByControlPoint)
		{
			int valIndex = normalRM == FbxLayerElement::eDirect ? i : normalHead->GetIndexArray().GetAt(i);
			MeshData.Normals.push_back(ToVector3(normalHead->GetDirectArray().GetAt(valIndex)).GetNormal());
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

		vector<FFloat4x4> vecLocalToBone(0);
		if (IsSkeletal())
		{
			// 标准化蒙皮权重
			ProcessBlendWeight();

			// 顶点坐标转换到本地空间
			ProcessSkeletalVertex(vecLocalToBone);
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

#if TEST_SOFT_SKINNED
	vector<FFloat3> skinned0(ControlPoints.size());
	vector<FFloat3> skinned1(ControlPoints.size());
	SkinnedPts.resize(ControlPoints.size());
#endif

	FPoseMapAlias globalPose;
	MeshData.Poses[K_INITIAL_POSE] = DefaultPose;
	auto& initialPose = MeshData.Poses[K_INITIAL_POSE];

	LostCore::FFloat4x4 globalMat;
	globalMat.SetIdentity();
	GetGlobalPoseMap(globalMat, MeshData.Skeleton, initialPose, globalPose);

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

#ifdef _DEBUG
		if (vertIndex == 817)
		{
			printf("");
		}
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
	assert((vecLocalToBone.size() > 0) == IsSkeletal());

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

			MeshData.Normals[i] = vecLocalToBone[i].ApplyVector(normal.GetNormal());

			if (generateTangent)
			{
				MeshData.Tangents[i] = vecLocalToBone[i].ApplyVector(tangent);
				MeshData.Binormals[i] = vecLocalToBone[i].ApplyVector(binormal.GetNormal());
			}
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
				dstVert.Normal = vecLocalToBone[dstVert.Index].ApplyVector(srcVert.Normal.GetNormal());
				if (generateTangent)
				{
					dstVert.Tangent = vecLocalToBone[dstVert.Index].ApplyVector(srcVert.Tangent.GetNormal());
					dstVert.Binormal = vecLocalToBone[dstVert.Index].ApplyVector(srcVert.Binormal.GetNormal());
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
	FbxAxisSystem axisSys = SdkScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem dstAxisSys(FbxAxisSystem::eDirectX);
	if (axisSys != dstAxisSys)
	{
		dstAxisSys.ConvertScene(SdkScene);
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

	for (const auto& mesh : TempMeshArray)
	{
		meshSection.push_back(FJson());
		FJson& meshJson = *(meshSection.end() - 1);
		meshJson[K_PATH] = mesh.MeshData.Save(DestDirectory);
		meshJson[K_VERTEX_ELEMENT] = mesh.MeshData.VertexFlags;
	}

	for (const auto& anim : TempAnimArray)
	{
		animSection.push_back(FJson());
		FJson& animJson = *(animSection.end() - 1);
		animJson[K_PATH] = anim.AnimData.Save(DestDirectory);
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

	TempMeshArray.push_back(FTempMesh(SdkScene, mesh));
}

void FFbxImporter2::ImportAnim()
{
	auto numAnimStacks = SdkScene->GetSrcObjectCount<FbxAnimStack>();
	for (uint32 i = 0; i < numAnimStacks; ++i)
	{
		FTempAnimStack stack(SdkScene, SdkScene->GetSrcObject<FbxAnimStack>(i));
		TempAnimArray.push_back(stack);
	}
}

FTempAnimStack::FTempAnimStack(FbxScene * scene, FbxAnimStack * stack)
	: Scene(scene), AnimStack(stack)
{
	Extract();
}

FTempAnimStack::~FTempAnimStack()
{
}

void FTempAnimStack::Extract()
{
	assert(Scene != nullptr && AnimStack != nullptr);

	AnimData.Name = AnimStack->GetName();
	auto numAnimLayers = AnimStack->GetMemberCount<FbxAnimLayer>();
	for (uint32 i = 0; i < numAnimLayers; ++i)
	{
		ParseLayer(AnimStack->GetMember<FbxAnimLayer>(i), Scene->GetRootNode());
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
	string nodeName = node->GetName();
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
	uint32 numKeys = curve->KeyGetCount();
	if (numKeys == 0)
	{
		return;
	}

	for (uint32 i = 0; i < numKeys; ++i)
	{
		auto keyTime = curve->KeyGetTime(i).GetSecondDouble();
		auto value = curve->KeyGetValue(i);
		output.AddKey(keyTime, value);
	}

	auto curveWrapMode = FRealCurveAlias::EWrap::Clamp;
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
