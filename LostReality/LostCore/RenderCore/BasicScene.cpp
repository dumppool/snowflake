/*
* file BasicScene.cpp
*
* author luoxw
* date 2017/02/06
*
*
*/

#include "stdafx.h"
#include "BasicScene.h"
#include "RenderContextInterface.h"

using namespace LostCore;

FBasicScene::FBasicScene()
{
	StaticMeshArray.clear();
}

FBasicScene::~FBasicScene()
{
	assert(StaticMeshArray.size() == 0);
}

void FBasicScene::Tick(float sec)
{
	for (auto sm : StaticMeshArray)
	{
		if (sm != nullptr)
		{
			sm->Tick(sec);
		}
	}
}

void FBasicScene::Draw(IRenderContext * rc, float sec)
{
	for (auto sm : StaticMeshArray)
	{
		if (sm != nullptr)
		{
			sm->Draw(rc, sec);
		}
	}
}

bool FBasicScene::Init(const char * name, IRenderContext * rc)
{
	return false;
}

void FBasicScene::Fini()
{
	assert(0);
}

void FBasicScene::AddStaticMesh(FBasicStaticMesh * sm)
{
	if (sm != nullptr && std::find(StaticMeshArray.begin(), StaticMeshArray.end(), sm) == StaticMeshArray.end())
	{
		StaticMeshArray.push_back(sm);
	}
}

void FBasicScene::RemoveStaticMesh(FBasicStaticMesh * sm)
{
	if (sm == nullptr)
	{
		return;
	}

	auto result = std::find(StaticMeshArray.begin(), StaticMeshArray.end(), sm);
	if (result != StaticMeshArray.end())
	{
		StaticMeshArray.erase(result);
	}
}

void FBasicScene::ClearStaticMesh(std::function<void(FBasicStaticMesh*)> func)
{
	auto clear = (func != nullptr ? func : [](FBasicStaticMesh* p) { delete p; });
	for (auto sm : StaticMeshArray)
	{
		if (sm != nullptr)
		{
			clear(sm);
		}
	}

	StaticMeshArray.clear();
}