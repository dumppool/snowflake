#include "stdafx.h"
#include "VRPlayer_D3D11.h"

#include "RenderContext_D3D11.h"

bool InitializeRenderDevice(HINSTANCE hInst, HWND hWnd)
{
	return (RenderContext_D3D11::Get()->Initialize(hInst, hWnd) &&
		RenderContext_D3D11::Get()->InitializeRHI());
}

void Tick(float DeltaSeconds)
{
	RenderContext_D3D11::Get()->Draw(DeltaSeconds);
}