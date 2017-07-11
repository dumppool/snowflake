/*
* file Importer2.cpp
*
* author luoxw
* date 2017/06/29
*
* 
*/

#include "stdafx.h"
#include "Importer.h"

using namespace Importer;
using namespace LostCore;

struct FTempMesh
{
	FbxScene*				Scene;
	FbxMesh*				Mesh;

	bool					bIsSkeletal;
	vector<FbxNode*>		Links;

	uint32					VertexFlags;

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
		Extract();
	}

	bool IsValid() const
	{
		return Scene != nullptr && Mesh != nullptr;
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
		return (VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent &&
			(VertexFlags & EVertexElement::Binormal) == EVertexElement::Binormal;
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

#ifdef _DEBUG
		Name = Mesh->GetName();

		auto meshNode = Mesh->GetNode();
		auto parentNode = meshNode->GetParent();
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

		if (IsSkeletal())
		{
			bIsSkeletal = true;
			SortSkeletalLink(Scene, Mesh, Links);
		}

		auto layer0 = Mesh->GetLayer(0);
		if (layer0 == nullptr)
		{
			LVERR(head, "mesh[%s] has no layer", Mesh->GetName());
			return;
		}

		// Element heads
		auto vcHead = layer0->GetVertexColors();
		auto normalHead = layer0->GetNormals();
		auto tangentHead = layer0->GetTangents();
		auto binormalHead = layer0->GetBinormals();
		auto uvHead = layer0->GetUVSets();
		auto coordHead = Mesh->GetControlPoints();
		auto coordCount = Mesh->GetControlPointsCount();

		VertexFlags = EVertexElement::Coordinate;
		VertexFlags |= (normalHead != nullptr ? EVertexElement::Normal : 0);
		VertexFlags |= (binormalHead != nullptr ? EVertexElement::Binormal : 0);
		VertexFlags |= (tangentHead != nullptr ? EVertexElement::Tangent : 0);
		VertexFlags |= (uvHead != nullptr ? EVertexElement::UV : 0);
		VertexFlags |= (vcHead != nullptr ? EVertexElement::VertexColor : 0);

		if (coordHead == nullptr)
		{
			LVERR(head, "mesh[%s] has no control points", Mesh->GetName());
			return;
		}

		for (int i = 0; i < coordCount; ++i)
		{

		}
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
	