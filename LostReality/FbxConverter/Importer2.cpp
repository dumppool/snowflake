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
	void ProcessSkeletalVertex();
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

	//void DumpNodeRecursively(std::function<FJson&()> outputGetter, FbxNode* node)
	//{
	//	FJson& output = outputGetter();
	//	auto attr = node->GetNodeAttribute();
	//	auto mesh = node->GetMesh();
	//	output[K_NAME] = node->GetName();
	//	output[K_TYPENAME] = node->GetTypeName();

	//	output[K_ATTRIBUTE_TYPE] = "null_a";
	//	if (attr != nullptr)
	//	{
	//		auto attrType = attr->GetAttributeType();
	//		output[K_ATTRIBUTE_TYPE] = SAttributeTypeString[(int)attrType];
	//	}

	//	if (mesh != nullptr)
	//	{
	//		auto deformerCount = mesh->GetDeformerCount();
	//		output[K_DEFORMER_COUNT] = deformerCount;
	//		if (deformerCount > 0)
	//		{
	//			auto deformer = mesh->GetDeformer(0);
	//			output[K_DEFORMER_NAME] = deformer->GetName();
	//			output[K_DEFORMER_TYPE] = deformer->GetTypeName();
	//		}
	//	}

	//	for (int i = 0; i < node->GetChildCount(); ++i)
	//	{
	//		DumpNodeRecursively([&]()->FJson& {output[K_CHILDREN].push_back(FJson()); return *(output[K_CHILDREN].end() - 1); }, node->GetChild(i));
	//	}
	//}

	// Dump，暂时不考虑
	bool DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);

	// 解析scene root节点，开始导入数据
	bool ImportSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);

	// Import node
	void ImportNode(FbxNode* node);

	// Import mesh
	void ImportMesh(FbxMesh* mesh);

private:
	FbxManager*			SdkManager;
	FbxScene*			SdkScene;

	string				DestDirectory;

	vector<FTempMesh>	TempMeshArray;
};

bool Importer::DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation)
{
	return FFbxImporter2::Get()->DumpSceneMeshes(importSrc, convertDst, outputBinary, exportAnimation);
}

bool Importer::ImportSceneMeshes2(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation)
{
	return FFbxImporter2::Get()->ImportSceneMeshes(importSrc, convertDst, outputBinary, exportAnimation);
}


inline FTempMesh::FTempMesh(FbxScene * scene, FbxMesh * mesh)
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

inline bool FTempMesh::IsValid() const
{
	return Mesh != nullptr;
}

inline bool FTempMesh::IsSkeletal() const
{
	return IsValid() && Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
}

inline bool FTempMesh::HasElementNormal() const
{
	return (MeshData.VertexFlags & EVertexElement::Normal) == EVertexElement::Normal;
}

inline bool FTempMesh::HasElementTangent() const
{
	return (MeshData.VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent;
}

inline bool FTempMesh::HasElementVertexColor() const
{
	return (MeshData.VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor;
}

inline void FTempMesh::Extract()
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
		}

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
	auto vcHead = SOptions.bImportVertexColor ? layer0->GetVertexColors() : nullptr;
	auto normalHead = SOptions.bImportNormal ? layer0->GetNormals() : nullptr;
	auto tangentHead = SOptions.bImportTangent ? layer0->GetTangents() : nullptr;
	auto binormalHead = SOptions.bImportTangent ? layer0->GetBinormals() : nullptr;
	//auto uvHead = layer0->GetUVSets();
	auto uvHead = Mesh->GetElementUV(0);
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

				if (normalHead != nullptr && normalMM != FbxLayerElement::eByControlPoint)
				{
					int valIndex = normalRM == FbxLayerElement::eDirect ? tmpIndex : normalHead->GetIndexArray().GetAt(tmpIndex);
					vert.Normal = ToVector3(normalHead->GetDirectArray().GetAt(valIndex)).GetNormal();
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

		// 标准化蒙皮权重
		ProcessBlendWeight();

		// 顶点坐标转换到本地空间
		ProcessSkeletalVertex();
	}
}

inline void FTempMesh::ProcessBlendWeight()
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

inline void FTempMesh::ProcessSkeletalVertex()
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

	MeshData.Coordinates.resize(ControlPoints.size());

	for (uint32 vertIndex = 0; vertIndex < ControlPoints.size(); ++vertIndex)
	{
		auto& indices = MeshData.BlendIndices[vertIndex];
		auto& weights = MeshData.BlendWeights[vertIndex];
		FbxAMatrix mat;
		memset(&mat, 0, sizeof(mat));
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


inline FFbxImporter2::FFbxImporter2()
	: SdkManager(nullptr)
	, SdkScene(nullptr)
{
	InitializeSdkObjects(SdkManager, SdkScene);
}

inline bool FFbxImporter2::DumpSceneMeshes(const string & importSrc, const string & convertDst, bool outputBinary, bool exportAnimation)
{
	//bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());

	return true;
}

inline bool FFbxImporter2::ImportSceneMeshes(const string & importSrc, const string & convertDst, bool outputBinary, bool exportAnimation)
{
	const char* head = "ImportSceneMeshes";
	bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());
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

	auto formatDst = convertDst;
	ReplaceChar(formatDst, "/", "\\");
	GetDirectory(DestDirectory, formatDst);

	ImportNode(SdkScene->GetRootNode());

	if (TempMeshArray.size() == 1)
	{
		TempMeshArray[0].MeshData.Save(formatDst);
	}
	else
	{
		for (const auto& mesh : TempMeshArray)
		{
			mesh.MeshData.Save(DestDirectory);

			//string f = DestDirectory + mesh.MeshData.Name + "." + K_PRIMITIVE;
			//FMeshDataAlias test;
			//test.Load(f);
			//assert(test == mesh.MeshData);
		}
	}

	return true;
}

inline void FFbxImporter2::ImportNode(FbxNode * node)
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

inline void FFbxImporter2::ImportMesh(FbxMesh * mesh)
{
	if (mesh == nullptr)
	{
		return;
	}

	TempMeshArray.push_back(FTempMesh(SdkScene, mesh));
}
