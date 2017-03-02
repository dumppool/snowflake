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
#include "BasicStaticMesh.h"
#include "BasicCamera.h"
#include "BasicScene.h"
#include "RenderContextInterface.h"
#include "PrimitiveGroupInterface.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

namespace LostCore
{
	class FSimpleCamera : public FBasicCamera
	{
	public:
		FSimpleCamera();
		virtual ~FSimpleCamera();

		virtual bool Init(const char* name, IRenderContext * rc) override;
		virtual void Draw(IRenderContext * rc, float sec) override;
	};

	FSimpleCamera::FSimpleCamera()
	{
	}

	FSimpleCamera::~FSimpleCamera()
	{
	}

	bool FSimpleCamera::Init(const char * name, IRenderContext * rc)
	{
		return true;
	}

	void FSimpleCamera::Draw(IRenderContext * rc, float sec)
	{
		if (rc != nullptr)
		{
			rc->SetViewProjectMatrix(GetViewProjectMatrix());
		}
	}

	class FSimpleScene : public FBasicScene
	{
	public:
		FSimpleScene();
		virtual ~FSimpleScene() override;

		virtual bool Init(const char* name, IRenderContext * rc) override;
		virtual void Fini() override;

	private:
	};

	FSimpleScene::FSimpleScene()
	{
	}

	FSimpleScene::~FSimpleScene()
	{
	}

	bool FSimpleScene::Init(const char * name, IRenderContext * rc)
	{
		FBasicStaticMesh* geo = new FBasicStaticMesh;
		AddStaticMesh(geo);

		IPrimitiveGroup* pg = nullptr;
		if (SSuccess == WrappedCreatePrimitiveGroup(&pg))
		{
			__declspec(align(16)) struct _Vertex { FVec3 XYZ; FVec2 UV; };
			float scaler = 1.f;

			// Mesh vertices
			const _Vertex vertices[] =
			{
				{ FVec3(0, 0, scaler),	FVec2(0.0f, 1.0f) },			// bottom left
				{ FVec3(0, 0, 0),	FVec2(0.0f, 0.0f) },			// top left
				{ FVec3(scaler, 0, 0),	FVec2(1.0f, 0.0f) },			// top right
				{ FVec3(0, scaler, 0),	FVec2(1.0f, 1.0f) },			// bottom right
			};

			// Mesh indices
			const uint16 indices[] = { 0, 2, 1, 1, 3, 2, 3, 1, 0, 3, 0, 2 };

			if (pg->ConstructVB(rc, vertices, sizeof(vertices), sizeof(_Vertex), false) &&
				pg->ConstructIB(rc, indices, sizeof(indices), sizeof(uint16), false))
			{
				geo->AddPrimitiveGroup(pg);
				return true;
			}
		}
		
		return false;
	}

	void FSimpleScene::Fini()
	{
		ClearStaticMesh([](FBasicStaticMesh* p) { delete p; });
	}

	class FSimpleWorld : public FBasicWorld
	{
	public:
		FSimpleWorld();
		virtual ~FSimpleWorld() override;

		virtual bool Init(const char* name, IRenderContext * rc) override;
		virtual void Fini() override;
		virtual void DrawPreScene(float sec) override;
		virtual void DrawPostScene(float sec) override;

		virtual bool InitWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height) override;
		virtual IRenderContext* GetRenderContext() override;
		virtual FBasicCamera* GetCamera() override;

	private:
		IRenderContext*			RC;
		FSimpleCamera*			Camera;
	};

	FSimpleWorld::FSimpleWorld() : RC(nullptr), Camera(nullptr)
	{
	}

	FSimpleWorld::~FSimpleWorld()
	{
		assert(RC == nullptr);
		assert(Camera == nullptr);
	}

	bool FSimpleWorld::Init(const char* name, IRenderContext * rc)
	{
		Camera = new FSimpleCamera;
		if (!Camera->Init(name, rc))
		{
			return false;
		}

		auto scene = new FSimpleScene;
		if (scene->Init(name, rc))
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
		ClearScene([](FBasicScene* p) { delete p; });

		if (RC != nullptr)
		{
			RC->Fini();
			delete RC;
			RC = nullptr;
		}

		if (Camera != nullptr)
		{
			Camera->Fini();
			delete Camera;
			Camera = nullptr;
		}
	}

	void FSimpleWorld::DrawPreScene(float sec)
	{
		if (RC == nullptr)
		{
			return;
		}

		if (Camera == nullptr)
		{
			return;
		}

		RC->BeginFrame(sec);
		Camera->Draw(RC, sec);
	}

	void FSimpleWorld::DrawPostScene(float sec)
	{
		if (RC == nullptr)
		{
			return;
		}

		RC->EndFrame(sec);
	}

	bool FSimpleWorld::InitWindow(const char* name, HWND wnd, bool bWindowed, int32 width, int32 height)
	{
		auto ret = WrappedCreateRenderContext(EContextID::D3D11_DXGI0, &RC);
		assert(SSuccess == ret);
		if (RC != nullptr && RC->Init(wnd, bWindowed, width, height) && Init(name, RC))
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