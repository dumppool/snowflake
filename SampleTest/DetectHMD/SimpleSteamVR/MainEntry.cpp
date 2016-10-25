#include "HookCorePCH.h"

#include "MainEntry.h"
#include "renderer/DirectXUtil.h"

#include "WICTextureLoader.h"
#pragma comment(lib, "DirectXTK.lib")

#include "renderer/IRenderer.h"

using namespace lostvr;

bool MainApp::Init(HINSTANCE hinst, LPWSTR title)
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

	IVRDevice* dev = LostVR::Get()->GetDevice();
	if (dev == nullptr)
	{
		LVMSG("MainApp::Init", "null vr device connected");
		return false;
	}

	uint32 w, h;
	if (!dev->GetDeviceRecommendSize(w, h) || !SDXINST.InitDevice(w, h, 0, true, 1))
	{
		LVMSG("MainApp::Init", "init graphics device failed.");
		return false;
	}
	else 
	{
		LVMSG("MainApp::Init", "init graphics device successfully");
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

	D3D11Projector::Get()->SetRenderTargetSize(OpenVRRenderer::Get()->RecommendWidth, OpenVRRenderer::Get()->RecommendHeight);
	if (!D3D11Projector::Get()->SetSourceRef((ID3D11Texture2D*)Buffer))
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
		OpenVRRenderer::Get()->GetEyeViewProject(i == 0 ? vr::EVREye::Eye_Left : vr::EVREye::Eye_Right, 0.1f, 10000.f, vr::API_DirectX, PoseEye, Proj);
		D3D11Projector::Get()->SetEyePose(i, PoseEye, Proj);
	}

	return true;
}

void MainApp::Run()
{
	bool bRun = true;
	while (bRun && SDXINST.HandleMessages())
	{
		static LVMatrix SWorld;
		static LVMatrix SMultiplierAdd = DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f);
		static LVMatrix SMultiplierSub = DirectX::XMMatrixScaling(0.9f, 0.9f, 0.9f);
		if (SDXINST.Key['W'])
		{
			D3D11Projector::Get()->DebugWorld *= SMultiplierAdd;
		}
		else if (SDXINST.Key['S'])
		{
			D3D11Projector::Get()->DebugWorld *= SMultiplierSub;
		}

		OpenVRRenderer::Get()->UpdatePose();
		D3D11Projector::Get()->OnPresent(nullptr);

		for (int i = 0; i < 2; ++i)
		{
			OpenVRRenderer::Get()->SubmitTexture(i, D3D11Projector::Get()->GetFinalBuffer(i));
		}

		SDXINST.SwapChain->Present(0, 0);
		::Sleep(10);
	}
}

MainApp::MainApp() : bInitialized(false)
{

}
