#include "HookInstaller.h"

#include <stdio.h>
#include <d3d11.h>
#include "mhook-lib/mhook.h"
#pragma comment(lib, "d3d11.lib")

static HRESULT WINAPI D3D11CreateDevice_Hook(
	_In_opt_ IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
	UINT FeatureLevels,
	UINT SDKVersion,
	_COM_Outptr_opt_ ID3D11Device** ppDevice,
	_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
	_COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext)
{
	printf("D3D11CreateDevice_hook get called.\n");
	::MessageBoxA(NULL, "D3D11CreateDevice_Hook", "", 0);
	HRESULT ret = D3D11CreateDevice(pAdapter, DriverType, Software, Flags, pFeatureLevels,
		FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);

	return ret;
}

static HRESULT WINAPI D3D11CreateDeviceAndSwapChain_Hook(__in_opt IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType,
	HMODULE Software, UINT Flags,
	__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL *pFeatureLevels,
	UINT FeatureLevels, UINT SDKVersion,
	__in_opt CONST DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	__out_opt IDXGISwapChain **ppSwapChain, __out_opt ID3D11Device **ppDevice,
	__out_opt D3D_FEATURE_LEVEL *pFeatureLevel,
	__out_opt ID3D11DeviceContext **ppImmediateContext)
{
	printf("D3D11CreateDeviceAndSwapChain_Hook get called.\n");
	::MessageBoxA(NULL, "D3D11CreateDeviceAndSwapChain_Hook", "", 0);
	return D3D11CreateDeviceAndSwapChain(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
		SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
}

bool InstallHook()
{
	printf("SetHook_D3D11...\n");

	PFN_D3D11_CREATE_DEVICE D3D11CreateDevice_OriginalPtr = (PFN_D3D11_CREATE_DEVICE)::GetProcAddress(::GetModuleHandle(TEXT("d3d11.dll")), "D3D11CreateDevice");
	if (Mhook_SetHook((PVOID*)&D3D11CreateDevice_OriginalPtr, D3D11CreateDevice_Hook))
	{
		printf("Hook D3D11CreateDevice successfully.\n");
	}
	else
	{
		printf("Hook D3D11CreateDevice failed.\n");
	}

	PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN D3D11CreateDeviceAndSwapChain_OriginalPtr = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)::GetProcAddress(::GetModuleHandle(TEXT("d3d11.dll")), "D3D11CreateDeviceAndSwapChain");
	if (Mhook_SetHook((PVOID*)&D3D11CreateDeviceAndSwapChain_OriginalPtr, D3D11CreateDeviceAndSwapChain_Hook))
	{
		printf("Hook D3D11CreateDeviceAndSwapChain successfully.\n");
	}
	else
	{
		printf("Hook D3D11CreateDeviceAndSwapChain failed.\n");
	}

	return true;
}

void UninstallHook()
{

}