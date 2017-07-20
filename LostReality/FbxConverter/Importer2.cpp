/*
* file Importer2.cpp
*
* author luoxw
* date 2017/06/29
*
* 
*/

#include "stdafx.h"

using namespace Importer;
using namespace LostCore;


struct FTempMesh
{
	struct FVertex
	{
		FVec2				TexCoords;
		FVec3				Normals;
		FVec3				Tangents;
		FVec3				Binormals;
		uint32				ControlPointIndices;
		//uint8				MaterialIndices[2];
		//uint32			SmoothingGroups;
		FVec3				Colors;
	};

	struct FTriangle
	{
		FVertex				Vertices[3];
	};

	FbxScene*				Scene;
	FbxMesh*				Mesh;

	bool					bIsSkeletal;

	// 骨骼的FbxAMatrix
	map<int32, FbxAMatrix> SkelAMatrix;

	// link mode
	FbxCluster::ELinkMode	LinkMode;

	// 骨骼
	FSkeletonTreeAlias		Skeleton;

	// poses
	map<string, FPoseMapAlias>	Poses;

	// 骨骼索引表
	map<string, int32>		SkeletonIndexMap;

	// 顶点结构
	uint32					VertexFlags;

	// 矩阵[node -> scene root]
	FMatrix					MeshToSceneRoot;

	// MeshToSceneRoot的旋转部分
	FMatrix					MeshToSceneRootDir;

	// control points都在mesh node本地空间
	vector<FVec3>			ControlPoints;

	vector<FVec3>			ControlPoints2;

	// 三角面
	vector<FTriangle>		Triangles;

	// 骨骼模型才有的骨骼索引&骨骼混合权重
	vector<array<float, MAX_BONES_PER_VERTEX>>	BlendIndices;
	vector<array<float, MAX_BONES_PER_VERTEX>>	BlendWeights;

#ifdef _DEBUG
	string					Name;
	string					ParentName;
	string					ParentTypeName;
#endif

	FTempMesh(FbxScene* scene, FbxMesh* mesh)
		: Scene(scene)
		, Mesh(mesh)
		, bIsSkeletal(false)
#ifdef _DEBUG
		, Name("uninitialized")
		, ParentName("uninitialized")
		, ParentTypeName("uninitialized")
#endif
	{
		SkelAMatrix.clear();
		Poses.clear();
		SkeletonIndexMap.clear();
		ControlPoints.clear();
		Triangles.clear();
		BlendIndices.clear();
		BlendWeights.clear();

		Extract();
	}

	bool IsValid() const
	{
		return Mesh != nullptr;
	}

	bool IsSkeletal() const
	{
		return IsValid() && Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	}

	bool HasElementNormal() const
	{
		return (VertexFlags & EVertexElement::Normal) == EVertexElement::Normal;
	}

