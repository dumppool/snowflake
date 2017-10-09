/*
* file DepthStencilStateDef.cpp
*
* author luoxw
* date 2017/03/06
*
*
*/

#include "stdafx.h"
#include "DepthStencilStateDef.h"

D3D11::FDepthStencilStateMap::FDepthStencilStateMap() : bInitialized(false)
{
}

D3D11::FDepthStencilStateMap::~FDepthStencilStateMap()
{
	ReleaseComObjects();
}

void D3D11::FDepthStencilStateMap::InitializeState(uint32 flags)
{
	const char* head = "FDepthStencilState::InitializeState";

	TRefCountPtr<ID3D11DepthStencilState> state;

	auto device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return;
	}

	const D3D11_DEPTH_STENCILOP_DESC opNever = {
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_COMPARISON_NEVER };

	const D3D11_DEPTH_STENCILOP_DESC opPass = {
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_STENCIL_OP_KEEP,
		D3D11_COMPARISON_NEVER };

	CD3D11_DEPTH_STENCIL_DESC desc(D3D11_DEFAULT);
	desc.DepthEnable = HAS_FLAGS(DS_DEPTH_READ | DS_DEPTH_WRITE, flags) ? TRUE : FALSE;
	desc.DepthFunc = HAS_FLAGS(DS_DEPTH_READ, flags) ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS;
	desc.DepthWriteMask = HAS_FLAGS(DS_DEPTH_WRITE, flags) ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.StencilEnable = HAS_FLAGS(DS_STENCIL_READ | DS_DEPTH_WRITE, flags) ? TRUE : FALSE;
	desc.StencilReadMask = HAS_FLAGS(DS_STENCIL_READ, flags) ? D3D11_DEFAULT_STENCIL_READ_MASK : 0;
	desc.StencilWriteMask = HAS_FLAGS(DS_STENCIL_WRITE, flags) ? D3D11_DEFAULT_STENCIL_WRITE_MASK : 0;
	desc.FrontFace = opNever;
	desc.BackFace = opNever;

	HRESULT hr = device->CreateDepthStencilState(&desc, state.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Create depth stencil state failed: 0x%08x(%d)", hr, hr);
		return;
	}

	StateMap[flags] = state;
	LVMSG(head, "Created depth stencil state for 0x%08x", flags);
}

void D3D11::FDepthStencilStateMap::Initialize()
{
	if (bInitialized)
	{
		return;
	}

	InitializeState(0);
	InitializeState(DS_DEPTH_READ | DS_DEPTH_WRITE);
	bInitialized = true;
}

void D3D11::FDepthStencilStateMap::ReleaseComObjects()
{
	if (!bInitialized)
	{
		return;
	}

	for (auto& elem : StateMap)
	{
		elem.second = nullptr;
	}

	StateMap.clear();
	bInitialized = false;
}

TRefCountPtr<ID3D11DepthStencilState> D3D11::FDepthStencilStateMap::GetState(uint32 flags)
{
	if (!bInitialized)
	{
		Initialize();
	}

	auto it = StateMap.find(flags);
	if (it != StateMap.end())
	{
		return it->second;
	}

	InitializeState(flags);
	if ((it = StateMap.find(flags)) != StateMap.end())
	{
		return it->second;
	}

	return TRefCountPtr<ID3D11DepthStencilState>();
}
