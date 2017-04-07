/*
* file MeshLoader.cpp
*
* author luoxw
* date 2017/03/24
*
*
*/

#include "stdafx.h"
#include "MeshLoader.h"

using namespace LostCore;

class FResourceLoader : public IResourceLoader
{
public:
	bool Load(const char * path) override;
	void GetPolygons(char** buf, int* sz) override;
	void GetVertices(char** buf, int* sz) override;
	void GetAnimations(char** buf, int* sz) override;

private:
	char* PolygonBuf;
	int PolygonSize;

	char* VertexBuf;
	int VertexSize;

	char* AnimationBuf;
	int AnimationSize;
};

bool FResourceLoader::Load(const char * path)
{
	ifstream file;
	file.open(path);

	if (file.fail())
	{
		return false;

	}

	FJson fileJson;
	fileJson << file;

	return true;
}

void FResourceLoader::GetPolygons(char** buf, int* sz)
{

}

void FResourceLoader::GetVertices(char** buf, int* sz)
{

}

void FResourceLoader::GetAnimations(char** buf, int* sz)
{

}

IResourceLoader * LostCore::LoadResource(const char * path)
{
	FResourceLoader* loader = new FResourceLoader;
	loader->Load(path);
	return loader;
}
