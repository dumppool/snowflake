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

	vector<FbxNode*>		Links;

	FTempMesh(FbxScene* scene, FbxMesh* mesh) : Scene(scene), Mesh(mesh)
	{}

	bool IsSkeletal()
	{
		return Mesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	}
};

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
		ImportPath = importSrc;

		// format path, find out the correct directory path
		ConvertPath = convertDst;

		bool result = LoadScene(SdkManager, SdkScene, importSrc.c_str());

		return true;
	}

private:
	FbxManager*			SdkManager;
	FbxScene*			SdkScene;

	string				ImportPath;
	string				ConvertPath;

	vector<FTempMesh>	TempMeshArray;
};

bool Importer::DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation)
{
	return FFbxImporter::Get()->DumpSceneMeshes(importSrc, convertDst, outputBinary, exportAnimation);
}

bool Importer::ImportSceneMeshes2(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation)
{
	return FFbxImporter::Get()->DumpSceneMeshes(importSrc, convertDst, outputBinary, exportAnimation);
}
