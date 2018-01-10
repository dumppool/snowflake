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
	, Commands(true)
	, Initializer(nullptr)
	, bIsThreadRunning(true)
	, Thread(new FThread(this, "Render Context"))
{
}

D3D11::FRenderContext::~FRenderContext()
{
	if (bIsThreadRunning)
	{
		SAFE_DELETE(Thread);
	}
}

bool D3D11::FRenderContext::Initialize()
{
	return true;
}

void D3D11::FRenderContext::Tick()
{
	if (Initializer != nullptr)
	{
		Initializer();
		Initializer = nullptr;
	}

	if (!Device.IsValid() || !Context.IsValid() || !SwapChain.IsValid() || ActivedPipeline == nullptr)
	{
		return;
	}

	FCommandQueue<FContextCommand> cmds(false);

	{
		static FStackCounterRequest SCounter("Sync reading");
		FScopedStackCounterRequest scopedCounter(SCounter);
		Commands.SyncRead(&cmds);
	}

	FContextCommand cmd;
	while (cmds.Pop(cmd))
	{
		cmd.Exec();
	}

	BeginFrame();
	RenderFrame();
	EndFrame();

	for (auto& item : UpdateGroup)
	{
		item.Exec();
	}

	FlushDeallocating();
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

	bIsThreadRunning = false;
}

bool D3D11::FRenderContext::IsThreadPrivate() const
{
	return false;
}

bool D3D11::FRenderContext::IsLoop() const
{
	return true;
}

void D3D11::FRenderContext::InitializeDevice(LostCore::EContextID id, HWND wnd, bool bWindowed, int32 width, int32 height)
{
	Initializer = [=]()
	{
		ExecInitializeDevice(id, wnd, bWindowed, width, height);
	};
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
	static FStackCounterRequest SCounter("Sync committing");
	FScopedStackCounterRequest scopedCounter(SCounter);
	Commands.SyncCommit();
}

void D3D11::FRenderContext::PushCommand(const FContextCommand & cmd)
{
	Commands.Ref().Push(cmd);
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
		RenderTarget = (new FTexture2D);
		//RenderTarget->Construct(this, width, height, SSwapChainTextureFormat, false, true, true, false);
		RenderTarget->ConstructFromSwapChain(SwapChain);
	}

	assert(DepthStencil == nullptr);
	{
		DepthStencil = (new FTexture2D);
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

void D3D11::FRenderContext::CommitPrimitiveGroup(FPrimitiveGroup* pg)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitPrimitiveGroup(pg);
}

void D3D11::FRenderContext::CommitBuffer(FConstantBuffer* buf)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitBuffer(buf);
}

void D3D11::FRenderContext::CommitShaderResource(FTexture2D* srv)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitShaderResource(srv);
}

void D3D11::FRenderContext::CommitInstancingData(FInstancingData* buf)
{
	assert(ActivedPipeline != nullptr);
	ActivedPipeline->CommitInstancingData(buf);
}

thread::id D3D11::FRenderContext::GetThreadId() const
{
	return Thread->GetId();
}

bool D3D11::FRenderContext::InRenderThread() const
{
	return this_thread::get_id() == GetThreadId();
}

void D3D11::FRenderContext::DeallocPrimitiveGroup(LostCore::IPrimitive* pg)
{
	if (InRenderThread())
	{
		DeallocatingPrimitiveGroups.push_back(pg);
	}
	else
	{
		PushCommand(FContextCommand(this, [=](void* p) {
			((FRenderContext*)p)->DeallocatingPrimitiveGroups.push_back(pg);
		}));
	}
}

void D3D11::FRenderContext::DeallocInstancingData(LostCore::IInstancingData* data)
{
	if (InRenderThread())
	{
		DeallocatingInstancingDatas.push_back(data);
	}
	else
	{
		PushCommand(FContextCommand(this, [=](void* p) {
			((FRenderContext*)p)->DeallocatingInstancingDatas.push_back(data);
		}));
	}
}

void D3D11::FRenderContext::DeallocConstantBuffer(LostCore::IConstantBuffer * cb)
{
	if (InRenderThread())
	{
		DeallocatingConstantBuffers.push_back(cb);
	}
	else
	{
		PushCommand(FContextCommand(this, [=](void* p) {
			((FRenderContext*)p)->DeallocatingConstantBuffers.push_back(cb);
		}));
	}
}

void D3D11::FRenderContext::DeallocGdiFont(LostCore::IFont * font)
{
	if (InRenderThread())
	{
		DeallocatingFonts.push_back(font);
	}
	else
	{
		PushCommand(FContextCommand(this, [=](void* p) {
			((FRenderContext*)p)->DeallocatingFonts.push_back(font);
		}));
	}
}

void D3D11::FRenderContext::FlushDeallocating()
{
	for (auto item : DeallocatingPrimitiveGroups)
	{
		SAFE_DELETE(item);
	}
	DeallocatingPrimitiveGroups.clear();

	for (auto item : DeallocatingConstantBuffers)
	{
		SAFE_DELETE(item);
	}
	DeallocatingConstantBuffers.clear();

	for (auto item : DeallocatingFonts)
	{
		SAFE_DELETE(item);
	}
	DeallocatingFonts.clear();
}

void D3D11::FRenderContext::AddUpdateCommand(const FContextCommand & obj)
{
	if (InRenderThread())
	{
		UpdateGroup.push_back(obj);
	}
	else
	{
		PushCommand(FContextCommand(nullptr, [=](void*) {
			UpdateGroup.push_back(obj);
		}));
	}
}

void D3D11::FRenderContext::RemoveUpdateCommand(const FContextCommand & obj)
{
	auto it = find(UpdateGroup.begin(), UpdateGroup.end(), obj);
	if (it != UpdateGroup.end())
	{
		UpdateGroup.erase(it);
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
