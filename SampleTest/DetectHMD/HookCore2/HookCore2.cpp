// HookCore2.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"

#include <Windows.h>
#include <stdio.h>
#include <d3d11.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "mhook-lib/mhook.h"
#include "wrapped-dxgi/dxgi_wrapped.h"

//#include "wrapped-d3d11\d3d11_context.h"
//#include "wrapped-d3d11\d3d11_device.h"

PFN_D3D11_CREATE_DEVICE D3D11CreateDevice_OriginalPtr = nullptr;
static HRESULT WINAPI D3D11CreateDevice_Hook(
	_In_opt_ IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
	UINT SDKVersion, _COM_Outptr_opt_ ID3D11Device** ppDevice,
	_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
	_COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext)
{
	//printf("D3D11CreateDevice_hook get called.\n");
	HRESULT ret = S_FALSE;


	if (nullptr != D3D11CreateDevice_OriginalPtr)
	{
		ret = D3D11CreateDevice_OriginalPtr(pAdapter, DriverType, Software, Flags, pFeatureLevels,
			FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
		LVMSG("D3D11CreateDevice", "Hooked, device(%16x)", *ppDevice);
	}
	else
	{
		LVMSG("D3D11CreateDevice", "Original");
		ret = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels,
			FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
	}

	return ret;
}

static PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN D3D11CreateDeviceAndSwapChain_OriginalPtr = nullptr;
static HRESULT WINAPI D3D11CreateDeviceAndSwapChain_Hook(__in_opt IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType,
	HMODULE Software, UINT Flags,
	__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL *pFeatureLevels,
	UINT FeatureLevels, UINT SDKVersion,
	__in_opt CONST DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	__out_opt IDXGISwapChain **ppSwapChain, __out_opt ID3D11Device **ppDevice,
	__out_opt D3D_FEATURE_LEVEL *pFeatureLevel,
	__out_opt ID3D11DeviceContext **ppImmediateContext)
{
	HRESULT ret = S_FALSE;
	if (D3D11CreateDeviceAndSwapChain_OriginalPtr)
	{
		ret = D3D11CreateDeviceAndSwapChain_OriginalPtr(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
			SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
		LVMSG("D3D11CreateDeviceAndSwapChain", "Hooked, device(%16x)", *ppDevice);
	}
	else
	{
		LVMSG("D3D11CreateDeviceAndSwapChain", "Original");
		ret = D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
			SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	}

	return ret;
}

typedef HRESULT(WINAPI *PFN_CREATE_DXGI_FACTORY)(REFIID, void **);

static PFN_CREATE_DXGI_FACTORY CreateDXGIFactory_OriginalPtr = nullptr;
static HRESULT WINAPI CreateDXGIFactory_Hook(__in REFIID riid, __out void **ppFactory)
{
	if (nullptr != CreateDXGIFactory_OriginalPtr)
	{
		HRESULT hr = CreateDXGIFactory_OriginalPtr(riid, ppFactory);
		LVMSG("CreateDXGIFactory", "Hooked, factory(%16x)", *ppFactory);
		if (SUCCEEDED(hr))
		{
			RefCountDXGIObject::HandleWrap(riid, ppFactory);
		}

		return hr;
	}
	else
	{
		LVMSG("CreateDXGIFactory", "Original");
		return CreateDXGIFactory(riid, ppFactory);
	}
}

static PFN_CREATE_DXGI_FACTORY CreateDXGIFactory1_OriginalPtr = nullptr;
static HRESULT WINAPI CreateDXGIFactory1_Hook(__in REFIID riid, __out void **ppFactory)
{
	if (nullptr != CreateDXGIFactory1_OriginalPtr)
	{
		HRESULT hr = CreateDXGIFactory1_OriginalPtr(riid, ppFactory);
		LVMSG("CreateDXGIFactory1", "Hooked, factory(%x)", *ppFactory);
		if (SUCCEEDED(hr))
		{
			RefCountDXGIObject::HandleWrap(riid, ppFactory);
		}

		return hr;
	}
	else
	{
		LVMSG("CreateDXGIFactory1", "Original");
		return CreateDXGIFactory1(riid, ppFactory);
	}
}

typedef HRESULT(WINAPI *PFN_CREATE_DXGI_FACTORY2)(UINT, REFIID, void **);
static PFN_CREATE_DXGI_FACTORY2 CreateDXGIFactory2_OriginalPtr = nullptr;
static HRESULT WINAPI CreateDXGIFactory2_Hook(UINT Flags, __in REFIID riid, __out void **ppFactory)
{
	return S_FALSE;
}


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

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result)
{
	//int Result[1] = { 0 };
	LVMSG("InstallHook", "SetHook_D3D11...\n");

	LVSETHOOK("d3d11.dll", "D3D11CreateDevice", PFN_D3D11_CREATE_DEVICE, D3D11CreateDevice_OriginalPtr, D3D11CreateDevice_Hook, *Result, 1);
	LVSETHOOK("d3d11.dll", "D3D11CreateDeviceAndSwapChain", PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN, D3D11CreateDeviceAndSwapChain_OriginalPtr, D3D11CreateDeviceAndSwapChain_Hook, *Result, 1);
	LVSETHOOK("dxgi.dll", "CreateDXGIFactory", PFN_CREATE_DXGI_FACTORY, CreateDXGIFactory_OriginalPtr, CreateDXGIFactory_Hook, *Result, 2);
	LVSETHOOK("dxgi.dll", "CreateDXGIFactory1", PFN_CREATE_DXGI_FACTORY, CreateDXGIFactory1_OriginalPtr, CreateDXGIFactory1_Hook, *Result, 4);
	//D3D11CreateDevice_OriginalPtr = (PFN_D3D11_CREATE_DEVICE)::GetProcAddress(::GetModuleHandle(TEXT("d3d11.dll")), "D3D11CreateDevice");
	//if (Mhook_SetHook((PVOID*)&D3D11CreateDevice_OriginalPtr, D3D11CreateDevice_Hook))
	//{
	//	::MessageBoxA(NULL, "Hook D3D11CreateDevice successfully", "D3D11CreateDevice", 0);
	//	printf("Hook D3D11CreateDevice successfully.\n");
	//}
	//else
	//{
	//	::MessageBoxA(NULL, "Hook D3D11CreateDevice failed", "D3D11CreateDevice", 0);
	//	printf("Hook D3D11CreateDevice failed.\n");
	//	*Result = 1;
	//	return;
	//}

	//CreateDXGIFactory_OriginalPtr = (PFN_CREATE_DXGI_FACTORY)::GetProcAddress(::GetModuleHandle(TEXT("dxgi.dll")), "CreateDXGIFactory");
	//PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN D3D11CreateDeviceAndSwapChain_OriginalPtr = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)::GetProcAddress(::GetModuleHandle(TEXT("d3d11.dll")), "D3D11CreateDeviceAndSwapChain");
	//if (Mhook_SetHook((PVOID*)&D3D11CreateDeviceAndSwapChain_OriginalPtr, D3D11CreateDeviceAndSwapChain_Hook))
	//{
	//	::MessageBoxA(NULL, "Hook D3D11CreateDeviceAndSwapChain successfully", "D3D11CreateDeviceAndSwapChain", 0);
	//	printf("Hook D3D11CreateDeviceAndSwapChain successfully.\n");
	//}
	//else
	//{
	//	::MessageBoxA(NULL, "Hook D3D11CreateDeviceAndSwapChain failed", "D3D11CreateDeviceAndSwapChain", 0);
	//	printf("Hook D3D11CreateDeviceAndSwapChain failed.\n");
	//	*Result = 2;
	//	return;
	//}

	*Result = -1;
	return;
}

extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* p)
{

}