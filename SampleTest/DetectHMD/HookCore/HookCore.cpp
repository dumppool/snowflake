// HookCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "HookCorePCH.h"
#include "mhook-lib/mhook.h"
#include "libovr-lib/OculusVR.h"
#include "openvr-lib/OpenVR.h"

#define LVSETHOOK(Module, FuncName, FuncType, FuncVarOrig, FuncVarHook, ErrVar, ErrCode)\
{\
	FuncVarOrig = (FuncType)::GetProcAddress(::GetModuleHandle(TEXT(Module)), FuncName);\
	if (Mhook_SetHook((PVOID*)&FuncVarOrig, FuncVarHook))\
	{\
		LVMSG("FuncName", "Hook"##" "##FuncName##" successfully!");\
	}\
	else\
	{\
		LVMSG(FuncName, "Hook"##" "##FuncName##" failed...");\
		ErrVar = ErrCode;\
		return;\
	}\
}

typedef HRESULT(STDMETHODCALLTYPE *PFN_DXGISWAPCHAIN_PRESENT)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
static PFN_DXGISWAPCHAIN_PRESENT SwapChainPresent_OrigPtr = nullptr;
static HRESULT STDMETHODCALLTYPE SwapChainPresent_Hook(IDXGISwapChain* This, UINT Sync, UINT Flags)
{
	HRESULT hr = S_FALSE;
	if (SwapChainPresent_OrigPtr != nullptr)
	{
		if (lostvr::OculusVR::Get()->IsDeviceConnected())
		{
			lostvr::OculusVR::Get()->OnPresent(This);
		}
		else if (lostvr::OpenVRRenderer::Get()->IsDeviceConnected())
		{
			lostvr::OpenVRRenderer::Get()->OnPresent(This);
		}

		hr = SwapChainPresent_OrigPtr(This, Sync, Flags);
	}
	else
	{
		LVMSG("SwapChainPresent_Hook", "hook get called without original function pointer!");
	}

	return hr;
}

static bool HookSwapChainPresent()
{
	if (SwapChainPresent_OrigPtr != nullptr)
	{
		LVMSG("HookSwapChainPresent", "already hooked.");
		//return false;
	}

	IDXGISwapChain* SwapChain = nullptr;
	ID3D11DeviceContext* Context = nullptr; 
	ID3D11Device* Device = nullptr;

	HWND hwnd = ::GetForegroundWindow();
	if (hwnd == NULL)
	{
		LVMSG("HookSwapChainPresent", "failed to get foreground window.");
		return false;
	}
	else
	{
		LVMSG("HookCore", "");
		LVMSG("HookCore", "*******************************************************");
		LVMSG("HookSwapChainPresent", "got foreground window(%x).", hwnd);
	}

	D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC Desc{ 0 };
	Desc.BufferCount = 1;
	Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	Desc.BufferDesc.RefreshRate.Numerator = 0;
	Desc.BufferDesc.RefreshRate.Denominator = 1;
	Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Desc.OutputWindow = hwnd;
	Desc.Windowed = (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP) != 0 ? FALSE : TRUE;;
	Desc.SampleDesc.Count = 1;
	Desc.SampleDesc.Quality = 0;
	Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	Desc.Flags = 0;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &FeatureLevels, 1,
		D3D11_SDK_VERSION, &Desc, &SwapChain, &Device, NULL, &Context);

	if (FAILED(hr))
	{
		LVMSG("HookSwapChainPresent", "D3D11CreateDeviceAndSwapChain failed");
		return false;
	}

	SwapChainPresent_OrigPtr = (PFN_DXGISWAPCHAIN_PRESENT)*((__int64*)*(__int64*)SwapChain + 8);
	if (Mhook_SetHook((PVOID*)&SwapChainPresent_OrigPtr, SwapChainPresent_Hook))
	{
		LVMSG("HookSwapChainPresent", "set hook succeeded.");
		return true;
	}
	else
	{
		LVMSG("HookSwapChainPresent", "set hook failed.");
		return false;
	}
}

static void UnhookSwapChainPresent()
{
	if (SwapChainPresent_OrigPtr == nullptr)
	{
		LVMSG("UnhookSwapChainPresent", "null original function pointer.");
	}

	if (Mhook_Unhook((PVOID*)&SwapChainPresent_OrigPtr))
	{
		LVMSG("UnhookSwapChainPresent", "unhook succeeded.");
	}
	else
	{
		LVMSG("UnhookSwapChainPresent", "unhook failed.");
	}
}

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result)
{
	HookSwapChainPresent();
	if (Result != nullptr)
	{
		*Result = -1;
	}

	return;
}

extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* p)
{
	UnhookSwapChainPresent();
	return;
}

#ifdef WITH_VRONLINE

extern "C" _declspec(dllexport) void _cdecl NativeInjectionEntryPointMhook(int* Result)
{
	InstallHook(Result);
}

#include "easyhook/Public/easyhook.h"
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	InstallHook(nullptr);
}

#endif