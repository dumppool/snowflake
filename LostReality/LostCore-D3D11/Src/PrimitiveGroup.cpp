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
{
}

D3D11::FPrimitiveGroup::~FPrimitiveGroup()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
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

	if (Material != nullptr)
	{
		Material->Draw(rc, sec);
	}

	cxt->DrawIndexed(IndexCount, 0, 0);
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
	return SSuccess == CreatePrimitiveVertex(device.GetReference(), buf, bytes, bDynamic, VertexBuffer.GetInitReference());
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
	return SSuccess == CreatePrimitiveIndex(device.GetReference(), buf, bytes, bDynamic, IndexBuffer.GetInitReference());
}

void D3D11::FPrimitiveGroup::SetMaterial(IMaterial * mat)
{
	Material = mat;
}

const IMaterial * D3D11::FPrimitiveGroup::GetMaterial() const
{
	return Material;
}
