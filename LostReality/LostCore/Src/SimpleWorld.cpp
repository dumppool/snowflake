/*
* file SimpleWorld.cpp
*
* author luoxw
* date 2017/02/05
*
*
*/

#include "stdafx.h"
#include "SimpleWorld.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

namespace LostCore
{
	class FSimpleWorld : public FBasicWorld
	{
	public:
		FSimpleWorld();
		virtual ~FSimpleWorld() override;

		bool Load(IRenderContext * rc, const char* url) override;
		void Fini();

		virtual bool InitializeWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height) override;
		virtual IRenderContext* GetRenderContext() override;
		virtual FBasicCamera* GetCamera() override;

	private:
		FBasicScene*			Scene;
		IRenderContext*			RC;
		FBasicCamera*			Camera;
	};

	FSimpleWorld::FSimpleWorld() : Scene(nullptr), RC(nullptr), Camera(nullptr)
	{
	}

	FSimpleWorld::~FSimpleWorld()
	{
		Fini();
	}

	bool FSimpleWorld::Load(IRenderContext * rc, const char* url)
	{
		FBasicWorld::Load(rc, "");

		Camera = new FBasicCamera;
		if (!Camera->Load(rc, ""))
		{
			return false;
		}

		auto scene = new FBasicScene;
		if (scene->Load(rc, "default_empty.json"))
		{
			AddScene(scene);
			return true;
		}
		else
		{
			return false;
		}
	}
	 
	void FSimpleWorld::Fini()
	{
		RemoveScene(Scene);
		SAFE_DELETE(Scene);
		SAFE_DELETE(Camera);

		//SAFE_DELETE(RC);
		WrappedDestroyRenderContext(forward<IRenderContext*>(RC));
		RC = nullptr;

	}

	bool FSimpleWorld::InitializeWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height)
	{
		auto ret = WrappedCreateRenderContext(EContextID::D3D11_DXGI0, &RC);
		assert(SSuccess == ret);
		if (RC != nullptr && RC->Init(wnd, bWindowed, width, height) && Load(RC, ""))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	IRenderContext * FSimpleWorld::GetRenderContext()
	{
		return RC;
	}

	FBasicCamera * FSimpleWorld::GetCamera()
	{
		return Camera;
	}

	LOSTCORE_API void CreateSimpleWorld(FBasicWorld ** ppObj)
	{
		if (ppObj != nullptr)
		{
			*ppObj = new FSimpleWorld;
		}
	}

	LOSTCORE_API void DestroySimpleWorld(FBasicWorld * obj)
	{
		if (obj != nullptr)
		{
			delete obj;
		}
	}
}