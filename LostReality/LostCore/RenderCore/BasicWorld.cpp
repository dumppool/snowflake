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

bool LostCore::FBasicWorld::Config(const FJson & config)
{
	return false;
}

bool FBasicWorld::Load(const char* url)
{
	assert(GUIRoot == nullptr);
	assert(SceneArray.size() == 0);

	GUIRoot = new FBasicGUI;
	return GUIRoot->Load(url);
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

void FBasicWorld::Tick()
{
	auto cam = GetCamera();
	if (cam != nullptr)
	{
		cam->Tick();
	}

	for (auto scene : SceneArray)
	{
		if (scene != nullptr)
		{
			scene->Tick();
		}
	}

	if (GUIRoot != nullptr)
	{
		GUIRoot->Tick();
	}
}

void FBasicWorld::Draw()
{
	if (GetRenderContext() == nullptr)
	{
		return;
	}

	DrawPreScene();

	for (auto scene : SceneArray)
	{
		if (scene != nullptr)
		{
			scene->Draw();
		}
	}

	DrawPostScene();
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

bool LostCore::FBasicWorld::InitializeWindow(HWND wnd, bool bWindowed, int32 width, int32 height)
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

void FBasicWorld::DrawPreScene()
{
	auto rc = GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	if (rc == nullptr)
	{
		return;
	}

	rc->BeginFrame(sec);

	if (GetCamera() != nullptr)
	{
		GetCamera()->Draw();
	}
}

void FBasicWorld::DrawPostScene()
{
	auto rc = GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	if (rc == nullptr)
	{
		return;
	}

	if (GUIRoot != nullptr)
	{
		GUIRoot->Draw();
	}

	rc->EndFrame(sec);
}
