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

FBasicWorld::FBasicWorld() : GUIRoot(nullptr)
{
	SceneArray.clear();
}

FBasicWorld::~FBasicWorld()
{
	Fini();

	assert(GUIRoot == nullptr);
	assert(SceneArray.size() == 0);
}

bool LostCore::FBasicWorld::Config(IRenderContext * rc, const FJson & config)
{
	return false;
}

bool FBasicWorld::Load(IRenderContext * rc, const char* url)
{
	assert(GUIRoot == nullptr);
	assert(SceneArray.size() == 0);

	GUIRoot = new FBasicGUI;
	return GUIRoot->Load(rc, url);
}

void FBasicWorld::Fini()
{
	SAFE_DELETE(GUIRoot);

	for (auto scene : SceneArray)
	{
		SAFE_DELETE(scene);
	}

	SceneArray.clear();
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

	if (GUIRoot != nullptr)
	{
		GUIRoot->Tick(sec);
	}
}

void FBasicWorld::Draw(IRenderContext * rc, float sec)
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
			scene->Draw(GetRenderContext(), sec);
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

bool LostCore::FBasicWorld::InitializeWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height)
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
	auto rc = GetRenderContext();

	if (rc == nullptr)
	{
		return;
	}

	rc->BeginFrame(sec);

	if (GetCamera() != nullptr)
	{
		GetCamera()->Draw(rc, sec);
	}
}

void FBasicWorld::DrawPostScene(float sec)
{
	auto rc = GetRenderContext();

	if (rc == nullptr)
	{
		return;
	}

	if (GUIRoot != nullptr)
	{
		GUIRoot->Draw(rc, sec);
	}

	rc->EndFrame(sec);
}
