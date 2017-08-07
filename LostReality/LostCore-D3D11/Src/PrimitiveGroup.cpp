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
	, Material(nullptr)
{
}

D3D11::FPrimitiveGroup::~FPrimitiveGroup()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;

	assert(Material == nullptr);
}

void D3D11::FPrimitiveGroup::Draw(LostCore::IRenderContext * rc, float sec)
{
	const char* head = "D3D11::FPrimitiveGroup::Draw";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(rc, head);
	if (!cxt.IsValid())
	{
		return;
	}

	auto buf = VertexBuffer.GetReference();
	cxt->IASetVertexBuffers(VertexBufferSlot, VertexBufferNum, &buf, &VertexStride, &VertexBufferOffset);
	cxt->IASetIndexBuffer(IndexBuffer.GetReference(), IndexFormat, IndexBufferOffset);
	cxt->IASetPrimitiveTopology(Topology);

	if (Material != nullptr)
	{
		Material->Draw(rc, sec);
	}

	if (IndexBuffer.IsValid())
	{
		cxt->DrawIndexed(IndexCount, 0, 0);
	}
	else
	{
		cxt->Draw(VertexCount, 0);
	}
}

bool D3D11::FPrimitiveGroup::ConstructVB(IRenderContext* rc, const void * buf, uint32 bytes, uint32 stride, bool bDynamic)
{
	const char* head = "D3D11::FPrimitiveGroup::ConstructVB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(rc, head);
	if (!device.IsValid())
	{
		return false;
	}

	VertexStride = stride;
	VertexBufferSlot = 0;
	VertexBufferNum = 1;
	VertexBufferOffset = 0;
	VertexCount = bytes / stride;
	bIsVBDynamic = bDynamic;
	return SSuccess == CreatePrimitiveVertex(device.GetReference(), buf, bytes, bIsVBDynamic, VertexBuffer);
}

bool D3D11::FPrimitiveGroup::ConstructIB(IRenderContext* rc, const void * buf, uint32 bytes, uint32 stride, bool bDynamic)
{
	const char* head = "D3D11::FPrimitiveGroup::ConstructIB";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(rc, head);
	if (!device.IsValid())
	{
		return false;
	}

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

	IndexBufferOffset = 0;
	IndexCount = bytes / stride;
	bIsIBDynamic = bDynamic;
	return SSuccess == CreatePrimitiveIndex(device.GetReference(), buf, bytes, bIsIBDynamic, IndexBuffer);
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

void D3D11::FPrimitiveGroup::SetMaterial(IMaterial * mat)
{
	Material = mat;
}

const IMaterial * D3D11::FPrimitiveGroup::GetMaterial() const
{
	return Material;
}

void D3D11::FPrimitiveGroup::UpdateVB(LostCore::IRenderContext * rc, const void * buf, uint32 bytes)
{
	const char* head = "D3D11::FPrimitiveGroup::UpdateVB";
	TRefCountPtr<ID3D11DeviceContext> cxt = FRenderContext::GetDeviceContext(rc, head);
	if (!cxt.IsValid())
	{
		return;
	}

	// 强制释放IndexBuffer
	IndexBuffer = nullptr;

	//assert(bIsVBDynamic);

	// 更新的bytes大于vertex buffer，重新创建
	if (bytes > (VertexCount * VertexStride))
	{
		VertexBuffer = nullptr;
		TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(rc, head);
		VertexCount = bytes / VertexStride;
		assert(SSuccess == CreatePrimitiveVertex(device.GetReference(), buf, bytes, bIsVBDynamic, VertexBuffer));
	}
	else
	{
		D3D11_BOX destRegion;
		destRegion.left = 0;
		destRegion.right = bytes;
		destRegion.top = 0;
		destRegion.bottom = 1;
		destRegion.front = 0;
		destRegion.back = 1;
		cxt->UpdateSubresource(VertexBuffer.GetReference(), 0, &destRegion, buf, 0, 0);
	}
}
