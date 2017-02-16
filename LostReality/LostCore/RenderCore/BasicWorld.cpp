/*
* file BasicWorld.cpp
*
* author luoxw
* date 2017/02/05
*
*
*/

#include "stdafx.h"
#include "BasicWorld.h"
#include "RenderContextInterface.h"
#include "BasicCamera.h"

using namespace LostCore;

FBasicWorld::FBasicWorld()
{
	SceneArray.clear();
}

FBasicWorld::~FBasicWorld()
{
	assert(SceneArray.size() == 0);
}

bool FBasicWorld::Init(const char * name, IRenderContext * rc)
{
	return false;
}

void FBasicWorld::Fini()
{
	assert(0);
}

void FBasicWorld::Tick(float sec)
{
	auto cam = GetCamera();
	if (cam != nullptr)
	{
		cam->Tick(sec);
	}

	for (auto scene : SceneArray)
	{
		if (scene != nullptr)
		{
			scene->Tick(sec);
		}
	}
}

void FBasicWorld::Draw(float sec, IRenderContext * rc)
{
	if (GetRenderContext() == nullptr)
	{
		return;
	}

	DrawPreScene(sec);

	for (auto scene : SceneArray)
	{
		if (scene != nullptr)
		{
			scene->Draw(sec, GetRenderContext());
		}
	}

	DrawPostScene(sec);
}

void FBasicWorld::AddScene(FBasicScene * scene)
{
	if (scene != nullptr && std::find(SceneArray.begin(), SceneArray.end(), scene) == SceneArray.end())
	{
		SceneArray.push_back(scene);
	}
}

void FBasicWorld::RemoveScene(FBasicScene * scene)
{
	if (scene == nullptr)
	{
		return;
	}

	auto result = std::find(SceneArray.begin(), SceneArray.end(), scene);
	if (result != SceneArray.end())
	{
		SceneArray.erase(result);
	}
}

void FBasicWorld::ClearScene(std::function<void(FBasicScene*)> func)
{
	if (func != nullptr)
	{
		for (auto scene : SceneArray)
		{
			if (scene != nullptr)
			{
				func(scene);
			}
		}
	}

	SceneArray.clear();
}
bool LostCore::FBasicWorld::InitWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height)
{
	return false;
}

IRenderContext * FBasicWorld::GetRenderContext()
{
	return nullptr;
}

FBasicCamera * LostCore::FBasicWorld::GetCamera()
{
	return nullptr;
}

void FBasicWorld::DrawPreScene(float sec)
{
}

void FBasicWorld::DrawPostScene(float sec)
{
}
