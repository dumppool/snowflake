/*
* file MeshLoader.cpp
*
* author luoxw
* date 2017/03/24
*
*
*/

#include "stdafx.h"
#include "Keywords.h"
#include "MeshLoader.h"

using namespace LostCore;

class FResourceLoader : public IResourceLoader
{
public:
	bool Load(const char * path) override;
	void GetPolygons(unsigned char** buf, int* sz) override;
	void GetVertices(unsigned char** buf, int* sz) override;
	void GetAnimations(unsigned char** buf, int* sz) override;

private:
	unsigned char* PolygonBuf;
	int PolygonSize;

	unsigned char* VertexBuf;
	int VertexSize;

	unsigned char* AnimationBuf;
	int AnimationSize;

	FJson FileJson;
};

bool FResourceLoader::Load(const char * path)
{
	ifstream file;
	file.open(path);

	if (file.fail())
	{
		return false;

	}

	FileJson << file;

	//if (FileJson.find(K_POLYGONS) != FileJson.end())
	//{
	//	auto tb = FJson::to_msgpack(FileJson[K_POLYGONS]);
	//	PolygonSize = tb.size();
	//	PolygonBuf = new unsigned char[tb.size()];
	//	memcpy(PolygonBuf, &tb[0], tb.size());
	//}

	//if (FileJson.find(K_VERTEX) != FileJson.end())
	//{
	//	auto tb = FJson::to_msgpack(FileJson[K_VERTEX]);
	//	VertexSize = tb.size();
	//	VertexBuf = new unsigned char[tb.size()];
	//	memcpy(VertexBuf, &tb[0], tb.size());
	//}

	return true;
}

void FResourceLoader::GetPolygons(unsigned char** buf, int* sz)
{
	if (FileJson.find(K_POLYGONS) == FileJson.end())
		return;

	auto tb = FJson::to_msgpack(FileJson[K_POLYGONS]);
	*sz = tb.size();
	*buf = new unsigned char[tb.size()];
	memcpy(*buf, &tb[0], tb.size());
}

void FResourceLoader::GetVertices(unsigned char** buf, int* sz)
{
	if (FileJson.find(K_VERTEX) == FileJson.end())
		return;

	auto tb = FJson::to_msgpack(FileJson[K_VERTEX]);
	*sz = tb.size();
	*buf = new unsigned char[tb.size()];
	memcpy(*buf, &tb[0], tb.size());
}

void FResourceLoader::GetAnimations(unsigned char** buf, int* sz)
{

}

IResourceLoader * LostCore::LoadResource(const char * path)
{
	FResourceLoader* loader = new FResourceLoader;
	loader->Load(path);
	return loader;
}
