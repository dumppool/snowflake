// HookCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "HookCorePCH.h"
//#include "mhook-lib/mhook.h"
//#include "libovr-lib/OculusVR.h"
//#include "openvr-lib/OpenVR.h"
//#include "renderers/Direct3D11Renderer.h"

#include "renderers/RouteAPIs.h"

using namespace lostvr;

//static Direct3D11Helper* SD3D11Helper = nullptr;
//
//#define LVSETHOOK(Module, FuncName, FuncType, FuncVarOrig, FuncVarHook, ErrVar, ErrCode)\
//{\
//	FuncVarOrig = (FuncType)::GetProcAddress(::GetModuleHandle(TEXT(Module)), FuncName);\
//	if (Mhook_SetHook((PVOID*)&FuncVarOrig, FuncVarHook))\
//	{\
//		LVMSG("FuncName", "Hook"##" "##FuncName##" successfully!");\
//	}\
//	else\
//	{\
//		LVMSG(FuncName, "Hook"##" "##FuncName##" failed...");\
//		ErrVar = ErrCode;\
//		return;\
//	}\
//}
//
//typedef HRESULT(STDMETHODCALLTYPE *PFN_DXGISWAPCHAIN_PRESENT)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
//static PFN_DXGISWAPCHAIN_PRESENT SwapChainPresent_OrigPtr = nullptr;
//static HRESULT STDMETHODCALLTYPE SwapChainPresent_Hook(IDXGISwapChain* This, UINT Sync, UINT Flags)
//{
//	HRESULT hr = S_FALSE;
//	if (SwapChainPresent_OrigPtr != nullptr)
//	{
//		if (lostvr::OculusVR::Get()->IsDeviceConnected())
//		{
//			lostvr::OculusVR::Get()->OnPresent(This);
//		}
//		else if (lostvr::OpenVRRenderer::Get()->IsDeviceConnected())
//		{
//			lostvr::OpenVRRenderer::Get()->OnPresent(This);
//		}
//
//		hr = SwapChainPresent_OrigPtr(This, Sync, Flags);
//	}
//	else
//	{
//		LVMSG("SwapChainPresent_Hook", "hook get called without original function pointer!");
//	}
//
//	return hr;
//}
//
//static bool HookSwapChainPresent()
//{
//	const CHAR* head = "HookSwapChainPresent";
//	if (SwapChainPresent_OrigPtr != nullptr)
//	{
//		LVMSG(head, "already hooked.");
//		return false;
//	}
//
//	if (SD3D11Helper == nullptr || SD3D11Helper->GetSwapChain() == nullptr)
//	{
//		if (SD3D11Helper != nullptr)
//		{
//			delete SD3D11Helper;
//		}
//
//		SD3D11Helper = new Direct3D11Helper(EDirect3D::DXGI0);
//		if (SD3D11Helper == nullptr)
//		{
//			LVMSG(head, "allocate new Direct3D11Helper failed");
//			return false;
//		}
//	}
//
//	IDXGISwapChain* swapChain = SD3D11Helper->GetSwapChain();
//
//#ifdef _WIN64
//	SwapChainPresent_OrigPtr = (PFN_DXGISWAPCHAIN_PRESENT)*((__int64*)*(__int64*)swapChain + 8);
//#else
//	SwapChainPresent_OrigPtr = (PFN_DXGISWAPCHAIN_PRESENT)*((__int32*)*(__int32*)swapChain + 8);
//#endif
//
//	if (Mhook_SetHook((PVOID*)&SwapChainPresent_OrigPtr, SwapChainPresent_Hook))
//	{
//		LVMSG("HookSwapChainPresent", "set hook succeeded.");
//		return true;
//	}
//	else
//	{
//		LVMSG("HookSwapChainPresent", "set hook failed.");
//		return false;
//	}
//}
//
//static void UnhookSwapChainPresent()
//{
//	if (SwapChainPresent_OrigPtr == nullptr)
//	{
//		LVMSG("UnhookSwapChainPresent", "null original function pointer.");
//	}
//
//	if (Mhook_Unhook((PVOID*)&SwapChainPresent_OrigPtr))
//	{
//		LVMSG("UnhookSwapChainPresent", "unhook succeeded.");
//	}
//	else
//	{
//		LVMSG("UnhookSwapChainPresent", "unhook failed.");
//	}
//}
//
//typedef HRESULT(STDMETHODCALLTYPE *PFN_DEVICE_PRESENT)(IDirect3DDevice9* This, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
//static PFN_DEVICE_PRESENT DevicePresent_OrigPtr = nullptr;
//static HRESULT STDMETHODCALLTYPE Direct3DDevice9Present_Hook(IDirect3DDevice9* This, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
//{
//	HRESULT hr = S_FALSE;
//	if (DevicePresent_OrigPtr != nullptr)
//	{
//		LVMSG("Direct3DDevice9Present_Hook", "hook get called...");
//		hr = DevicePresent_OrigPtr(This, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
//	}
//	else
//	{
//		LVMSG("Direct3DDevice9Present_Hook", "hook get called without original function pointer!");
//	}
//
//	return hr;
//}
//
//static bool HookDirect3DDevice9Present(int* Result)
//{
//	HWND wnd = GetForegroundWindow();
//
//	IDirect3D9* D3D = Direct3DCreate9(D3D_SDK_VERSION);
//	if (D3D == nullptr)
//	{
//		LVMSG("HookDirect3DDevice9Present", "Direct3DCreate9");
//		return false;
//	}
//
//	D3DDISPLAYMODE mode;
//	D3D->GetAdapterDisplayMode(0, &mode);
//
//	D3DPRESENT_PARAMETERS pp;
//	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
//	//pp.BackBufferWidth = WinSizeW;
//	//pp.BackBufferHeight = WinSizeH;
//	pp.BackBufferFormat = mode.Format;
//	pp.BackBufferCount = 1;
//	pp.SwapEffect = D3DSWAPEFFECT_COPY;
//	pp.Windowed = true;
//	pp.hDeviceWindow = wnd;
//
//	IDirect3DDevice9* Device = nullptr;
//	if (FAILED(D3D->CreateDevice(D3DADAPTER_DEFAULT, true ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
//		wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &Device)))
//	{
//		LVMSG("HookDirect3DDevice9Present", "CreateDevice failed");
//		return false;
//	}
//
//#ifdef _WIN64
//	DevicePresent_OrigPtr = (PFN_DEVICE_PRESENT)*((__int64*)*(__int64*)Device + 17);
//#else
//	DevicePresent_OrigPtr = (PFN_DEVICE_PRESENT)*((__int32*)*(__int32*)Device + 17);
//#endif
//	if (Mhook_SetHook((PVOID*)&DevicePresent_OrigPtr, Direct3DDevice9Present_Hook))
//	{
//		LVMSG("HookDirect3DDevice9Present", "set hook succeeded.");
//		return true;
//	}
//	else
//	{
//		LVMSG("HookDirect3DDevice9Present", "set hook faled.");
//		return false;
//	}
//}
//
//static void UnhookDirect3DDevice9Present(int* Result)
//{
//}

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result)
{
	//HookSwapChainPresent();
	//HookDirect3DDevice9Present(Result);
	RouteModule::Get()->InstallRouters();
	if (Result != nullptr)
	{
		*Result = 111;
	}

	return;
}

extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* p)
{
	RouteModule::Get()->UninstallRouters();
	return;
}

#ifdef WITH_VRONLINE

extern "C" _declspec(dllexport) void _cdecl NativeInjectionEntryPointMhook(int* Result)
{
	RouteModule::Get()->InstallRouters();
}

#include "easyhook/Public/easyhook.h"
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	RouteModule::Get()->UninstallRouters();
}

#endif