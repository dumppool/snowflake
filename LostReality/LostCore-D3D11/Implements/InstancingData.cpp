/*
* file InstancingData.cpp
*
* author luoxw
* date 2017/12/03
*
*
*/

#include "stdafx.h"
#include "InstancingData.h"

using namespace LostCore;

D3D11::FInstancingData::FInstancingData()
	: Flags(0)
	, Buffer(nullptr)
	, BufferSize(0)
	, NumInstances(0)
	, Stride(0)
{

}

D3D11::FInstancingData::~FInstancingData()
{
	Flags = 0;
	Buffer = nullptr;
	BufferSize = 0;
	NumInstances = 0;
	Stride = 0;
}

void D3D11::FInstancingData::Commit()
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecCommit(this);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(this, &ExecCommit));
	}
}

void D3D11::FInstancingData::ExecConstructBuffer(void* p, const FBuf& buf)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FInstancingData::ExecConstructBuffer";
	auto pthis = (FInstancingData*)p;
	auto device = FRenderContext::GetDevice(head);
	auto result = ConstructBuffer(device, buf.data(), buf.size(), true, D3D11_BIND_VERTEX_BUFFER, pthis->Buffer);
	assert(result == SSuccess);
	pthis->BufferSize = buf.size();
}

void D3D11::FInstancingData::ExecCommit(void* p)
{
	assert(FRenderContext::Get()->InRenderThread());
	FRenderContext::Get()->CommitInstancingData((FInstancingData*)p);
}

void D3D11::FInstancingData::ExecSetVertexElement(void* p, uint32 flags)
{
	assert(FRenderContext::Get()->InRenderThread());
	auto pthis = (FInstancingData*)p;
	pthis->Flags = flags;
}

void D3D11::FInstancingData::ExecUpdate(void* p, const FBuf& buf, uint32 numInstances)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FInstancingData::ExecUpdate";
	auto pthis = (FInstancingData*)p;
	auto cxt = FRenderContext::GetDeviceContext(head);
	if (buf.size() > pthis->BufferSize)
	{
		ExecConstructBuffer(p, buf);
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE mapped;
		cxt->Map(pthis->Buffer.GetReference(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, buf.data(), buf.size());
		cxt->Unmap(pthis->Buffer.GetReference(), 0);
	}

	pthis->NumInstances = numInstances;
	if (numInstances != 0)
	{
		pthis->Stride = buf.size() / numInstances;
	}
}

void D3D11::FInstancingData::SetVertexElement(uint32 flags)
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecSetVertexElement(this, flags);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecSetVertexElement, placeholders::_1, flags)));
	}
}

void D3D11::FInstancingData::Update(const void* buf, uint32 sz, uint32 numInstances)
{
	FBuf dst(sz);
	memcpy(dst.data(), buf, sz);
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecUpdate(this, dst, numInstances);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecUpdate, placeholders::_1, dst, numInstances)));
	}
}

uint32 D3D11::FInstancingData::GetFlags() const
{
	return Flags;
}

TRefCountPtr<ID3D11Buffer> D3D11::FInstancingData::GetBuffer() const
{
	return Buffer;
}

uint32 D3D11::FInstancingData::GetNumInstances() const
{
	return NumInstances;
}

uint32 D3D11::FInstancingData::GetStride() const
{
	return Stride;
}
