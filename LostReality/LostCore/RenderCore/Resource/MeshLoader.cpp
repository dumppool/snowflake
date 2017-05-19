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
	void GetVertices(unsigned char** buf, int* sz, int* stride) override;
	void GetAnimations(unsigned char** buf, int* sz) override;

	unsigned char* PolygonBuf;
	int PolygonSize;

	unsigned char* VertexBuf;
	int VertexSize;
	int VertexStride;

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

	if (FileJson.find(K_POLYGONS) != FileJson.end())
	{
		auto tb = FJson::to_msgpack(FileJson[K_POLYGONS]);
		PolygonSize = tb.size();
		PolygonBuf = new unsigned char[tb.size()];
		memcpy(PolygonBuf, &tb[0], tb.size());
	}

	if (FileJson.find(K_VERTEX) != FileJson.end() && FileJson.find(K_VERTEX_ELEMENT) != FileJson.end())
	{
		VertexStride = GetVertexStride(FileJson[K_VERTEX_ELEMENT]);
		auto tb = FJson::to_msgpack(FileJson[K_VERTEX]);
		VertexSize = tb.size();
		VertexBuf = new unsigned char[tb.size()];
		memcpy(VertexBuf, &tb[0], tb.size());
	}

	return true;
}

void FResourceLoader::GetPolygons(unsigned char** buf, int* sz)
{
	*buf = PolygonBuf;
	*sz = PolygonSize;
}

void FResourceLoader::GetVertices(unsigned char** buf, int* sz, int* stride)
{
	*buf = VertexBuf;
	*sz = VertexSize;
	*stride = VertexStride;
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

int LostCore::GetVertexStride(int flag)
{
	int stride = 0;
	stride += ((flag & EVertexElement::Coordinate) == EVertexElement::Coordinate ? 3 * sizeof(float) : 0);
	stride += ((flag & EVertexElement::Normal) == EVertexElement::Normal ? 3 * sizeof(float) : 0);
	stride += ((flag & EVertexElement::Binormal) == EVertexElement::Binormal ? 3 * sizeof(float) : 0);
	stride += ((flag & EVertexElement::Tangent) == EVertexElement::Tangent ? 3 * sizeof(float) : 0);
	stride += ((flag & EVertexElement::UV) == EVertexElement::UV ? 2 * sizeof(float) : 0);
	stride += ((flag & EVertexElement::VertexColor) == EVertexElement::VertexColor ? 3 * sizeof(float) : 0);
	return stride;
}
