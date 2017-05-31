/*
* file MeshLoader.h
*
* author luoxw
* date 2017/03/24
*
*
*/

#pragma once

namespace LostCore
{
	class IResourceLoader
	{
	public:
		virtual bool Load(const char* path) = 0;
		virtual void GetPolygons(unsigned char** buf, int* sz) = 0;
		virtual void GetVertices(unsigned char** buf, int* sz) = 0;
		virtual FVertexTypes::Details GetVertexDetails() = 0;
		virtual void GetAnimations(unsigned char** buf, int* sz) = 0;
	};

	IResourceLoader* LoadResource(const char* path);
	LostCore::FVertexTypes::Details GetVertexDetails(int flag);
}