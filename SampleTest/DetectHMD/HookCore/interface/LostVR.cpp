#include "HookCorePCH.h"
#include "LostVR.h"
#include "IVRDevice.h"
#include "IRenderer.h"

using namespace lostvr;

void LostVR::OnPresent(IDXGISwapChain* InSwapChain)
{
	bool bVRDeviceReady = VRDeviceModule::Get()->IsConnected();
	if (!bVRDeviceReady)
	{
		bVRDeviceReady = VRDeviceModule::Get()->Startup();
	}

	if (!bVRDeviceReady)
	{
		LVMSG("LostVR::OnPresent", "VRDeviceModule startup failed.");
		VRDeviceModule::Get()->Shutdown();
		return;
	}

	//bool bRendererReady = RendererModule::Get()->IsConnected();
	//if (!bRendererReady)
	//{
	//	bRendererReady = RendererModule::Get()->Startup();
	//}

	//if (!bRendererReady)
	//{
	//	LVMSG("LostVR::OnPresent", "RendererModule startup failed.");
	//	RendererModule::Get()->Shutdown();
	//	return;
	//}

	//VRDeviceModule::Get()->GetVRDevice()->Submit(RendererModule::Get()->GetRenderer(), InSwapChain);
}
