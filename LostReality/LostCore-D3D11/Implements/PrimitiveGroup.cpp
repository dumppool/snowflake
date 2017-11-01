/*
* file PrimitiveGroup.cpp
*
* author luoxw
* date 2017/02/10
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
{
}

D3D11::FPrimitiveGroup::~FPrimitiveGroup()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
}

void D3D11::FPrimitiveGroup::Commit()
{
	FRenderContext::Get()->CommitPrimitiveGroup(shared_from_this());
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

void D3D11::FPrimitiveGroup::ConstructVB(const FBuf& buf, uint32 stride, bool bDynamic)
{
	assert(this->VertexBuffer.GetReference() == nullptr);
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecConstructVB(buf, stride, bDynamic);
	}
	else
	{
		auto p = shared_from_this();
		FRenderContext::Get()->PushCommand([=]()
		{
			p->ExecConstructVB(buf, stride, bDynamic);
		});
	}
}

void D3D11::FPrimitiveGroup::ConstructIB(const FBuf& buf, uint32 stride, bool bDynamic)
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecConstructIB(buf, stride, bDynamic);
	}
	else
	{
		auto p = shared_from_this();
		FRenderContext::Get()->PushCommand([=]()
		{
			p->ExecConstructIB(buf, stride, bDynamic);
		});
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
		ExecUpdateVB(buf);
	}
	else
	{
		auto p = shared_from_this();
		FRenderContext::Get()->PushCommand([=]()
		{
			p->ExecUpdateVB(buf);
		});
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

void D3D11::FPrimitiveGroup::ExecConstructVB(const FBuf & buf, uint32 stride, bool bDynamic)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecConstructVB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	this->VertexStride = stride;
	this->VertexBufferSlot = 0;
	this->VertexBufferNum = 1;
	this->VertexBufferOffset = 0;
	this->VertexCount = buf.size() / stride;
	this->bIsVBDynamic = bDynamic;
	assert(this->VertexBuffer.GetReference() == nullptr);
	CreatePrimitiveVertex(device.GetReference(), buf, this->bIsVBDynamic, this->VertexBuffer);
}

void D3D11::FPrimitiveGroup::ExecConstructIB(const FBuf & buf, uint32 stride, bool bDynamic)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecConstructIB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);

	switch (stride)
	{
	case 2:
		IndexFormat = DXGI_FORMAT_R16_UINT;
		break;
	case 4:
		IndexFormat = DXGI_FORMAT_R32_UINT;
		break;
	default:
		break;
	}

	this->IndexBufferOffset = 0;
	this->IndexCount = buf.size() / stride;
	this->bIsIBDynamic = bDynamic;
	CreatePrimitiveIndex(device.GetReference(), buf, this->bIsIBDynamic, this->IndexBuffer);
}

void D3D11::FPrimitiveGroup::ExecUpdateVB(const FBuf & buf)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FPrimitiveGroup::ExecUpdateVB";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(head);

	// 强制释放IndexBuffer
	this->IndexBuffer = nullptr;

	//assert(bIsVBDynamic);

	// 更新的bytes大于vertex buffer，重新创建
	if (buf.size() > (this->VertexCount * this->VertexStride))
	{
		this->VertexBuffer = nullptr;
		TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
		this->VertexCount = buf.size() / this->VertexStride;
		auto result = CreatePrimitiveVertex(device.GetReference(), buf, this->bIsVBDynamic, this->VertexBuffer);
		assert(SSuccess == result);
	}
	else
	{
		this->VertexCount = buf.size() / this->VertexStride;
		D3D11_BOX destRegion;
		destRegion.left = 0;
		destRegion.right = buf.size();
		destRegion.top = 0;
		destRegion.bottom = 1;
		destRegion.front = 0;
		destRegion.back = 1;
		cxt->UpdateSubresource(this->VertexBuffer.GetReference(), 0, &destRegion, buf.data(), 0, 0);
	}
}
