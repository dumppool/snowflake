/*
* file ShaderParameter.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#include "stdafx.h"
#include "ShaderParameter.h"
#include "RenderContext.h"

D3D11::FShaderParameter::FShaderParameter()
	: Buffer(nullptr)
	, ByteWidth(0)
{
}

D3D11::FShaderParameter::~FShaderParameter()
{
	Buffer = nullptr;
	ByteWidth = 0;
}

bool D3D11::FShaderParameter::Construct(LostCore::IRenderContext* rc, uint32 bytes, bool bDynamic)
{
	const char* head = "D3D11::FShaderParameter::Construct";
	ID3D11Device* device = FRenderContext::GetDevice(rc, head);
	if (device == nullptr)
	{
		return false;
	}

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// does not consider immutable
	desc.Usage = bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	desc.ByteWidth = bytes;

	assert(SUCCEEDED(device->CreateBuffer(&desc, nullptr, Buffer.GetInitReference())));
	ByteWidth = bytes;
	return true;
}

void D3D11::FShaderParameter::UpdateBuffer(LostCore::IRenderContext * rc, const void * buf, uint32 bytes)
{
	assert(ByteWidth == bytes);

	const char* head = "D3D11::FShaderParameter::UpdateBuffer";
	ID3D11DeviceContext* cxt = FRenderContext::GetDeviceContext(rc, head);
	if (cxt == nullptr)
	{
		return;
	}

	cxt->UpdateSubresource(Buffer.GetReference(), 0, nullptr, buf, 0, 0);
}
