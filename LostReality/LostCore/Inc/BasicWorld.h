/*
* file BasicWorld.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "BasicInterface.h"
#include "BasicGUI.h"
#include "BasicScene.h"

namespace LostCore
{
	class FBasicCamera;

	class FBasicWorld : public IBasicInterface
	{
	public:
		FBasicWorld();
		virtual ~FBasicWorld() override;

		virtual bool Config(const FJson& config) override;
		virtual bool Load(const char* url) override;
		virtual void Tick() override;
		virtual void Draw() override;

		virtual void DrawPreScene();
		virtual void DrawPostScene();

		virtual bool InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height) = 0;
		virtual FBasicCamera* GetCamera() = 0;
		virtual FBasicScene* GetScene() = 0;
	};
}