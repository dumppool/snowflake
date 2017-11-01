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
	, Thread(new FTickThread(this, "RenderContext"))
	, InitialCommands(true)
	, Commands(true)
{
}

D3D11::FRenderContext::~FRenderContext()
{
	SAFE_DELETE(Thread);
}

bool D3D11::FRenderContext::Initialize()
{
	return true;
}

void D3D11::FRenderContext::Tick()
{
	FCmd cmd;
	while (InitialCommands.Pop(cmd))
	{
		cmd();
	}

	if (!Device.IsValid() || !Context.IsValid() || !SwapChain.IsValid() || ActivedPipeline == nullptr)
	{
		return;
	}

	while (Commands.Pop(cmd))
	{
		cmd();
	}

	BeginFrame();
	RenderFrame();
	EndFrame();
}

void D3D11::FRenderContext::Destroy()
{
	DestroyPipelines();
	DestroyStateObjects();

	RenderTarget = nullptr;
	DepthStencil = nullptr;
	GlobalConstantBuffer = nullptr;

	SwapChain = nullptr;
	Context = nullptr;

	//ReportLiveObjects();
	Device = nullptr;
}

void D3D11::FRenderContext::InitializeDevice(LostCore::EContextID id, HWND wnd, bool bWindowed, int32 width, int32 height)
{
	PushInitialCommand([=]()
	{
		ExecInitializeDevice(id, wnd, bWindowed, width, height);
	});
}

void D3D11::FRenderContext::SetViewProjectMatrix(const FFloat4x4 & vp)
{
	Param.ViewProject = vp;
}

void D3D11::FRenderContext::FirstCommit()
{
	FBuf buf;
	Param.GetBuffer(buf);
	GlobalConstantBuffer->UpdateBuffer(buf);
	GlobalConstantBuffer->Commit();
}

void D3D11::FRenderContext::FinishCommit()
{
	if (ActivedPipeline != nullptr)
	{
		ActivedPipeline->FinishCommit();
	}
}

void D3D11::FRenderContext::PushCommand(const FCmd & cmd)
{
	Commands.Push(cmd);
}

void D3D11::FRenderContext::ExecInitializeDevice(LostCore::EContextID id, HWND wnd, bool bWindowed, int32 width, int32 height)
{
	assert(InRenderThread());

	ContextID = id;
	D3D11::CreateDevice(ContextID, Device, Context);
	GlobalConstantBuffer->SetShaderSlot(SHADER_SLOT_GLOBAL);
	GlobalConstantBuffer->SetShaderFlags(SHADER_FLAG_VS | SHADER_FLAG_PS);
	InitializeStateObjects();
	InitializePipelines();

	D3D11::CreateSwapChain(Device.GetReference(), wnd, bWindowed, width, height, SwapChain);

	assert(RenderTarget == nullptr);
	{
		RenderTarget = FTexture2DPtr(new FTexture2D);
		//RenderTarget->Construct(this, width, height, SSwapChainTextureFormat, false, true, true, false);
		RenderTarget->ConstructFromSwapChain(SwapChain);
	}

	assert(DepthStencil == nullptr);
	{
		DepthStencil = FTexture2DPtr(new FTexture2D);
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
}

void D3D11::FRenderContext::BeginFrame()
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


	ActivedPipeline->BeginFrame();
}

void D3D11::FRenderContext::RenderFrame()
{
	ActivedPipeline->RenderFrame();
}

void D3D11::FRenderContext::EndFrame()
{
	ActivedPipeline->EndFrame();
	SwapChain->Present(0, 0);
}

void D3D11::FRenderContext::PushInitialCommand(const FCmd & cmd)
{
	InitialCommands.Push(cmd);
}

void D3D11::FRenderContext::CommitPrimitiveGroup(const FPrimitiveGroupPtr& pg)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitPrimitiveGroup(pg);
}

void D3D11::FRenderContext::CommitBuffer(const FConstantBufferPtr& buf)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitBuffer(buf);
}

void D3D11::FRenderContext::CommitShaderResource(const FTexture2DPtr& srv)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitShaderResource(srv);
}

thread::id D3D11::FRenderContext::GetThreadId() const
{
	return Thread->GetId();
}

bool D3D11::FRenderContext::InRenderThread() const
{
	return this_thread::get_id() == GetThreadId();
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
