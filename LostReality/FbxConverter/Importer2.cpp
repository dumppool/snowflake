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
	struct FVertex
	{
		FFloat2				TexCoords;
		FFloat3				Normals;
		FFloat3				Tangents;
		FFloat3				Binormals;
		uint32				ControlPointIndices;
		FFloat3				Colors;
	};

	struct FTriangle
	{
		FVertex				Vertices[3];
	};

	FbxScene*				Scene;
	FbxMesh*				Mesh;

	bool					bIsSkeletal;

	// 骨骼的FbxAMatrix
	map<string, FbxAMatrix> SkelAMatrix;

	// link mode
	FbxCluster::ELinkMode	LinkMode;

	// 骨骼
	FSkeletonTreeAlias		Skeleton;

	// poses
	map<string, FPoseMapAlias>	Poses;
	FPoseMapAlias			DefaultPose;
	FPoseMapAlias			GlobalPose;

	// 骨骼索引表
	map<string, int32>		SkeletonIndexMap;

	// 顶点结构
	uint32					VertexFlags;

	// 矩阵[node -> scene root]
	FFloat4x4					MeshToSceneRoot;

	// MeshToSceneRoot的旋转部分
	FFloat4x4					MeshToSceneRootDir;

	// geometry space
	vector<FFloat3>			ControlPoints;

	// bone space
	vector<FFloat3>			ControlPoints2;

#if TEST_SOFT_SKINNED
	// skinned points
	vector<FFloat3>			SkinnedPts;