	bool HasElementTangent() const
	{
		return (VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent;
	}

	bool HasElementVertexColor() const
	{
		return (VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor;
	}

	// 不考虑import整个场景的需求
	void Extract()
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

		VertexFlags = 0;
		if (IsSkeletal())
		{
			bIsSkeletal = true;
			VertexFlags |= EVertexElement::Skin;

			int skinCount = Mesh->GetDeformerCount(FbxDeformer::eSkin);
			assert(skinCount == 1);

			// fill blend indices & weights
			int cpCount = Mesh->GetControlPointsCount();
			
			BlendIndices.resize(cpCount);
			for_each(BlendIndices.begin(), BlendIndices.end(), [](array<float, MAX_BONES_PER_VERTEX>& arr) {arr.fill(-1); });
			
			BlendWeights.resize(cpCount);
			for_each(BlendWeights.begin(), BlendWeights.end(), [](array<float, MAX_BONES_PER_VERTEX>& arr) {arr.fill(INFINITY); });

			set<FbxNode*> rootLinks;
			auto skin = (FbxSkin*)Mesh->GetDeformer(0, FbxDeformer::eSkin);
			int clusterCount = skin->GetClusterCount();
			for (int i = 0; i < clusterCount; ++i)
			{
				auto cluster = skin->GetCluster(i);

				auto link = cluster->GetLink();
				if (IsBone(link))
				{
					SkeletonIndexMap[link->GetName()] = i;
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

				SkelAMatrix[SkeletonIndexMap[link->GetName()]] = GetInitialClusterMatrix(Mesh, cluster);

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
					auto& outputIndices = BlendIndices[index];
					auto& outputWeights = BlendWeights[index];

					auto weight = weights[j];

					for (int k = 0; k < MAX_BONES_PER_VERTEX; ++k)
					{
						if (outputIndices[k] < 0)
						{
							outputIndices[k] = (float)i;
							outputWeights[k] = (float)weight;
							break;
						}
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
			BuildSkeletonTree(link0, Skeleton);

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
					FPoseMapAlias poseMap;
					BuildPoseTree(pose, link0, poseTree);
					GetPoseMapFromTree(poseTree, poseMap);
					Poses[pose->GetName()] = poseMap;
				}
			}
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

		VertexFlags |= (coordHead != nullptr ? EVertexElement::Coordinate : 0);
		VertexFlags |= (normalHead != nullptr ? EVertexElement::Normal : 0);
		VertexFlags |= (tangentHead != nullptr  && binormalHead != nullptr ? EVertexElement::Tangent : 0);
		VertexFlags |= (uvHead != nullptr ? EVertexElement::UV : 0);
		VertexFlags |= (vcHead != nullptr ? EVertexElement::VertexColor : 0);

		if (coordHead == nullptr)
		{
			LVERR(head, "mesh[%s] has no control points", Mesh->GetName());
			return;
		}

		auto mat = ComputeMatrixLocalToParent(meshNode, scene->GetRootNode());
		bool oddNegativeScale = IsOddNegativeScale(mat);
		MeshToSceneRoot = ToMatrix(mat);
		MeshToSceneRootDir = ToMatrix(mat.Inverse().Transpose());
		for (int i = 0; i < coordCount; ++i)
		{
			//mat.MultT(coordHead[i])
			ControlPoints.push_back(ToVector3(coordHead[i]));
		}

		FbxLayerElement::EReferenceMode referenceMode;
		FbxLayerElement::EMappingMode mappingMode;
		if (Mesh->IsTriangleMesh())
		{
			Triangles.resize(polygonCount);
			for (int i = 0; i < polygonCount; ++i)
			{
				auto& tri = Triangles[i];
				for (int j = 0; j < 3; ++j)
				{
					int idx = oddNegativeScale ? (2 - j) : j;
					int cpIndex = Mesh->GetPolygonVertex(i, j);
					int tmpIndex = i * 3 + j;

					auto& vert = tri.Vertices[idx];

					vert.ControlPointIndices = cpIndex;
					if (normalHead != nullptr)
					{
						referenceMode = normalHead->GetReferenceMode();
						mappingMode = normalHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eByControlPoint ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eDirect ? mapIndex : normalHead->GetIndexArray().GetAt(mapIndex);
						vert.Normals = ToVector3(normalHead->GetDirectArray().GetAt(valIndex));
						vert.Normals.Normalize();
					}

					if (tangentHead != nullptr)
					{
						referenceMode = tangentHead->GetReferenceMode();
						mappingMode = tangentHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eByControlPoint ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eDirect ? mapIndex : tangentHead->GetIndexArray().GetAt(mapIndex);
						vert.Tangents = ToVector3(tangentHead->GetDirectArray().GetAt(valIndex));
						vert.Tangents.Normalize();
					}

					if (binormalHead != nullptr)
					{
						referenceMode = binormalHead->GetReferenceMode();
						mappingMode = binormalHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eByControlPoint ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eDirect ? mapIndex : binormalHead->GetIndexArray().GetAt(mapIndex);
						vert.Binormals = ToVector3(binormalHead->GetDirectArray().GetAt(valIndex));
						vert.Binormals.Normalize();
					}

					// TODO: 考虑多重uv
					if (uvHead != nullptr)
					{
						referenceMode = uvHead->GetReferenceMode();
						mappingMode = uvHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eByControlPoint ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eDirect ? mapIndex : uvHead->GetIndexArray().GetAt(mapIndex);
						vert.TexCoords = ToVector2(uvHead->GetDirectArray().GetAt(valIndex));
					}

					// TODO: 没有Color类，暂不导入
					//if (vcHead != nullptr)
					//{
					//	referenceMode = vcHead->GetReferenceMode();
					//	mappingMode = vcHead->GetMappingMode();
					//	int mapIndex = referenceMode == FbxLayerElement::eByControlPoint ? cpIndex : tmpIndex;
					//	int valIndex = mappingMode == FbxLayerElement::eDirect ? mapIndex : vcHead->GetIndexArray().GetAt(mapIndex);
					//	tri.Colors[idx] = ToVector2(vcHead->GetDirectArray().GetAt(valIndex));
					//}
				}
			}

			if ((ControlPoints.size() != BlendIndices.size() || ControlPoints.size() != BlendWeights.size()))
			{
				LVERR(head, "Size of control points mismatch in mesh[%s]: cp[%d], bi[%d], bw[%d]", Mesh->GetName(),
					ControlPoints.size(), BlendIndices.size(), BlendWeights.size());
			}

			// 顶点坐标转换到本地空间
			ProcessSkeletalVertex();
		}
	}

	void ProcessSkeletalVertex()
	{
		if (ControlPoints.size() == 0)
		{
			return;
		}

		ControlPoints2.resize(ControlPoints.size());
		for (uint32 vertIndex = 0; vertIndex < ControlPoints.size(); ++vertIndex)
		{
			auto& indices = BlendIndices[vertIndex];
			auto& weights = BlendWeights[vertIndex];
			FbxAMatrix mat;
			memset(&mat, 0, sizeof(mat));
			float w = 0.f;
			for (uint32 j = 0; j < MAX_BONES_PER_VERTEX; ++j)
			{
				if (indices[j] == -1)
				{
					break;
				}

				auto index = indices[j];
				auto val = weights[j];

				FbxAMatrix tmp = SkelAMatrix[index];
				AMatrixScale(tmp, val);

				AMatrixAdd(mat, tmp);
				w += val;
			}

			ControlPoints2[vertIndex] = ToMatrix(mat).ApplyPoint(ControlPoints[vertIndex]);
			if (LinkMode == FbxCluster::eNormalize)
			{
				ControlPoints2[vertIndex] *= 1.f/w;
			}
			else if (LinkMode == FbxCluster::eTotalOne)
			{
				ControlPoints2[vertIndex] += ControlPoints[vertIndex] * (1.f - w);
			}
		}
	}

	FMeshDataAlias ToMeshDataGPU(const string& outputDir) const
	{
		FMeshDataAlias output;

		if (Mesh->IsTriangleMesh())
		{
			output.Name = Mesh->GetName();

			output.IndexCount = 0;
			if (output.IndexCount > 0)
			{
				output.VertexCount = ControlPoints2.size();
			}
			else
			{
				output.VertexCount = Triangles.size() * 3;
			}
			
			output.VertexFlags = VertexFlags;

			// Indices & vertices
			output.Indices.clear();
			output.Vertices.clear();
			FBinaryIO stream;
			vector<uint8> padding;
			auto paddingSz = GetPaddingSize(GetVertexDetails(VertexFlags).Stride, 16);
			padding.resize(paddingSz);
			for (uint32 i = 0; i < Triangles.size(); ++i)
			{
				for (uint32 j = 0; j < 3; ++j)
				{
					const auto& vert = Triangles[i].Vertices[j];

					Serialize(stream, (uint8*)&(ControlPoints2[vert.ControlPointIndices]), sizeof(FVec3));

					if ((VertexFlags & EVertexElement::UV) == EVertexElement::UV)
					{
						stream << vert.TexCoords;
					}

					if ((VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
					{
						stream << vert.Normals;
					}

					if ((VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
					{
						stream << vert.Tangents << vert.Binormals;
					}

					if ((VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
					{
						stream << vert.Colors;
					}

					if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
					{
						Serialize(stream, (const uint8*)&BlendWeights[vert.ControlPointIndices][0], sizeof(float)*MAX_BONES_PER_VERTEX);
						Serialize(stream, (const uint8*)&BlendIndices[vert.ControlPointIndices][0], sizeof(float)*MAX_BONES_PER_VERTEX);
					}

					if (padding.size() > 0)
					{
						Serialize(stream, (uint8*)&padding[0], padding.size());
					}
				}
			}

			output.Vertices.resize(stream.RemainingSize());
			Deserialize(stream, (uint8*)&output.Vertices[0], stream.RemainingSize());

			// Skeletal...
			output.Poses = Poses;
			output.Skeleton = Skeleton;
			output.SkeletonIndexMap = SkeletonIndexMap;

			output.VertexMagic = MAGIC_VERTEX;
		}

		if (!outputDir.empty())
		{
			auto outputFile = outputDir + output.Name + "." + K_PRIMITIVE;
			FBinaryIO stream;
			stream << output;
			stream.WriteToFile(outputFile);

			auto sz = stream.Size()/1024.f;

			//FBinaryIO test;
			//FMeshDataAlias mesh;
			//test.ReadFromFile(outputFile);
			//test >> mesh;

			LVMSG("FbxConverter", "mesh[%s, %.3fKB] is saved[%s]", output.Name.c_str(), sz, outputFile.c_str());
		}
		
		return output;
	}
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
	FFbxImporter2()
		: SdkManager(nullptr)
		, SdkScene(nullptr)
	{
		InitializeSdkObjects(SdkManager, SdkScene);
	}

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

	bool DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation)
	{
		//bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());

		return true;
	}

	bool ImportSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation)
	{
		const char* head = "ImportSceneMeshes";
		bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());
		if (!result || SdkManager == nullptr || SdkScene == nullptr)
		{
			LVERR(head, "LoadScene failed");
			return false;
		}

		DestDirectory = convertDst;
		ReplaceChar(DestDirectory, "/", "\\");
		GetDirectory(DestDirectory, DestDirectory);

		ImportNode(SdkScene->GetRootNode());

		for (const auto& mesh : TempMeshArray)
		{
			mesh.ToMeshDataGPU(DestDirectory);
		}

		return true;
	}

	void ImportNode(FbxNode* node)
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

	void ImportMesh(FbxMesh* mesh)
	{
		if (mesh == nullptr)
		{
			return;
		}

		TempMeshArray.push_back(FTempMesh(SdkScene, mesh));
	}

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
	