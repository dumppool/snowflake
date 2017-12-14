/*
* file PrimitiveGroupRequest.cpp
*
* author luoxw
* date 2017/11/01
*
*
*/

#include "stdafx.h"
#include "PrimitiveGroup.h"

using namespace LostCore;

D3D11::FPrimitiveGroup::FPrimitiveGroup()
	: VertexBuffer(nullptr)
	, IndexBuffer(nullptr)
	, Stride(0)
	, Count(0)
	, IndexFormat(DXGI_FORMAT_UNKNOWN)
	, IndexCount(0)
	, Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, RenderOrder(ERenderOrder::Opacity)
{
}

D3D11::FPrimitiveGroup::FPrimitiveGroup(const FPrimitiveGroup & rhs)
{
}

D3D11::FPrimitiveGroup::FPrimitiveGroup(const FPrimitiveGroup && ref)
{
}

D3D11::FPrimitiveGroup::~FPrimitiveGroup()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
}

void D3D11::FPrimitiveGroup::Commit()
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecCommit(this);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, &ExecCommit));
	}
}

void D3D11::FPrimitiveGroup::SetVertexElement(uint32 flags)
{
	Flags = flags;
}

uint32 D3D11::FPrimitiveGroup::GetFlags() const
{
	return Flags;
}

void D3D11::FPrimitiveGroup::SetRenderOrder(ERenderOrder ro)
{
	RenderOrder = ro;
}

ERenderOrder D3D11::FPrimitiveGroup::GetRenderOrder() const
{
	return RenderOrder;
}

void D3D11::FPrimitiveGroup::ConstructVB(const void* buf, uint32 sz, uint32 stride, bool dynamic)
{
	assert(this->VertexBuffer.GetReference() == nullptr);

	FBuf dst(sz);
	memcpy(dst.data(), buf, sz);
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecConstructVB(this, dst, stride, dynamic);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecConstructVB, placeholders::_1, dst, stride, dynamic)));
	}
}

void D3D11::FPrimitiveGroup::ConstructIB(const FBuf& buf, uint32 stride, bool dynamic)
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecConstructIB(this, buf, stride, dynamic);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecConstructIB, placeholders::_1, buf, stride, dynamic)));
	}
}

void D3D11::FPrimitiveGroup::SetTopology(EPrimitiveTopology topo)
{
	switch (topo)
	{
	case EPrimitiveTopology::PointList:
		Topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case EPrimitiveTopology::LineList:
		Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case EPrimitiveTopology::TriangleList:
		Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	default:
		break;
	}
}

void D3D11::FPrimitiveGroup::UpdateVB(const void* buf, uint32 sz, uint32 stride)
{
	FBuf dst(sz);
	memcpy(dst.data(), buf, sz);
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecUpdateVB(this, dst, stride);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecUpdateVB, placeholders::_1, dst, stride)));
	}
}

void D3D11::FPrimitiveGroup::Draw(const vector<FInstancingData*>& batch)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "D3D11::FPrimitiveGroup::Draw";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	uint32 instanceCount = 0;

	vector<ID3D11Buffer*> vbs;
	vector<uint32> strides, offsets;
	vbs.push_back(VertexBuffer.GetReference());
	strides.push_back(Stride);
	offsets.push_back(0);
	for (auto& buf : batch)
	{
		if (instanceCount == 0)
		{
			instanceCount = buf->GetNumInstances();
		}

		assert(instanceCount == buf->GetNumInstances());
		vbs.push_back(buf->GetBuffer());
		strides.push_back(buf->GetStride());
		offsets.push_back(0);
	}

	cxt->IASetVertexBuffers(0, vbs.size(), vbs.data(), strides.data(), offsets.data());
	cxt->IASetIndexBuffer(IndexBuffer.GetReference(), IndexFormat, 0);
	cxt->IASetPrimitiveTopology(Topology);

	if (batch.empty())
	{
		if (IndexBuffer.IsValid())
		{
			cxt->DrawIndexed(IndexCount, 0, 0);
		}
		else
		{
			cxt->Draw(Count, 0);
		}
	}
	else
	{
		if (IndexBuffer.IsValid())
		{
			cxt->DrawIndexedInstanced(IndexCount, instanceCount, 0, 0, 0);
		}
		else
		{
			cxt->DrawInstanced(Count, instanceCount, 0, 0);
		}
	}
}

TRefCountPtr<ID3D11Buffer> D3D11::FPrimitiveGroup::GetVertexBuffer()
{
	return VertexBuffer;
}

void D3D11::FPrimitiveGroup::ExecCommit(void * p)
{
	assert(FRenderContext::Get()->InRenderThread());
	FRenderContext::Get()->CommitPrimitiveGroup((FPrimitiveGroup*)p);
}

void D3D11::FPrimitiveGroup::ExecConstructVB(void* p, const FBuf& buf, uint32 stride, bool bDynamic)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecConstructVB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	auto pthis = (FPrimitiveGroup*)p;
	pthis->Stride = stride;
	if (stride != 0)
	{
		pthis->Count = buf.size() / stride;
	}
	else
	{
		pthis->Count = 0;
	}

	pthis->bIsVBDynamic = bDynamic;
	auto pvb = pthis->VertexBuffer.GetReference();
	assert(pthis->VertexBuffer.GetReference() == nullptr);
	ConstructBuffer(device.GetReference(), buf.data(), buf.size(), pthis->bIsVBDynamic, D3D11_BIND_VERTEX_BUFFER, pthis->VertexBuffer);
}

void D3D11::FPrimitiveGroup::ExecConstructIB(void* p, const FBuf & buf, uint32 stride, bool bDynamic)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecConstructIB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	auto pthis = (FPrimitiveGroup*)p;

	switch (stride)
	{
	case 2:
		pthis->IndexFormat = DXGI_FORMAT_R16_UINT;
		break;
	case 4:
		pthis->IndexFormat = DXGI_FORMAT_R32_UINT;
		break;
	default:
		break;
	}

	pthis->IndexCount = buf.size() / stride;
	pthis->bIsIBDynamic = bDynamic;
	CreatePrimitiveIndex(device.GetReference(), buf, pthis->bIsIBDynamic, pthis->IndexBuffer);
}

void D3D11::FPrimitiveGroup::ExecUpdateVB(void* p, const FBuf& buf, uint32 stride)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecUpdateVB";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);
	auto pthis = (FPrimitiveGroup*)p;

	// 强制释放IndexBuffer
	pthis->IndexBuffer = nullptr;

	// 更新的bytes大于vertex buffer，重新创建
	if (!pthis->VertexBuffer.IsValid() || buf.size() > (pthis->Count * pthis->Stride))
	{
		pthis->VertexBuffer = nullptr;
		ExecConstructVB(pthis, buf, stride, true);
	}
	else
	{
		if (pthis->bIsVBDynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mapped;
			cxt->Map(pthis->VertexBuffer.GetReference(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
			memcpy(mapped.pData, buf.data(), buf.size());
			cxt->Unmap(pthis->VertexBuffer.GetReference(), 0);
		}
		else
		{
			pthis->Count = buf.size() / pthis->Stride;
			D3D11_BOX destRegion;
			destRegion.left = 0;
			destRegion.right = buf.size();
			destRegion.top = 0;
			destRegion.bottom = 1;
			destRegion.front = 0;
			destRegion.back = 1;
			cxt->UpdateSubresource(pthis->VertexBuffer.GetReference(), 0, &destRegion, buf.data(), 0, 0);
		}
	}
}