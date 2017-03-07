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
#include "RenderContext.h"

using namespace LostCore;

D3D11::FTexture2D::FTexture2D()
	: Texture(nullptr)
	, ShaderResource(nullptr)
	, RenderTarget(nullptr)
	, BindFlags(0x0)
	, AccessFlags(0x0)
{
}

D3D11::FTexture2D::~FTexture2D()
{
	Texture = nullptr;
	ShaderResource = nullptr;
	RenderTarget = nullptr;
	BindFlags = 0x0;
	AccessFlags = 0x0;
}

bool D3D11::FTexture2D::Construct(
	IRenderContext* rc,
	uint32 width, 
	uint32 height, 
	uint32 format, 
	bool bIsDepthStencil, 
	bool bIsRenderTarget, 
	bool bIsShaderResource,
	bool bIsWritable)
{

	const char* head = "D3D11::FTexture2D::Construct";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(rc, head);
	if (!device.IsValid())
	{
		return false;
	}

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

	assert(SUCCEEDED(device->CreateTexture2D(&desc, nullptr, Texture.GetInitReference())));

	if (bIsRenderTarget)
	{
		CD3D11_RENDER_TARGET_VIEW_DESC cdesc(Texture.GetReference(), D3D11_RTV_DIMENSION_TEXTURE2D);
		assert(SUCCEEDED(device->CreateRenderTargetView(Texture.GetReference(), &cdesc, RenderTarget.GetInitReference())));
	}
	else if (bIsDepthStencil)
	{
		CD3D11_DEPTH_STENCIL_VIEW_DESC cdesc(Texture.GetReference(), D3D11_DSV_DIMENSION_TEXTURE2D);
		assert(SUCCEEDED(device->CreateDepthStencilView(Texture.GetReference(), &cdesc, DepthStencil.GetInitReference())));
	}

	if (bIsShaderResource)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC cdesc(Texture.GetReference(), D3D11_SRV_DIMENSION_TEXTURE2D);
		assert(SUCCEEDED(device->CreateShaderResourceView(Texture.GetReference(), &cdesc, ShaderResource.GetInitReference())));
	}

	return true;
}

bool D3D11::FTexture2D::ConstructFromSwapChain(const TRefCountPtr<IDXGISwapChain>& swapChain)
{
	const char* head = "D3D11::FTexture2D::ConstructFromSwapChain";

	HRESULT hr;

	BindFlags = D3D11_BIND_RENDER_TARGET;
	assert(SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)Texture.GetInitReference())));

	TRefCountPtr<ID3D11Device> device;
	assert(SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)device.GetInitReference())));

	CD3D11_RENDER_TARGET_VIEW_DESC cdesc(Texture.GetReference(), D3D11_RTV_DIMENSION_TEXTURE2D);
	assert(SUCCEEDED(device->CreateRenderTargetView(Texture.GetReference(), &cdesc, RenderTarget.GetInitReference())));

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

bool D3D11::FTexture2D::IsWritable() const
{
	return (AccessFlags & D3D11_CPU_ACCESS_WRITE) != 0;
}

bool D3D11::FTexture2D::IsReadable() const
{
	return false;
}
