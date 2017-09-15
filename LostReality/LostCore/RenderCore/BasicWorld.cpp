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
}

FBasicWorld::~FBasicWorld()
{
}

bool LostCore::FBasicWorld::Config(const FJson & config)
{
	return false;
}

bool FBasicWorld::Load(const char* url)
{
	return false;
}

void FBasicWorld::Tick()
{
	auto cam = GetCamera();
	if (cam != nullptr)
	{
		cam->Tick();
	}

	if (GetScene() != nullptr)
	{
		GetScene()->Tick();
	}
}

void FBasicWorld::Draw()
{
	if (FGlobalHandler::Get()->GetRenderContext() == nullptr)
	{
		return;
	}

	DrawPreScene();

	if (GetScene() != nullptr)
	{
		GetScene()->Draw();
	}

	DrawPostScene();
}

void FBasicWorld::DrawPreScene()
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	if (rc == nullptr)
	{
		return;
	}

	if (GetCamera() != nullptr)
	{
		GetCamera()->Draw();
	}

	rc->BeginFrame(sec);
}

void FBasicWorld::DrawPostScene()
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	if (rc != nullptr)
	{
		rc->EndFrame(sec);
	}
}
