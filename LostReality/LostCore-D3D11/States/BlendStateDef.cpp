/*
* file BlendStateDef.cpp
*
* author luoxw
* date 2017/03/02
*
*
*/

#include "stdafx.h"
#include "BlendStateDef.h"

using namespace LostCore;
using namespace D3D11;

D3D11::FBlendStateMap::FBlendStateMap() : bInitialized(false)
{

}

D3D11::FBlendStateMap::~FBlendStateMap()
{
	ReleaseComObjects();
}

void D3D11::FBlendStateMap::InitializeState(uint32 flags)
{
	const char* head = "FBlendStateMap::InitializeState";
	auto device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return;
	}

	TRefCountPtr<ID3D11BlendState> state;

	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	if (HAS_FLAGS(BS_INDEPENDENT_ALPHA, flags))
	{
		desc.IndependentBlendEnable = TRUE;
	}

	if (HAS_FLAGS(BS_ALPHA_TO_COVERAGE, flags))
	{
		desc.AlphaToCoverageEnable = TRUE;
	}

	auto blendMode = (EBlendMode)((flags >> BLEND_MODE_OFFSET) & 0xff);
	if (blendMode == EBlendMode::None)
	{
		desc.RenderTarget[0].BlendEnable = FALSE;
	}
	else if (blendMode == EBlendMode::Add)
	{
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	}
	else if (blendMode == EBlendMode::AlphaBlend)
	{
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	}

	auto blendWrite = (EBlendWrite)((flags >> BLEND_WRITE_OFFSET) & 0xff);
	if (blendWrite == EBlendWrite::None)
	{
		
	}
	else if (blendWrite == EBlendWrite::R)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED;
	}
	else if (blendWrite == EBlendWrite::G)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_GREEN;
	}
	else if (blendWrite == EBlendWrite::B)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_BLUE;
	}
	else if (blendWrite == EBlendWrite::A)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALPHA;
	}
	else if (blendWrite == EBlendWrite::RG)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | 
			D3D11_COLOR_WRITE_ENABLE_GREEN;
	}
	else if (blendWrite == EBlendWrite::RGB)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED |
			D3D11_COLOR_WRITE_ENABLE_GREEN |
			D3D11_COLOR_WRITE_ENABLE_BLUE;
	}
	else if (blendWrite == EBlendWrite::RGBA)
	{
		desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	HRESULT hr = device->CreateBlendState(&desc, state.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "create blend state failed: 0x%08x(%d)", hr, hr);
	}

	StateMap[flags] = state;
	LVMSG(head, "Created blend state for 0x%08x.", flags);
}

void D3D11::FBlendStateMap::Initialize()
{
	const char* head = "FBlendStateMap::Initialize";

	if (bInitialized)
	{
		return;
	}

	InitializeState(
		((uint32)EBlendMode::None<<BLEND_MODE_OFFSET) |
		((uint32)EBlendWrite::RGBA << BLEND_WRITE_OFFSET));

	InitializeState(
		((uint32)EBlendMode::Add << BLEND_MODE_OFFSET) |
		((uint32)EBlendWrite::RGBA << BLEND_WRITE_OFFSET));

	InitializeState(
		((uint32)EBlendMode::AlphaBlend << BLEND_MODE_OFFSET) |
		((uint32)EBlendWrite::RGBA << BLEND_WRITE_OFFSET));

	bInitialized = true;
}

void D3D11::FBlendStateMap::ReleaseComObjects()
{
	if (!bInitialized)
	{
		return;
	}

	for (auto it = StateMap.begin(); it != StateMap.end(); ++it)
	{
		it->second = nullptr;
	}

	StateMap.clear();
	bInitialized = false;
}

TRefCountPtr<ID3D11BlendState> D3D11::FBlendStateMap::GetState(uint32 flags)
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

	return TRefCountPtr<ID3D11BlendState>();
}
