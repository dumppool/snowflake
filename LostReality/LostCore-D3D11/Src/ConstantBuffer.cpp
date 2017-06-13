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

bool D3D11::FConstantBuffer::Initialize(const TRefCountPtr<ID3D11Device>& device)
{
	const char* head = "D3D11::FConstantBuffer::Initialize";
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

void D3D11::FConstantBuffer::UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void * buf, int32 sz)
{
	assert(sz == ByteWidth);
	assert(Buffer.IsValid());

	cxt->UpdateSubresource(Buffer.GetReference(), 0, nullptr, buf, 0, 0);
}

void D3D11::FConstantBuffer::Bind(const TRefCountPtr<ID3D11DeviceContext>& cxt)
{
	assert(Slot >= 0);

	auto ref = Buffer.GetReference();
	cxt->VSSetConstantBuffers(Slot, 1, &ref);
}

void D3D11::FConstantBufferRegister0::UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void * buf, int32 sz)
{
	assert(sz == GetByteWidth());
	assert(GetBufferRHI().IsValid());

	FParam p;
	memcpy(&p, buf, GetByteWidth());
	p.ViewProject = p.ViewProject.GetTranspose();
	cxt->UpdateSubresource(GetBufferRHI().GetReference(), 0, nullptr, &p, 0, 0);
}

void D3D11::FConstantBufferMatrix::UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void * buf, int32 sz)
{
	assert(sz == GetByteWidth());
	assert(GetBufferRHI().IsValid());

	FParam p;
	memcpy(&p, buf, GetByteWidth());
	p.Mat = p.Mat.GetTranspose();
	cxt->UpdateSubresource(GetBufferRHI().GetReference(), 0, nullptr, &p, 0, 0);
}

void D3D11::FConstantBufferSkinned::UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void * buf, int32 sz)
{
	assert(sz == GetByteWidth());
	assert(GetBufferRHI().IsValid());

	FParam p;
	memcpy(&p, buf, sz);
	p.World = p.World.GetTranspose();
	for_each(p.Bones.begin(), p.Bones.end(), [](LostCore::FMatrix& mat) {mat = mat.GetTranspose(); });
	cxt->UpdateSubresource(GetBufferRHI().GetReference(), 0, nullptr, &p, 0, 0);
}
