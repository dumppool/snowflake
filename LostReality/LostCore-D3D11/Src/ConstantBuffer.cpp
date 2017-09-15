/*
* file ConstantBuffer.cpp
*
* author luoxw
* date 2017/06/13
*
*
*/

#include "stdafx.h"
#include "ConstantBuffer.h"

bool D3D11::FConstantBuffer::Initialize(LostCore::IRenderContext * rc)
{
	const char* head = "D3D11::FConstantBuffer::Initialize";
	auto device = FRenderContext::GetDevice(rc, head);
	if (!device.IsValid())
	{
		return false;
	}

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// does not consider immutable
	desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.ByteWidth = ByteWidth;

	if (FAILED(device->CreateBuffer(&desc, nullptr, Buffer.GetInitReference())))
	{
		LVERR(head, "create buffer failed");
		return false;
	}

	return true;
}

bool D3D11::FConstantBuffer::Initialize(LostCore::IRenderContext * rc, int32 byteWidth, bool dynamic, int32 slot)
{
	ByteWidth = byteWidth;
	bDynamic = dynamic;
	Slot = slot;
	return Initialize(rc);
}

void D3D11::FConstantBuffer::UpdateBuffer(LostCore::IRenderContext * rc, const void * buf, int32 sz)
{
	const char* head = "D3D11::FConstantBuffer::UpdateBuffer";
	auto cxt = FRenderContext::GetDeviceContext(rc, head);
	if (!cxt.IsValid())
	{
		return;
	}

	assert(sz == ByteWidth);
	assert(Buffer.IsValid());

	cxt->UpdateSubresource(Buffer.GetReference(), 0, nullptr, buf, 0, 0);
}

void D3D11::FConstantBuffer::Bind(LostCore::IRenderContext * rc)
{
	const char* head = "D3D11::FConstantBuffer::Bind";
	auto cxt = FRenderContext::GetDeviceContext(rc, head);
	if (!cxt.IsValid())
	{
		return;
	}

	assert(Slot >= 0);

	auto ref = Buffer.GetReference();
	if ((Slot&SHADER_SLOT_VS) == SHADER_SLOT_VS)
	{
		cxt->VSSetConstantBuffers(Slot, 1, &ref);
	}

	if ((Slot&SHADER_SLOT_PS) == SHADER_SLOT_PS)
	{
		cxt->PSSetConstantBuffers(Slot, 1, &ref);
	}
	
	if ((Slot&SHADER_SLOT_GS) == SHADER_SLOT_GS)
	{
		cxt->GSSetConstantBuffers(Slot, 1, &ref);
	}
	
	if ((Slot&SHADER_SLOT_HS) == SHADER_SLOT_HS)
	{
		cxt->HSSetConstantBuffers(Slot, 1, &ref);
	}
	
	if ((Slot&SHADER_SLOT_DS) == SHADER_SLOT_DS)
	{
		cxt->DSSetConstantBuffers(Slot, 1, &ref);
	}
	
	if ((Slot&SHADER_SLOT_CS) == SHADER_SLOT_CS)
	{
		cxt->CSSetConstantBuffers(Slot, 1, &ref);
	}
}
