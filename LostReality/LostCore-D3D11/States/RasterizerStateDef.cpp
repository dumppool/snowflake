/*
* file RasterizerStateDef.cpp
*
* author luoxw
* date 2017/03/02
*
*
*/

#include "stdafx.h"
#include "RasterizerStateDef.h"

D3D11::FRasterizerStateMap::FRasterizerStateMap() : bInitialized(false)
{
}

D3D11::FRasterizerStateMap::~FRasterizerStateMap()
{
	ReleaseComObjects();
}

void D3D11::FRasterizerStateMap::InitializeState(uint32 flags)
{
	const char* head = "FRasterizerStateMap::InitializeState";
	auto device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return;
	}

	TRefCountPtr<ID3D11RasterizerState> state;

	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AntialiasedLineEnable = FALSE;

	if (HAS_FLAGS(RAS_CULL_BACK, flags))
	{
		desc.CullMode = D3D11_CULL_BACK;
	}
	else if (HAS_FLAGS(RAS_CULL_FRONT, flags))
	{
		desc.CullMode = D3D11_CULL_FRONT;
	}
	else
	{
		desc.CullMode = D3D11_CULL_NONE;
	}

	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = FALSE;
	desc.FillMode = HAS_FLAGS(RAS_FILL_WIREFRAME, flags) ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.ScissorEnable = FALSE;
	desc.SlopeScaledDepthBias = 0.0f;
	HRESULT hr = device->CreateRasterizerState(&desc, state.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Create rasterizer state failed: 0x%08x(%d)", hr, hr);
	}

	StateMap[flags] = state;
	LVMSG(head, "Created rasterizer for 0x08x.", flags);
}

void D3D11::FRasterizerStateMap::Initialize()
{
	if (bInitialized)
	{
		return;
	}

	InitializeState(0);
	InitializeState(RAS_FILL_WIREFRAME);
	bInitialized = true;
}

void D3D11::FRasterizerStateMap::ReleaseComObjects()
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

TRefCountPtr<ID3D11RasterizerState> D3D11::FRasterizerStateMap::GetState(uint32 flags)
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

	return TRefCountPtr<ID3D11RasterizerState>();
}
