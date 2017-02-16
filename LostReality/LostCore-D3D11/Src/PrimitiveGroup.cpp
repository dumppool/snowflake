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

void D3D11::FPrimitiveGroup::Draw(float sec, IRenderContext * rc)
{
	ID3D11DeviceContext* cxt = static_cast<ID3D11DeviceContext*>(rc->GetContextRHI());
	if (cxt == nullptr)
	{
		return;
	}

	auto buf = VertexBuffer.GetReference();
	cxt->IASetVertexBuffers(VertexBufferSlot, VertexBufferNum, &buf, &VertexStride, &VertexBufferOffset);
	cxt->IASetIndexBuffer(IndexBuffer.GetReference(), IndexFormat, IndexBufferOffset);

	if (Material != nullptr)
	{
		Material->Draw(sec, rc);
	}

	cxt->DrawIndexed(IndexCount, 0, 0);
}

void * D3D11::FPrimitiveGroup::GetRHI()
{
	return nullptr;
}

bool D3D11::FPrimitiveGroup::SetPrimitive(const char * path)
{
	return false;
}

bool D3D11::FPrimitiveGroup::ConstructVB(IRenderContext* rc, const void * buf, uint32 bytes, uint32 stride, bool bDynamic)
{
	if (rc == nullptr)
	{
		return false;
	}

	ID3D11Device* device = static_cast<ID3D11Device*>(rc->GetDeviceRHI());
	if (device == nullptr)
	{
		return false;
	}

	VertexStride = stride;
	VertexBufferSlot = 0;
	VertexBufferNum = 1;
	VertexBufferOffset = 0;
	VertexCount = bytes / stride;
	return SSuccess == CreatePrimitiveVertex(device, buf, bytes, bDynamic, VertexBuffer.GetInitReference());
}

bool D3D11::FPrimitiveGroup::ConstructIB(IRenderContext* rc, const void * buf, uint32 bytes, uint32 stride, bool bDynamic)
{
	if (rc == nullptr)
	{
		return false;
	}

	ID3D11Device* device = static_cast<ID3D11Device*>(rc->GetDeviceRHI());
	if (device == nullptr)
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
	return SSuccess == CreatePrimitiveIndex(device, buf, bytes, bDynamic, IndexBuffer.GetInitReference());
}

void D3D11::FPrimitiveGroup::SetMaterial(IMaterial * mat)
{
	Material = mat;
	assert(0);
}

const IMaterial * D3D11::FPrimitiveGroup::GetMaterial() const
{
	return Material;
}
