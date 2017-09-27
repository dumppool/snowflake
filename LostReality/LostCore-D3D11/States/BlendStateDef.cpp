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

void D3D11::FBlendStateMap::InitializeState(uint32 overallFlags, const array<uint32, 8>& flags)
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
	desc.AlphaToCoverageEnable = TRUE;
	desc.IndependentBlendEnable = FALSE;
	for (int32 i = 0; i < 8; i++)
	{
		auto& f = flags[i];
		auto blendMode = (EBlendMode)(f & BLEND_MODE_MASK);
		if (blendMode == EBlendMode::None)
		{
			desc.RenderTarget[0].BlendEnable = FALSE;
			desc.AlphaToCoverageEnable = FALSE;
			desc.IndependentBlendEnable = FALSE;
		}
		else if (blendMode == EBlendMode::Add)
		{
			desc.AlphaToCoverageEnable = TRUE;
			desc.IndependentBlendEnable = FALSE;
			desc.RenderTarget[0].BlendEnable = TRUE;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		}
		else if (blendMode == EBlendMode::AlphaBlend)
		{
			desc.AlphaToCoverageEnable = TRUE;
			desc.IndependentBlendEnable = FALSE;
			desc.RenderTarget[0].BlendEnable = TRUE;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		}
	}

	HRESULT hr = device->CreateBlendState(&desc, state.GetInitReference());
	if (FAILED(hr))
	{
		LVERR("FBlendState_Add::GetState", "create blend state failed: 0x%08x(%d)", hr, hr);
	}
}

void D3D11::FBlendStateMap::Initialize()
{
	const char* head = "FBlendStateMap::Initialize";

	if (bInitialized)
	{
		return;
	}

	StateMap.insert(std::make_pair(FBlendState_Solid::GetName(), FBlendState_Solid::GetState(device)));
	LVMSG(head, "Insert blend state[%s]", FBlendState_Solid::GetName().c_str());

	StateMap.insert(std::make_pair(FBlendState_Add::GetName(), FBlendState_Add::GetState(device)));
	LVMSG(head, "Insert blend state[%s]", FBlendState_Add::GetName().c_str());

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
	if (bInitialized)
	{
		for (auto element : StateMap)
		{
			if (element.first.compare(key) == 0)
			{
				return element.second;
			}
		}
	}

	return TRefCountPtr<ID3D11BlendState>();
}
