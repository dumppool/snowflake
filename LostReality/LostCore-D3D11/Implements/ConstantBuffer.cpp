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
	, ByteWidth(0)
	, bDynamic(false)
	, ShaderFlags(0)
	, Buffer(nullptr)
{
}

D3D11::FConstantBuffer::FConstantBuffer(const FConstantBuffer & rhs)
{
}

D3D11::FConstantBuffer::FConstantBuffer(const FConstantBuffer && rval)
{
}

D3D11::FConstantBuffer::~FConstantBuffer()
{
	Buffer = nullptr;
}

bool D3D11::FConstantBuffer::Initialize(int32 byteWidth, bool dynamic)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FConstantBuffer::Initialize";

	auto device = FRenderContext::GetDevice(head);
	ByteWidth = LostCore::GetAlignedSize(byteWidth, 16);
	bDynamic = dynamic;

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// ÔÝ²»¿¼ÂÇimmutable
	desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.ByteWidth = ByteWidth;

	auto hr = device->CreateBuffer(&desc, nullptr, Buffer.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "create buffer(%d) failed: 0x%08x(%d).", ByteWidth, hr, hr);
		return false;
	}

	return true;
}

void D3D11::FConstantBuffer::UpdateBuffer(const FBuf& buf)
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecUpdateBuffer(this, buf);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(
			this, bind(&ExecUpdateBuffer, placeholders::_1, buf)));
	}
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

	if (HAS_FLAGS(SHADER_FLAG_PS, ShaderFlags))
	{
		cxt->PSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_GS, ShaderFlags))
	{
		cxt->GSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_HS, ShaderFlags))
	{
		cxt->HSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_DS, ShaderFlags))
	{
		cxt->DSSetConstantBuffers(ShaderSlot, 1, &ref);
	}

	if (HAS_FLAGS(SHADER_FLAG_CS, ShaderFlags))
	{
		cxt->CSSetConstantBuffers(ShaderSlot, 1, &ref);
	}
}

inline int32 D3D11::FConstantBuffer::GetByteWidth() const
{
	return ByteWidth;
}

inline TRefCountPtr<ID3D11Buffer> D3D11::FConstantBuffer::GetBufferRHI()
{
	return Buffer;
}

bool D3D11::FConstantBuffer::IsValid() const
{
	return Buffer.IsValid();
}

void D3D11::FConstantBuffer::ExecCommit(void * p)
{
	assert(FRenderContext::Get()->InRenderThread());
	auto pthis = (FConstantBuffer*)p;
	FRenderContext::Get()->CommitBuffer(pthis);
}

void D3D11::FConstantBuffer::ExecUpdateBuffer(void* p, const FBuf & buf)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "FConstantBuffer::ExecUpdateBuffer";
	auto cxt = FRenderContext::GetDeviceContext(head);
	auto pthis = (FConstantBuffer*)p;

	auto sz = buf.size();
	if (pthis->ByteWidth != LostCore::GetAlignedSize(sz, 16) && !pthis->Initialize(sz, false))
	{
		return;
	}

	cxt->UpdateSubresource(pthis->Buffer.GetReference(), 0, nullptr, buf.data(), 0, 0);
}

void D3D11::FConstantBuffer::Commit()
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
