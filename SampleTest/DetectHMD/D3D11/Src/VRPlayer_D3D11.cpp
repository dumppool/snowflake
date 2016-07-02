#include "stdafx.h"
#include "VRPlayer_D3D11.h"

#include "RenderContext_D3D11.h"

#define ENABLE_SIMPLE_TEST 1

bool InitializeRenderDevice(HINSTANCE hInst, HWND hWnd)
{
#if ENABLE_SIMPLE_TEST
	return RenderContext_D3D11::Get()->Initialize_Test(hInst, hWnd);
#else
	return (RenderContext_D3D11::Get()->Initialize(hInst, hWnd) &&
		RenderContext_D3D11::Get()->InitializeRHI());
#endif
}

void Tick(float DeltaSeconds)
{
#if ENABLE_SIMPLE_TEST
	RenderContext_D3D11::Get()->Draw_Test(DeltaSeconds);
#else
	RenderContext_D3D11::Get()->Draw(DeltaSeconds);
#endif
}