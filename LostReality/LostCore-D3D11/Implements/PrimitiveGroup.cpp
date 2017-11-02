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
	, VertexBufferSlot(0)
	, VertexBufferNum(0)
	, VertexStride(0)
	, VertexBufferOffset(0)
	, VertexCount(0)
	, IndexFormat(DXGI_FORMAT_UNKNOWN)
	, IndexBufferOffset(0)
	, IndexCount(0)
	, Topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	, RenderOrder(ERenderOrder::Opacity)
	, Fence(1)
{
	Fence.store(1);

	assert(VertexBuffer.GetReference() == nullptr);
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
	VertexElement = flags;
}

uint32 D3D11::FPrimitiveGroup::GetVertexElement() const
{
	return VertexElement;
}

void D3D11::FPrimitiveGroup::SetRenderOrder(ERenderOrder ro)
{
	RenderOrder = ro;
}

ERenderOrder D3D11::FPrimitiveGroup::GetRenderOrder() const
{
	return RenderOrder;
}

void D3D11::FPrimitiveGroup::ConstructVB(const FBuf& buf, uint32 stride, bool dynamic)
{
	assert(this->VertexBuffer.GetReference() == nullptr);
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecConstructVB(this, buf, stride, dynamic);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecConstructVB, placeholders::_1, buf, stride, dynamic)));
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

void D3D11::FPrimitiveGroup::UpdateVB(const FBuf& buf)
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecUpdateVB(this, buf);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecUpdateVB, placeholders::_1, buf)));
	}
}

void D3D11::FPrimitiveGroup::Draw()
{
	const char* head = "D3D11::FPrimitiveGroup::Draw";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	auto buf = VertexBuffer.GetReference();
	cxt->IASetVertexBuffers(VertexBufferSlot, VertexBufferNum, &buf, &VertexStride, &VertexBufferOffset);
	cxt->IASetIndexBuffer(IndexBuffer.GetReference(), IndexFormat, IndexBufferOffset);
	cxt->IASetPrimitiveTopology(Topology);

	if (IndexBuffer.IsValid())
	{
		cxt->DrawIndexed(IndexCount, 0, 0);
	}
	else
	{
		cxt->Draw(VertexCount, 0);
	}
}

void D3D11::FPrimitiveGroup::ExecCommit(void * p)
{
	assert(FRenderContext::Get()->InRenderThread());
	FRenderContext::Get()->CommitPrimitiveGroup((FPrimitiveGroup*)p);
}

void D3D11::FPrimitiveGroup::ExecConstructVB(void* p, const FBuf & buf, uint32 stride, bool bDynamic)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecConstructVB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	auto pthis = (FPrimitiveGroup*)p;
	pthis->VertexStride = stride;
	pthis->VertexBufferSlot = 0;
	pthis->VertexBufferNum = 1;
	pthis->VertexBufferOffset = 0;
	pthis->VertexCount = buf.size() / stride;
	pthis->bIsVBDynamic = bDynamic;
	auto pvb = pthis->VertexBuffer.GetReference();
	auto v = pthis->Fence.load();
	assert(pthis->VertexBuffer.GetReference() == nullptr);
	CreatePrimitiveVertex(device.GetReference(), buf, pthis->bIsVBDynamic, pthis->VertexBuffer);
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

	pthis->IndexBufferOffset = 0;
	pthis->IndexCount = buf.size() / stride;
	pthis->bIsIBDynamic = bDynamic;
	CreatePrimitiveIndex(device.GetReference(), buf, pthis->bIsIBDynamic, pthis->IndexBuffer);
}

void D3D11::FPrimitiveGroup::ExecUpdateVB(void* p, const FBuf & buf)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecUpdateVB";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);
	auto pthis = (FPrimitiveGroup*)p;

	// 强制释放IndexBuffer
	pthis->IndexBuffer = nullptr;

	//assert(bIsVBDynamic);

	// 更新的bytes大于vertex buffer，重新创建
	if (buf.size() > (pthis->VertexCount * pthis->VertexStride))
	{
		pthis->VertexBuffer = nullptr;
		TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
		pthis->VertexCount = buf.size() / pthis->VertexStride;
		auto result = CreatePrimitiveVertex(device.GetReference(), buf, pthis->bIsVBDynamic, pthis->VertexBuffer);
		assert(SSuccess == result);
	}
	else
	{
		pthis->VertexCount = buf.size() / pthis->VertexStride;
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
