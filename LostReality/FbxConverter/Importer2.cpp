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

static void ParseAnimation(std::function<FJson&()> outputGetter, FbxScene* scene)
{

}

static void ParsePose(std::function<FJson&()> outputGetter, FbxScene* scene)
{

}

static void ParseSkeleton(std::function<FJson&()> outputGetter, FbxNode* node)
{

}

static void ParseMesh(std::function<FJson&()> outputGetter, FbxNode* node)
{

}

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
			case FbxNodeAttribute::eSkeleton:
				ParseSkeleton([&]()->FJson& {(*it)[K_TYPE] = K_SKELETON; return *it; }, node);
				break;
			case FbxNodeAttribute::eMesh:
				ParseMesh([&]()->FJson& {(*it)[K_TYPE] = K_MESH; return *it; }, node);
				break;
			//case FbxNodeAttribute::eMarker:
			//	(*it)[K_TYPE] = K_MARKER;
			//	ParseMarker(*it, node);
			//	break;
			//case FbxNodeAttribute::eNurbs:
			//	(*it)[K_TYPE] = K_NURBS;
			//	ParseNurbs(*it, node);
			//	break;
			//case FbxNodeAttribute::ePatch:
			//	(*it)[K_TYPE] = K_PATCH;
			//	ParsePatch(*it, node);
			//	break;
			//case FbxNodeAttribute::eCamera:
			//	(*it)[K_TYPE] = K_CAMERA;
			//	ParseCamera(*it, node);
			//	break;
			//case FbxNodeAttribute::eLight:
			//	(*it)[K_TYPE] = K_LIGHT;
			//	ParseLight(*it, node);
			//	break;
			//case FbxNodeAttribute::eLODGroup:
			//	(*it)[K_TYPE] = K_LODGROUP;
			//	ParseLODGroup(*it, node);
			//	break;
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
		ParsePose([&]()->FJson& {return PoseData; }, SdkScene);
	}

	void ImportAnimation()
	{
		ParseAnimation([&]()->FJson& {return AnimData; }, SdkScene);
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

bool Importer::ImportScene2(const string& importSrc, const string& convertDst)
{
	return FFbxImporter::Get()->ImportScene(importSrc, convertDst);
}