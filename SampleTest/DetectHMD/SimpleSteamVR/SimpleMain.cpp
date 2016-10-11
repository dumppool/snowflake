#include "stdafx.h"
#include "Win32_DirectXAppUtil.h"

#include "openvr-lib/OpenVR.h"
#include "projector/Projector.h"

#include "WICTextureLoader.h"
#pragma comment(lib, "DirectXTK.lib")

using namespace LostVR;

class MainApp
{
public:
	bool Init(HINSTANCE hinst, LPWSTR title)
	{
		if (!SDXINST.InitWindow(hinst, title))
		{
			LVMSG("MainApp::Init", "init window(%x) failed", hinst);
			return false;
		}
		else
		{
			LVMSG("MainApp::Init", "init window(%x) successfully", hinst);
		}

		if (!OpenVRRenderer::Get()->Init()) 
		{
			return false;
		}
		else
		{
		}

		if (!SDXINST.InitDevice(OpenVRRenderer::Get()->RecommendWidth, OpenVRRenderer::Get()->RecommendHeight, 0, true, 1))
		{
			LVMSG("MainApp::Init", "init device failed.");
			return false;
		}
		else
		{
			LVMSG("MainApp::Init", "init device successfully");
		}

		ID3D11Resource* Buffer = nullptr;
		const TCHAR* ImageFile = TEXT("C:\\Users\\Administrator\\Documents\\GitHub\\snowflake\\SampleTest\\DetectHMD\\SimpleSteamVR\\cube_texture.png");
		if (FAILED(DirectX::CreateWICTextureFromFile(SDXINST.Device, ImageFile, &Buffer, nullptr)))
		{
			LVMSG("MainApp::Init", "load texture(%ls) failed", ImageFile);
			return false;
		}
		else
		{
			LVMSG("MainApp::Init", "load texture(%ls) successfully", ImageFile);
		}

		TextureProjector::Get()->SetRenderTargetSize(OpenVRRenderer::Get()->RecommendWidth, OpenVRRenderer::Get()->RecommendHeight);
		if (!TextureProjector::Get()->SetSourceRef((ID3D11Texture2D*)Buffer))
		{
			ReleaseComObjectRef(Buffer);
			LVMSG("MainApp::Init", "init projector failed");
			return false;
		}
		else
		{
			ReleaseComObjectRef(Buffer);
			LVMSG("MainApp::Init", "init projector successfully");
		}

		for (int i = 0; i < 2; ++i)
		{
			LVMatrix PoseEye, Proj;
			OpenVRRenderer::Get()->GetEyeViewProject(i == 0?vr::EVREye::Eye_Left:vr::EVREye::Eye_Right, 0.1f, 10000.f, vr::API_DirectX, PoseEye, Proj);
			TextureProjector::Get()->SetEyePose(i, PoseEye, Proj);
		}

		return true;
	}

	void Run()
	{
		bool bRun = true;
		while (bRun && SDXINST.HandleMessages())
		{
			static LVMatrix SWorld;
			static LVMatrix SMultiplierAdd = DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f);
			static LVMatrix SMultiplierSub = DirectX::XMMatrixScaling(0.9f, 0.9f, 0.9f);
			if (SDXINST.Key['W'])
			{
				TextureProjector::Get()->DebugWorld *= SMultiplierAdd;
			}
			else if (SDXINST.Key['S'])
			{
				TextureProjector::Get()->DebugWorld *= SMultiplierSub;
			}

			OpenVRRenderer::Get()->UpdatePose();
			TextureProjector::Get()->OnPresent(nullptr);

			for (int i = 0; i < 2; ++i)
			{
				OpenVRRenderer::Get()->SubmitTexture(i, TextureProjector::Get()->GetFinalBuffer(i));
			}

			SDXINST.SwapChain->Present(0, 0);
			::Sleep(10);
		}
	}
};
//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	MainApp App;
	if (App.Init(hinst, TEXT("Simple SteamVR")))
		App.Run();
	
	return 0;
}