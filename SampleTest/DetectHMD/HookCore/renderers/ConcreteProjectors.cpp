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
, Tex(nullptr)
, SharedTex(nullptr)
, SwapChainRef(nullptr)
{
}

TextureProjector1::~TextureProjector1()
{
	DestroyRHI();
}

bool TextureProjector1::IsInitialized(IDXGISwapChain* swapChain) const
{
	return (Renderer != nullptr && SwapChainRef == swapChain);
}

bool TextureProjector1::InitializeRenderer(IDXGISwapChain* swapChain)
{
	const CHAR* head = "TextureProjector1::InitializeRenderer";

	HRESULT hr = S_FALSE;

	SAFE_RELEASE(SwapChainRef);
	SwapChainRef = swapChain;
	SwapChainRef->AddRef();

	SAFE_DELETE(Renderer);
	DXGI_SWAP_CHAIN_DESC scDesc;
	SwapChainRef->GetDesc(&scDesc);
	Renderer = new Direct3D11Helper(EDirect3D::DXGI1, scDesc.BufferDesc.Width, scDesc.BufferDesc.Height, scDesc.BufferDesc.Format);

	D3D11_TEXTURE2D_DESC sharedDesc = { 0 };
	sharedDesc.ArraySize = 1;
	sharedDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	sharedDesc.CPUAccessFlags = 0;
	sharedDesc.Format = scDesc.BufferDesc.Format;
	sharedDesc.Width = scDesc.BufferDesc.Width;
	sharedDesc.Height = scDesc.BufferDesc.Height;
	sharedDesc.MipLevels = 1;
	sharedDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED/*_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX*/;
	sharedDesc.SampleDesc.Count = 1;
	sharedDesc.SampleDesc.Quality = 0;
	sharedDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Device* device = nullptr;
	if (SUCCEEDED(SwapChainRef->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
	{
		SAFE_RELEASE(SharedTex);
		hr = device->CreateTexture2D(&sharedDesc, nullptr, &SharedTex);
		if (SUCCEEDED(hr))
		{
			LVMSG(head, "create shared texture succeeded");
		}
		else
		{
			LVMSG(head, "create shared texture failed: 0x%x(%d)", hr, hr);
		}
	}

	return true;
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
	SwapChainRef->GetDesc(&scDesc);

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.Format = scDesc.BufferDesc.Format;
	desc.Width = scDesc.BufferDesc.Width;
	desc.Height = scDesc.BufferDesc.Height;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Device* device = nullptr;

	SAFE_RELEASE(Tex);
	hr = Renderer->GetDevice()->CreateTexture2D(&desc, nullptr, &Tex);
	if (FAILED(hr))
	{
		LVMSG(head, "failed to create texture 2d: 0x%x(%d)", hr, hr);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = scDesc.BufferDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	SAFE_RELEASE(SRV);
	hr = Renderer->GetDevice()->CreateShaderResourceView(Tex, &srvDesc, &SRV);
	if (FAILED(hr))
	{
		LVMSG(head, "failed to create shader resource view: 0x%x(%d)", hr, hr);
		return false;
	}

	return true;
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
	SAFE_RELEASE(Tex);
	SAFE_RELEASE(SharedTex);
	SAFE_RELEASE(SwapChainRef);
}

bool TextureProjector1::PrepareSRV()
{
	const CHAR* head = "TextureProjector1::PrepareSRV";
	if (SwapChainRef == nullptr || Renderer == nullptr)
	{
		return false;
	}

	bool bSuccess = true;

	ID3D11Texture2D* srcTex = nullptr;
	ID3D11Texture2D* tmpTex = nullptr;
	IDXGIResource* src = nullptr;
	IDXGIResource* dst = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	if (SUCCEEDED(SwapChainRef->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
	{
		device->GetImmediateContext(&context);
		if (context != nullptr && SUCCEEDED(SwapChainRef->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&srcTex)))
		{
			//context->CopyResource(SharedTex, srcTex);
			ContextCopyResource(context, SharedTex, srcTex, "source context", true);

			if (SUCCEEDED(SharedTex->QueryInterface(__uuidof(IDXGIResource), (void**)&src)))
			{
				HANDLE hShared;
				if (SUCCEEDED(src->GetSharedHandle(&hShared)) &&
					SUCCEEDED(Renderer->GetDevice()->OpenSharedResource(hShared, __uuidof(IDXGIResource), (void**)&dst)) &&
					SUCCEEDED(dst->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&tmpTex)))
				{
					D3D11_TEXTURE2D_DESC tmpDesc;
					tmpTex->GetDesc(&tmpDesc);
					//LVMSG(head, "tmp width(%d), height(%d), format(%d)", tmpDesc.Width, tmpDesc.Height, tmpDesc.Format);
					//Renderer->GetContext()->CopyResource(Tex, tmpTex);
					ContextCopyResource(Renderer->GetContext(), Tex, tmpTex, "destination context", true);
					bSuccess = true;
				}
			}
		}
	}

	SAFE_RELEASE(context);
	SAFE_RELEASE(device);
	SAFE_RELEASE(srcTex);
	SAFE_RELEASE(tmpTex);
	SAFE_RELEASE(src);
	SAFE_RELEASE(dst);
	return bSuccess;
}
