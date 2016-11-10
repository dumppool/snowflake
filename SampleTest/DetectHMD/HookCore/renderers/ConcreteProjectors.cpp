/*
 * file ConcreteProjectors.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"
#include "ConcreteProjectors.h"

using namespace lostvr;

TextureProjector0::TextureProjector0() : Renderer(nullptr), SRV(nullptr)
{
}

TextureProjector0::~TextureProjector0()
{
	DestroyRHI();
}

bool TextureProjector0::IsInitialized(IDXGISwapChain * swapChain) const
{
	return (Renderer != nullptr && Renderer->GetSwapChain() == swapChain);
}

bool TextureProjector0::InitializeRenderer(IDXGISwapChain * swapChain)
{
	Renderer = new Direct3D11Helper(EDirect3D::DeviceRef);
	return Renderer->UpdateRHIWithSwapChain(swapChain);
}

bool TextureProjector0::InitializeTextureSRV()
{
	if (Renderer == nullptr)
	{
		return false;
	}

	return SUCCEEDED(Renderer->CreateShaderResourceViewBySwapChain(nullptr, (void**)&SRV));
}

Direct3D11Helper * lostvr::TextureProjector0::GetRenderer()
{
	return Renderer;
}

ID3D11ShaderResourceView* lostvr::TextureProjector0::GetTextureSRV()
{
	return SRV;
}

void TextureProjector0::DestroyRHI()
{
	BaseTextureProjector::DestroyRHI();

	SAFE_DELETE(Renderer);
	SAFE_RELEASE(SRV);
}

TextureProjector1::TextureProjector1() : Renderer(nullptr)
, SRV(nullptr)
, SharedTex_Self(nullptr)
, SharedTex_Target(nullptr)
, SwapChainRef_Target(nullptr)
{
}

TextureProjector1::~TextureProjector1()
{
	DestroyRHI();
}

bool TextureProjector1::IsInitialized(IDXGISwapChain* swapChain) const
{
	return (Renderer != nullptr && SwapChainRef_Target == swapChain);
}

bool TextureProjector1::InitializeRenderer(IDXGISwapChain* swapChain)
{
	const CHAR* head = "TextureProjector1::InitializeRenderer";

	HRESULT hr = S_FALSE;

	SAFE_RELEASE(SwapChainRef_Target);
	SwapChainRef_Target = swapChain;
	SwapChainRef_Target->AddRef();

	SAFE_DELETE(Renderer);
	DXGI_SWAP_CHAIN_DESC scDesc;
	SwapChainRef_Target->GetDesc(&scDesc);
	Renderer = new Direct3D11Helper(EDirect3D::DXGI1, scDesc.BufferDesc.Width, scDesc.BufferDesc.Height, scDesc.BufferDesc.Format);

	SAFE_RELEASE(SharedTex_Target);
	D3D11_TEXTURE2D_DESC sharedDesc = { 0 };
	Renderer->GetDescriptionTemplate_DefaultTexture2D(sharedDesc);
	sharedDesc.Format = scDesc.BufferDesc.Format;
	sharedDesc.Width = scDesc.BufferDesc.Width;
	sharedDesc.Height = scDesc.BufferDesc.Height;
	sharedDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED/*_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX*/;

	ID3D11Device* device = nullptr;
	if (SUCCEEDED(SwapChainRef_Target->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
	{
		if (Renderer->CreateTexture2D(&SharedTex_Target, nullptr, &sharedDesc, device))
		{
			LVMSG(head, "create shared texture succeeded");
		}
		else
		{
			LVMSG(head, "create shared texture failed");
		}
	}

	SAFE_RELEASE(device);

	SAFE_RELEASE(SharedTex_Self);
	IDXGIResource* res = nullptr;
	if (SUCCEEDED(SharedTex_Target->QueryInterface(__uuidof(IDXGIResource), (void**)&res)))
	{
		HANDLE hShared;
		if (SUCCEEDED(res->GetSharedHandle(&hShared)) &&
			SUCCEEDED(Renderer->GetDevice()->OpenSharedResource(hShared, __uuidof(ID3D11Texture2D), (void**)&SharedTex_Self)))
		{
			LVMSG(head, "map shared resource succeeded");
		}
		else
		{
			LVMSG(head, "map shared resource failed");
		}
	}

	SAFE_RELEASE(res);
	return SharedTex_Target != nullptr && SharedTex_Self != nullptr;
}

Direct3D11Helper* TextureProjector1::GetRenderer()
{
	return Renderer;
}

bool TextureProjector1::InitializeTextureSRV()
{
	const CHAR* head = "TextureProjector1::InitializeTextureSRV";
	HRESULT hr;

	if (Renderer == nullptr)
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scDesc;
	SwapChainRef_Target->GetDesc(&scDesc);

	D3D11_TEXTURE2D_DESC desc = { 0 };
	Renderer->GetDescriptionTemplate_DefaultTexture2D(desc);
	desc.Format = scDesc.BufferDesc.Format;
	desc.Width = scDesc.BufferDesc.Width;
	desc.Height = scDesc.BufferDesc.Height;

	SAFE_RELEASE(SRV);
	return Renderer->CreateShaderResourceView(SharedTex_Self, &SRV);
}

ID3D11ShaderResourceView* TextureProjector1::GetTextureSRV()
{
	return SRV;
}

void TextureProjector1::DestroyRHI()
{
	BaseTextureProjector::DestroyRHI();

	SAFE_DELETE(Renderer);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(SharedTex_Self);
	SAFE_RELEASE(SharedTex_Target);
	SAFE_RELEASE(SwapChainRef_Target);
}

bool TextureProjector1::PrepareSRV()
{
	const CHAR* head = "TextureProjector1::PrepareSRV";
	if (SwapChainRef_Target == nullptr || Renderer == nullptr)
	{
		return false;
	}

	bool bSuccess = true;

	ID3D11Texture2D* srcTex = nullptr;
	IDXGIResource* src = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	if (SUCCEEDED(SwapChainRef_Target->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
	{
		device->GetImmediateContext(&context);
		if (context != nullptr && SUCCEEDED(SwapChainRef_Target->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&srcTex)))
		{
			ContextCopyResource(context, SharedTex_Target, srcTex, "source context");
		}
	}

	SAFE_RELEASE(context);
	SAFE_RELEASE(device);
	SAFE_RELEASE(srcTex);
	SAFE_RELEASE(src);
	return bSuccess;
}

TextureProjector9::TextureProjector9() : Renderer(nullptr)
, DeviceRef_Target(nullptr)
, SRV(nullptr)
{
}

TextureProjector9::~TextureProjector9()
{
	DestroyRHI();
}

bool TextureProjector9::IsInitialized_Direct3D9(IDirect3DDevice9 * device)
{
	return (Renderer != nullptr && DeviceRef_Target == device);
}

bool TextureProjector9::InitializeRenderer_Direct3D9(IDirect3DDevice9 * device)
{
	const CHAR* head = "TextureProjector9::InitializeRenderer_Direct3D9";
	SAFE_DELETE(Renderer);
	Renderer = new Direct3D11Helper(EDirect3D::DXGI1, device);
	if (Renderer == nullptr)
	{
		LVMSG(head, "alloc renderer failed");
		return false;
	}

	SAFE_RELEASE(DeviceRef_Target);
	DeviceRef_Target = device;
	DeviceRef_Target->AddRef();

	return true;
}

bool TextureProjector9::InitializeTextureSRV()
{
	IDirect3DSurface9* surface = nullptr;
	DeviceRef_Target->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);

	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);

	D3D11_TEXTURE2D_DESC td;
	Renderer->GetDescriptionTemplate_DefaultTexture2D(td);
	td.Width = surfaceDesc.Width;
	td.Height = surfaceDesc.Height;
	td.Format = Renderer->GetDirect9FormatMatch(surfaceDesc.Format);

	SAFE_RELEASE(Tex);
	SAFE_RELEASE(SRV);
	Renderer->CreateTexture2D(&Tex, &SRV, &td);

	return SRV != nullptr;
}

void TextureProjector9::DestroyRHI()
{
	BaseTextureProjector_Direct3D9::DestroyRHI();

	SAFE_DELETE(Renderer);
	SAFE_RELEASE(DeviceRef_Target);
	SAFE_RELEASE(SRV);
	SAFE_RELEASE(Tex);
}

bool TextureProjector9::PrepareSRV()
{
	return Renderer->UpdateBuffer_Direct3D9(DeviceRef_Target) &&
		Renderer->OutputBuffer_Texture2D_Direct9(Tex);
}
