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

using namespace LostCore;

D3D11::FRenderContext::FRenderContext(LostCore::EContextID id)
	: ContextID(id)
	, Device(nullptr)
	, Context(nullptr)
	, SwapChain(nullptr)
	, ShadeModel(EShadeModel::Undefined)
{
}

D3D11::FRenderContext::~FRenderContext()
{
	SwapChain = nullptr;
	Context = nullptr;
	Device = nullptr;
	ShadeModel = EShadeModel::Undefined;
}

bool D3D11::FRenderContext::Init(HWND wnd, bool bWindowed, int32 width, int32 height)
{
	D3D11::CreateDevice(ContextID, Device.GetInitReference(), Context.GetInitReference());
	D3D11::CreateSwapChain(Device.GetReference(), wnd, bWindowed, width, height, SwapChain.GetInitReference());
	return (Device.GetReference() != nullptr) && (Context.GetReference() != nullptr) && (SwapChain.GetReference() != nullptr);
}

void D3D11::FRenderContext::Fini()
{
	SwapChain = nullptr;
	Context = nullptr;
	Device = nullptr;
}

bool D3D11::FRenderContext::EnableShadeModel(EShadeModel sm)
{
	ShadeModel = sm;
	return false;
}

EShadeModel D3D11::FRenderContext::GetShadeModel() const
{
	return ShadeModel;
}

FMatrix D3D11::FRenderContext::GetViewProjectMatrix() const
{
	return ViewProject;
}

void D3D11::FRenderContext::SetViewProjectMatrix(const FMatrix & vp)
{
	ViewProject = vp;
}

EContextID D3D11::FRenderContext::GetContextID() const
{
	return ContextID;
}

const char * D3D11::FRenderContext::GetContextString() const
{
	return LostCore::GetContextDesc(ContextID);
}

void D3D11::FRenderContext::BeginFrame(float sec)
{
}

void D3D11::FRenderContext::EndFrame(float sec)
{
	if (SwapChain.GetReference() != nullptr)
	{
		SwapChain->Present(0, 0);
	}
}