#endif

	// 三角面
	vector<FTriangle>		Triangles;

	// 骨骼模型才有的骨骼索引&骨骼混合权重
	vector<FFloat4>	BlendWeights;
	vector<FSInt4>	BlendIndices;

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
			for_each(BlendIndices.begin(), BlendIndices.end(), [](FSInt4& arr) {arr = FSInt4(-1,-1,-1,-1); });
			
			BlendWeights.resize(cpCount);
			for_each(BlendWeights.begin(), BlendWeights.end(), [](FFloat4& arr) {arr = FFloat4(INFINITY, INFINITY, INFINITY, INFINITY); });

			Poses[K_INITIAL_POSE] = FPoseMap();

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

				DefaultPose[link->GetName()] = ToMatrix(GetNodeLocalMatrix(link));
				//SkelAMatrix[link->GetName()] = GetInitialClusterMatrix(Mesh, cluster, Poses[K_INITIAL_POSE]);

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
			BuildSkeletonTree(link0, Skeleton);

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
					GetPoseMapFromTree(poseTree2, GlobalPose);
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
						int mapIndex = referenceMode == FbxLayerElement::eDirect ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eByControlPoint ? mapIndex : normalHead->GetIndexArray().GetAt(mapIndex);
						vert.Normals = ToVector3(normalHead->GetDirectArray().GetAt(valIndex)).GetNormal();
						vert.Normals.Normalize();
					}

					if (tangentHead != nullptr)
					{
						referenceMode = tangentHead->GetReferenceMode();
						mappingMode = tangentHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eDirect ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eByControlPoint ? mapIndex : tangentHead->GetIndexArray().GetAt(mapIndex);
						vert.Tangents = ToVector3(tangentHead->GetDirectArray().GetAt(valIndex)).GetNormal();
						vert.Tangents.Normalize();
					}

					if (binormalHead != nullptr)
					{
						referenceMode = binormalHead->GetReferenceMode();
						mappingMode = binormalHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eDirect ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eByControlPoint ? mapIndex : binormalHead->GetIndexArray().GetAt(mapIndex);
						vert.Binormals = ToVector3(binormalHead->GetDirectArray().GetAt(valIndex)).GetNormal();
						vert.Binormals.Normalize();
					}

					// TODO: 考虑多重uv
					if (uvHead != nullptr)
					{
						referenceMode = uvHead->GetReferenceMode();
						mappingMode = uvHead->GetMappingMode();
						int mapIndex = referenceMode == FbxLayerElement::eDirect ? cpIndex : tmpIndex;
						int valIndex = mappingMode == FbxLayerElement::eByControlPoint ? mapIndex : uvHead->GetIndexArray().GetAt(mapIndex);
						vert.TexCoords = ToVector2(uvHead->GetDirectArray().GetAt(valIndex));
					}

					// TODO: 没有Color类，暂不导入
					//if (vcHead != nullptr)
					//{
					//	referenceMode = vcHead->GetReferenceMode();
					//	mappingMode = vcHead->GetMappingMode();
					//	int mapIndex = referenceMode == FbxLayerElement::eByControlPoint ? cpIndex : tmpIndex;
					//	int valIndex = mappingMode == FbxLayerElement::eByControlPoint ? mapIndex : vcHead->GetIndexArray().GetAt(mapIndex);
					//	tri.Colors[idx] = ToVector2(vcHead->GetDirectArray().GetAt(valIndex));
					//}
				}
			}

			if ((ControlPoints.size() != BlendIndices.size() || ControlPoints.size() != BlendWeights.size()))
			{
				LVERR(head, "Size of control points mismatch in mesh[%s]: cp[%d], bi[%d], bw[%d]", Mesh->GetName(),
					ControlPoints.size(), BlendIndices.size(), BlendWeights.size());
			}

			// 标准化蒙皮权重
			ProcessBlendWeight();

			// 顶点坐标转换到本地空间
			ProcessSkeletalVertex();
		}
	}

	void ProcessBlendWeight()
	{
		for (uint32 i = 0; i < BlendWeights.size(); ++i)
		{
			auto& indices = BlendIndices[i];
			auto& weights = BlendWeights[i];
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

	void ProcessSkeletalVertex()
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
		Poses[K_INITIAL_POSE] = DefaultPose;
		auto& initialPose = Poses[K_INITIAL_POSE];

		LostCore::FFloat4x4 globalMat;
		globalMat.SetIdentity();
		GetGlobalPoseMap(globalMat, Skeleton, initialPose, globalPose);

		map<int32, string> inverseMap;
		for_each(SkeletonIndexMap.begin(), SkeletonIndexMap.end(),
			[&](const pair<string, int32>& elem) {inverseMap[elem.second] = elem.first; });

		ControlPoints2.resize(ControlPoints.size());

		for (uint32 vertIndex = 0; vertIndex < ControlPoints.size(); ++vertIndex)
		{
			auto& indices = BlendIndices[vertIndex];
			auto& weights = BlendWeights[vertIndex];
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

				/*******************************/
				//auto it = GlobalPose.begin();
				//auto tmpIndex = index;
				//while ((tmpIndex--) > 0)
				//	++it;

				//auto tmp2 = it->second;
				//tmp2 = tmp2 * val;
				//mat2 = mat2 + tmp2;

				const auto& n = inverseMap[index];
				auto tmp2 = globalPose[n];
				mat2 = mat2 + globalPose[n] * val;

				/*******************************/

				FbxAMatrix tmp = SkelAMatrix[n];
				AMatrixScale(tmp, val);

				AMatrixAdd(mat, tmp);
				w += val;
			}

			mat2 = mat2 * (1.f / w);
			auto invMat2 = mat2;
			invMat2.Invert();
			ControlPoints2[vertIndex] = invMat2.ApplyPoint(ControlPoints[vertIndex]);
#if TEST_SOFT_SKINNED
			skinned0[vertIndex] = mat2.ApplyPoint(ControlPoints2[vertIndex]);
#endif

#ifdef _DEBUG
			if (vertIndex == 817)
			{
				printf("");
			}
#endif

			if (LinkMode == FbxCluster::eNormalize)
			{
				ControlPoints2[vertIndex] *= 1.f/w;
			}
			else if (LinkMode == FbxCluster::eTotalOne)
			{
				ControlPoints2[vertIndex] += ControlPoints[vertIndex] * (1.f - w);
			}
		}

#if TEST_SOFT_SKINNED
		SkinnedPts = skinned0;
		//for (uint32 i = 0; i < ControlPoints.size(); ++i)
		//{
		//	auto& indices = BlendIndices[i];
		//	auto& weights = BlendWeights[i];
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

		//		skinned1[i] += globalPose[inverseMap[index]].ApplyPoint(ControlPoints2[i]) * weights[j];
		//	}

		//	//totalMat = totalMat * (1.f / totalWeight);
		//	skinned0[i] = totalMat.ApplyPoint(ControlPoints2[i]);
		//}
		//if (skinned0[0] != skinned1[0])
		//	printf("");
#endif
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

#if EXPORT_ORIGINAL_GEOMETRY
					stream << ControlPoints[vert.ControlPointIndices];
#elif TEST_SOFT_SKINNED
					stream << SkinnedPts[vert.ControlPointIndices];
#else
					stream << ControlPoints2[vert.ControlPointIndices];
					//Serialize(stream, (uint8*)&(ControlPoints2[vert.ControlPointIndices]), sizeof(FFloat3));
#endif

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
						stream << BlendWeights[vert.ControlPointIndices] << BlendIndices[vert.ControlPointIndices];
					}

					if (padding.size() > 0)
					{
						Serialize(stream, (uint8*)&padding[0], padding.size());
					}
				}
			}

			assert(stream.RemainingSize() == output.VertexCount * GetAlignedSize(GetVertexDetails(VertexFlags).Stride, 16));

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
	