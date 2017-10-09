/*
* file SamplerStateDef.cpp
*
* author luoxw
* date 2017/04/25
*
*
*/

#include "stdafx.h"
#include "SamplerStateDef.h"

D3D11::FSamplerStateMap::FSamplerStateMap() : bInitialized(false)
{
}

D3D11::FSamplerStateMap::~FSamplerStateMap()
{
	ReleaseComObjects();
}

void D3D11::FSamplerStateMap::InitializeState(uint32 flags)
{
	const char* head = "FSamplerStateMap::InitializeState";
	auto device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return;
	}

	TRefCountPtr<ID3D11SamplerState> state;

	CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
	HRESULT hr = device->CreateSamplerState(&desc, state.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Create sampler state failed: 0x%08x(%d)", hr, hr);
	}

	StateMap[flags] = state;
	LVMSG(head, "Created sampler state for 0x%08x.", flags);
}

void D3D11::FSamplerStateMap::Initialize()
{
	if (bInitialized)
	{
		return;
	}

	InitializeState(0);

	bInitialized = true;
}

void D3D11::FSamplerStateMap::ReleaseComObjects()
{
	if (!bInitialized)
	{
		return;
	}

	for (auto& element : StateMap)
	{
		element.second = nullptr;
	}

	StateMap.clear();
	bInitialized = false;
}

TRefCountPtr<ID3D11SamplerState> D3D11::FSamplerStateMap::GetState(uint32 flags)
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

	return TRefCountPtr<ID3D11SamplerState>();
}
