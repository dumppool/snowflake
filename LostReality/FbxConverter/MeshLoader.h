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
		virtual void GetPolygons(char** buf, int* sz) = 0;
		virtual void GetVertices(char** buf, int* sz) = 0;
		virtual void GetAnimations(char** buf, int* sz) = 0;
	};

	IResourceLoader* LoadResource(const char* path);
}