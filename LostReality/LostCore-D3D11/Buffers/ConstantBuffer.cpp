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

bool D3D11::FConstantBuffer::Initialize(int32 byteWidth, bool dynamic)
{
	ByteWidth = byteWidth;
	bDynamic = dynamic;

	const char* head = "D3D11::FConstantBuffer::Initialize";
	auto device = FRenderContext::GetDevice(head);
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

void D3D11::FConstantBuffer::UpdateBuffer(const void * buf, int32 sz)
{
	const char* head = "D3D11::FConstantBuffer::UpdateBuffer";
	auto cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	assert(sz == ByteWidth);
	assert(Buffer.IsValid());

	cxt->UpdateSubresource(Buffer.GetReference(), 0, nullptr, buf, 0, 0);
}

void D3D11::FConstantBuffer::Bind(int32 slot)
{
	const char* head = "D3D11::FConstantBuffer::Bind";
	auto cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	assert(slot >= 0);

	auto ref = Buffer.GetReference();
	if ((slot&SHADER_SLOT_VS) == SHADER_SLOT_VS)
	{
		cxt->VSSetConstantBuffers(slot&SHADER_SLOT_MASK, 1, &ref);
	}

	if ((slot&SHADER_SLOT_PS) == SHADER_SLOT_PS)
	{
		cxt->PSSetConstantBuffers(slot&SHADER_SLOT_MASK, 1, &ref);
	}
	
	if ((slot&SHADER_SLOT_GS) == SHADER_SLOT_GS)
	{
		cxt->GSSetConstantBuffers(slot&SHADER_SLOT_MASK, 1, &ref);
	}
	
	if ((slot&SHADER_SLOT_HS) == SHADER_SLOT_HS)
	{
		cxt->HSSetConstantBuffers(slot&SHADER_SLOT_MASK, 1, &ref);
	}
	
	if ((slot&SHADER_SLOT_DS) == SHADER_SLOT_DS)
	{
		cxt->DSSetConstantBuffers(slot&SHADER_SLOT_MASK, 1, &ref);
	}
	
	if ((slot&SHADER_SLOT_CS) == SHADER_SLOT_CS)
	{
		cxt->CSSetConstantBuffers(slot&SHADER_SLOT_MASK, 1, &ref);
	}
}
