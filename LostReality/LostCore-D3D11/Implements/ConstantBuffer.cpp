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

D3D11::FConstantBuffer::FConstantBuffer()
	: ShaderSlot(0)
{
}

D3D11::FConstantBuffer::~FConstantBuffer()
{
	Buffer = nullptr;
}

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

void D3D11::FConstantBuffer::SetShaderSlot(int32 slot)
{
	ShaderSlot = slot;
}

int32 D3D11::FConstantBuffer::GetShaderSlot() const
{
	return ShaderSlot;
}

void D3D11::FConstantBuffer::SetShaderFlags(int32 flags)
{
	ShaderFlags = flags;
}

int32 D3D11::FConstantBuffer::GetShaderFlags() const
{
	return ShaderFlags;
}

void D3D11::FConstantBuffer::Bind()
{
	const char* head = "FConstantBuffer::Bind";
	auto cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	auto ref = Buffer.GetReference();
	if (HAS_FLAGS(SHADER_FLAG_VS, ShaderFlags))
	{
		cxt->VSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_VS, ShaderFlags))
	{
		cxt->PSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_VS, ShaderFlags))
	{
		cxt->GSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_VS, ShaderFlags))
	{
		cxt->HSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_VS, ShaderFlags))
	{
		cxt->DSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_VS, ShaderFlags))
	{
		cxt->CSSetConstantBuffers(ShaderSlot, 1, &ref);
	}
}

void D3D11::FConstantBuffer::Commit()
{
	FRenderContext::Get()->CommitBuffer(this);
}
