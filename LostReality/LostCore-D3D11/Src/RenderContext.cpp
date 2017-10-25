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
#include "Implements/ConstantBuffer.h"
#include "Implements/Texture.h"

#include "Pipelines/ForwardPipeline.h"
#include "Pipelines/DeferredPipeline.h"

using namespace LostCore;

D3D11::FRenderContext::FRenderContext()
	: Device(nullptr)
	, Context(nullptr)
	, SwapChain(nullptr)
	, RenderTarget(nullptr)
	, DepthStencil(nullptr)
	, GlobalConstantBuffer(new FConstantBuffer)
	, ActivedPipeline(nullptr)
{
}

D3D11::FRenderContext::~FRenderContext()
{
	Destroy();
}

bool D3D11::FRenderContext::Initialize(LostCore::EContextID id)
{
	ContextID = id;
	D3D11::CreateDevice(ContextID, Device, Context);
	GlobalConstantBuffer->Initialize(sizeof(Param), false);
	GlobalConstantBuffer->SetShaderSlot(SHADER_SLOT_GLOBAL);
	GlobalConstantBuffer->SetShaderFlags(SHADER_FLAG_VS | SHADER_FLAG_PS);
	InitializeStateObjects();
	InitializePipelines();
	return Device.IsValid();
}

void D3D11::FRenderContext::Destroy()
{
	DestroyPipelines();
	DestroyStateObjects();

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

	assert(RenderTarget == nullptr);
	{
		RenderTarget = new FTexture2D;
		//RenderTarget->Construct(this, width, height, SSwapChainTextureFormat, false, true, true, false);
		RenderTarget->ConstructFromSwapChain(SwapChain);
	}

	assert(DepthStencil == nullptr);
	{
		DepthStencil = new FTexture2D;
		DepthStencil->Construct(width, height, SDepthStencilFormat, true, false, false, false, nullptr, 0);
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
	//const int32 sz = 127 - '!';
	//WCHAR chars[sz];
	//for (int32 i = 0; i < sz; ++i)
	//{
	//	chars[i] = 127 + i;
	//}

	//Font.Initialize(this, LostCore::FFontConfig(), chars, sz);
	return SwapChain.IsValid();
}

void D3D11::FRenderContext::SetViewProjectMatrix(const FFloat4x4 & vp)
{
	Param.ViewProject = vp;
}

void D3D11::FRenderContext::BeginFrame()
{
	if (Context.IsValid() && ActivedPipeline != nullptr)
	{
		auto rtv = RenderTarget->GetRenderTargetRHI().GetReference();
		auto dsv = DepthStencil->GetDepthStencilRHI().GetReference();
		FLOAT clearColor[] = { 0.01f, 0.051f, 0.2f, 1.f };
		Context->ClearRenderTargetView(rtv, clearColor);
		Context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH, 1.f, 0xff);
		Context->OMSetRenderTargets(1, &rtv, dsv);

		Context->RSSetViewports(1, &Viewport);

		//Context->RSSetState(bWireframe ?
		//	FRasterizerStateMap::Get()->GetState("WIREFRAME") :
		//	FRasterizerStateMap::Get()->GetState("SOLID"));

		//Context->OMSetDepthStencilState(FDepthStencilStateMap::Get()->GetState("Z_ENABLE_WRITE"), 0);

		FBufFast buf;
		Param.GetBuffer(buf);
		GlobalConstantBuffer->UpdateBuffer(buf);
		GlobalConstantBuffer->Commit();

		ActivedPipeline->Render();
	}
}

void D3D11::FRenderContext::EndFrame()
{
	if (SwapChain.IsValid())
	{
		SwapChain->Present(0, 0);
	}
}

void D3D11::FRenderContext::CommitPrimitiveGroup(FPrimitiveGroup * pg)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitPrimitiveGroup(pg);
}

void D3D11::FRenderContext::CommitBuffer(FConstantBuffer * buf)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitBuffer(buf);
}

void D3D11::FRenderContext::CommitShaderResource(FTexture2D * srv)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitShaderResource(srv);
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

void D3D11::FRenderContext::InitializeStateObjects()
{
	// 不需要显式的初始化
	//if (Device.IsValid())
	//{
	//	FBlendStateMap::Get()->Initialize();
	//	FRasterizerStateMap::Get()->Initialize();
	//	FDepthStencilStateMap::Get()->Initialize();
	//	FSamplerStateMap::Get()->Initialize();
	//}
}

void D3D11::FRenderContext::DestroyStateObjects()
{
	FBlendStateMap::Get()->ReleaseComObjects();
	FRasterizerStateMap::Get()->ReleaseComObjects();
	FDepthStencilStateMap::Get()->ReleaseComObjects();
	FSamplerStateMap::Get()->ReleaseComObjects();
}

void D3D11::FRenderContext::InitializePipelines()
{
	auto pl = new FForwardPipeline;
	pl->Initialize();
	Pipelines[pl->GetEnum()] = pl;

	ActivedPipeline = pl;
}

void D3D11::FRenderContext::DestroyPipelines()
{
	for (auto it : Pipelines)
	{
		it.second->Destroy();
		SAFE_DELETE(it.second);
	}

	Pipelines.clear();
}
