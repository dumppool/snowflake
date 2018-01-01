/*
* file Texture.cpp
*
* author luoxw
* date 2017/02/14
*
*
*/

#include "stdafx.h"
#include "Texture.h"
#include "States/SamplerStateDef.h"

using namespace LostCore;

D3D11::FTexture2D::FTexture2D()
	: Texture(nullptr)
	, ShaderResource(nullptr)
	, RenderTarget(nullptr)
	, DepthStencil(nullptr)
	, Sampler(nullptr)
	, BindFlags(0x0)
	, AccessFlags(0x0)
{
}

D3D11::FTexture2D::~FTexture2D()
{
	Texture = nullptr;
	ShaderResource = nullptr;
	RenderTarget = nullptr;
	DepthStencil = nullptr;
	Sampler = nullptr;
	BindFlags = 0x0;
	AccessFlags = 0x0;
}

void D3D11::FTexture2D::CommitShaderResource()
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecCommitShaderResource(this);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(this, &ExecCommitShaderResource));
	}
}

int32 D3D11::FTexture2D::GetWidth() const
{
	return Width;
}

int32 D3D11::FTexture2D::GetHeight() const
{
	return Height;
}

void D3D11::FTexture2D::SetShaderResourceSlot(int32 slot)
{
	ShaderResourceSlot = slot;
}

void D3D11::FTexture2D::SetRenderTargetSlot(int32 slot)
{
	RenderTargetSlot = slot;
}

void D3D11::FTexture2D::BindShaderResource(TRefCountPtr<ID3D11DeviceContext>& cxt)
{
	assert(IsShaderResource());
	auto srv = ShaderResource.GetReference();
	auto sampler = Sampler.GetReference();
	cxt->PSSetShaderResources(ShaderResourceSlot, 1, &srv);
	cxt->PSSetSamplers(ShaderResourceSlot, 1, &sampler);
}

int32 D3D11::FTexture2D::GetShaderResourceSlot() const
{
	return ShaderResourceSlot;
}

int32 D3D11::FTexture2D::GetRenderTargetSlot() const
{
	return RenderTargetSlot;
}

bool D3D11::FTexture2D::Construct(
	uint32 width, 
	uint32 height, 
	uint32 format, 
	bool bIsDepthStencil, 
	bool bIsRenderTarget, 
	bool bIsShaderResource,
	bool bIsWritable,
	void* initialData,
	uint32 initialPitch)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "D3D11::FTexture2D::Construct";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	if (!device.IsValid())
	{
		return false;
	}

	Width = width;
	Height = height;

	BindFlags = 0x0;
	if (bIsDepthStencil)
	{
		BindFlags |= D3D11_BIND_DEPTH_STENCIL;
	}
	else if (bIsRenderTarget)
	{
		BindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	if (bIsShaderResource)
	{
		BindFlags |= D3D11_BIND_SHADER_RESOURCE;
		Sampler = FSamplerStateMap::Get()->GetState(0);
	}

	AccessFlags = 0x0;
	if (bIsWritable)
	{
		AccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ArraySize = 1;
	desc.BindFlags = BindFlags;
	desc.CPUAccessFlags = AccessFlags;
	desc.Format = (DXGI_FORMAT)format;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = bIsWritable ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;

	if (initialData != nullptr)
	{
		D3D11_SUBRESOURCE_DATA sr;
		sr.pSysMem = initialData;
		sr.SysMemPitch = initialPitch;
		sr.SysMemSlicePitch = 0;
		auto succeeded = SUCCEEDED(device->CreateTexture2D(&desc, &sr, Texture.GetInitReference()));
		assert(succeeded);
	}
	else
	{
		auto succeeded = SUCCEEDED(device->CreateTexture2D(&desc, nullptr, Texture.GetInitReference()));
		assert(succeeded);
	}

	if (bIsRenderTarget)
	{
		CD3D11_RENDER_TARGET_VIEW_DESC cdesc(Texture.GetReference(), D3D11_RTV_DIMENSION_TEXTURE2D);
		auto succeeded = SUCCEEDED(device->CreateRenderTargetView(Texture.GetReference(), &cdesc, RenderTarget.GetInitReference()));
		assert(succeeded);
	}
	else if (bIsDepthStencil)
	{
		CD3D11_DEPTH_STENCIL_VIEW_DESC cdesc(Texture.GetReference(), D3D11_DSV_DIMENSION_TEXTURE2D);
		auto succeeded = SUCCEEDED(device->CreateDepthStencilView(Texture.GetReference(), &cdesc, DepthStencil.GetInitReference()));
		assert(succeeded);
	}

	if (bIsShaderResource)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC cdesc(Texture.GetReference(), D3D11_SRV_DIMENSION_TEXTURE2D);
		auto succeeded = SUCCEEDED(device->CreateShaderResourceView(Texture.GetReference(), &cdesc, ShaderResource.GetInitReference()));
		assert(succeeded);
	}

	return true;
}

bool D3D11::FTexture2D::ConstructFromSwapChain(const TRefCountPtr<IDXGISwapChain>& swapChain)
{
	assert(FRenderContext::Get()->InRenderThread());
	const char* head = "D3D11::FTexture2D::ConstructFromSwapChain";

	BindFlags = D3D11_BIND_RENDER_TARGET;
	auto succeeded = SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)Texture.GetInitReference()));
	assert(succeeded);

	D3D11_TEXTURE2D_DESC desc;
	Texture->GetDesc(&desc);
	Width = desc.Width;
	Height = desc.Height;

	TRefCountPtr<ID3D11Device> device;
	succeeded = SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)device.GetInitReference()));
	assert(succeeded);

	CD3D11_RENDER_TARGET_VIEW_DESC cdesc(Texture.GetReference(), D3D11_RTV_DIMENSION_TEXTURE2D);
	succeeded = SUCCEEDED(device->CreateRenderTargetView(Texture.GetReference(), &cdesc, RenderTarget.GetInitReference()));
	assert(succeeded);

	return true;
}

bool D3D11::FTexture2D::IsRenderTarget() const
{
	return (BindFlags & D3D11_BIND_RENDER_TARGET) != 0;
}

TRefCountPtr<ID3D11ShaderResourceView> D3D11::FTexture2D::GetShaderResourceRHI()
{
	return ShaderResource;
}

bool D3D11::FTexture2D::IsShaderResource() const
{
	return (BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0;
}

TRefCountPtr<ID3D11RenderTargetView> D3D11::FTexture2D::GetRenderTargetRHI()
{
	return RenderTarget;
}

bool D3D11::FTexture2D::IsDepthStencil() const
{
	return (BindFlags & D3D11_BIND_DEPTH_STENCIL) != 0;
}

TRefCountPtr<ID3D11DepthStencilView> D3D11::FTexture2D::GetDepthStencilRHI()
{
	return DepthStencil;
}

TRefCountPtr<ID3D11SamplerState> D3D11::FTexture2D::GetSamplerRHI()
{
	return Sampler;
}

bool D3D11::FTexture2D::IsWritable() const
{
	return (AccessFlags & D3D11_CPU_ACCESS_WRITE) != 0;
}

bool D3D11::FTexture2D::IsReadable() const
{
	return false;
}

void D3D11::FTexture2D::ExecCommitShaderResource(void * p)
{
	assert(FRenderContext::Get()->InRenderThread());
	FRenderContext::Get()->CommitShaderResource((FTexture2D*)p);
}
