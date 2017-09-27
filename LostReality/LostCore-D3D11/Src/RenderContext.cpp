/*
* file RenderContext.cpp
*
* author luoxw
* date 2017/01/22
*
*
*/

#include "stdafx.h"
#include "RenderContext.h"
#include "States/BlendStateDef.h"
#include "States/RasterizerStateDef.h"
#include "States/DepthStencilStateDef.h"
#include "States/SamplerStateDef.h"
#include "Buffers/ConstantBuffer.h"
#include "Implements/Texture.h"

using namespace LostCore;

D3D11::FRenderContext::FRenderContext()
	: Device(nullptr)
	, Context(nullptr)
	, SwapChain(nullptr)
	, ShadeModel(EShadeModel::Undefined)
	, bWireframe(false)
	, RenderTarget(nullptr)
	, DepthStencil(nullptr)
	, GlobalConstantBuffer(new FConstantBuffer)
{
}

D3D11::FRenderContext::~FRenderContext()
{
	Destroy();
}

bool D3D11::FRenderContext::Initialize(LostCore::EContextID id)
{
	ContextID = id;
	return SSuccess == D3D11::CreateDevice(ContextID, Device, Context);
}

void D3D11::FRenderContext::Destroy()
{
	ShadeModel = EShadeModel::Undefined;
	bWireframe = false;

	//Font.Destroy();

	FBlendStateMap::Get()->ReleaseComObjects();
	FRasterizerStateMap::Get()->ReleaseComObjects();
	FDepthStencilStateMap::Get()->ReleaseComObjects();
	FSamplerStateMap::Get()->ReleaseComObjects();

	SAFE_DELETE(RenderTarget);
	SAFE_DELETE(DepthStencil);
	SAFE_DELETE(GlobalConstantBuffer);

	SwapChain = nullptr;
	Context = nullptr;

	//ReportLiveObjects();
	Device = nullptr;
}

bool D3D11::FRenderContext::InitializeScreen(HWND wnd, bool bWindowed, int32 width, int32 height)
{
	D3D11::CreateSwapChain(Device.GetReference(), wnd, bWindowed, width, height, SwapChain);
	if (Device.IsValid())
	{
		FBlendStateMap::Get()->Initialize(Device);
		FRasterizerStateMap::Get()->Initialize(Device);
		FDepthStencilStateMap::Get()->Initialize(Device);
		FSamplerStateMap::Get()->Initialize(Device);
	}

	assert(RenderTarget == nullptr);
	{
		RenderTarget = new FTexture2D;
		//RenderTarget->Construct(this, width, height, SSwapChainTextureFormat, false, true, true, false);
		RenderTarget->ConstructFromSwapChain(SwapChain);
	}

	assert(DepthStencil == nullptr);
	{
		DepthStencil = new FTexture2D;
		DepthStencil->Construct(this, width, height, SDepthStencilFormat, true, false, false, false, nullptr, 0);
	}

	Viewport.Width = (FLOAT)width;
	Viewport.Height = (FLOAT)height;
	Viewport.TopLeftX = 0.f;
	Viewport.TopLeftY = 0.f;
	Viewport.MinDepth = 0.f;
	Viewport.MaxDepth = 1.f;

	Param.ScreenWidth = (float)width;
	Param.ScreenHeight = (float)height;
	Param.ScreenWidthRcp = (float)1.f / width;
	Param.ScreenHeightRcp = (float)1.f / height;

	// push & initialize ascii chars
	const int32 sz = 127 - '!';
	WCHAR chars[sz];
	for (int32 i = 0; i < sz; ++i)
	{
		chars[i] = 127 + i;
	}

	//Font.Initialize(this, LostCore::FFontConfig(), chars, sz);

	return Device.IsValid() && Context.IsValid() && SwapChain.IsValid() && GlobalConstantBuffer->Initialize(this, sizeof(Param), false);
}

void D3D11::FRenderContext::SetViewProjectMatrix(const FFloat4x4 & vp)
{
	Param.ViewProject = vp;
}

void D3D11::FRenderContext::BeginFrame(float sec)
{
	if (Context.IsValid())
	{
		auto rtv = RenderTarget->GetRenderTargetRHI().GetReference();
		auto dsv = DepthStencil->GetDepthStencilRHI().GetReference();
		FLOAT clearColor[] = { 0.01f, 0.051f, 0.2f, 1.f };
		Context->ClearRenderTargetView(rtv, clearColor);
		Context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0xff);
		Context->OMSetRenderTargets(1, &rtv, dsv);

		Context->RSSetViewports(1, &Viewport);

		Context->RSSetState(bWireframe ?
			FRasterizerStateMap::Get()->GetState("WIREFRAME") :
			FRasterizerStateMap::Get()->GetState("SOLID"));

		//Context->OMSetDepthStencilState(FDepthStencilStateMap::Get()->GetState("Z_ENABLE_WRITE"), 0);

		GlobalConstantBuffer->UpdateBuffer(this, &Param.GetBuffer(), sizeof(Param));
		GlobalConstantBuffer->Bind(this, 0 | SHADER_SLOT_VS | SHADER_SLOT_PS);
	}
}

void D3D11::FRenderContext::EndFrame(float sec)
{
	if (SwapChain.IsValid())
	{
		SwapChain->Present(0, 0);
	}
}

void D3D11::FRenderContext::ReportLiveObjects()
{
#ifdef _DEBUG
	{
		TRefCountPtr<ID3D11Debug> debugger;
		if (Device.GetReference() != nullptr && SUCCEEDED(Device->QueryInterface(__uuidof(ID3D11Debug), (void**)debugger.GetInitReference())))
		{
			debugger->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
	}
#endif
}
