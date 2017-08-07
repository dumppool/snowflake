/*
* file RenderContextBase.cpp
*
* author luoxw
* date 2017/02/13
*
*
*/

#include "stdafx.h"
#include "RenderContextBase.h"
//#include "BufferDef.h"

using namespace LostCore;

EReturnCode D3D11::CreateDevice(LostCore::EContextID id, 
	TRefCountPtr<ID3D11Device>& ppDevice, TRefCountPtr<ID3D11DeviceContext>& ppContext)
{
	const char* head = "D3D11::CreateDevice";
	const char* typeDesc = LostCore::GetContextDesc(id);

	HRESULT hr;

	TRefCountPtr<IDXGIFactory> factory;
	TRefCountPtr<IDXGIAdapter> adapter;

	if (id == LostCore::EContextID::D3D11_DXGI0)
	{
		hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory.GetInitReference());
	}
	else if (id == LostCore::EContextID::D3D11_DXGI1)
	{
		hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)factory.GetInitReference());
	}
	else
	{
		LVERR(head, "Intent to create unsupported device: %s", typeDesc);
		return SErrorInternalError;
	}

	DXGI_ADAPTER_DESC adapterDesc;
	for (int i = 0;; ++i)
	{
		if (FAILED(factory->EnumAdapters(i, adapter.GetInitReference())))
		{
			LVERR(head, "Enum adapter ended at: %d, type: %s", i, typeDesc);
			break;
		}

		if (FAILED(adapter->GetDesc(&adapterDesc)))
		{
			LVERR(head, "Failed get description for adapter at %d, type: %s", i, typeDesc);
		}

		D3D_FEATURE_LEVEL features[] = { D3D_FEATURE_LEVEL_11_0 };

		UINT createFlags = 0;

#ifdef _DEBUG
		createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		if (SUCCEEDED(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, createFlags, features, 1,
			D3D11_SDK_VERSION, ppDevice.GetInitReference(), nullptr, ppContext.GetInitReference())))
		{
			LVMSG(head, "Create device successfully, enum: %d, adapter: %ls, type: %s", i, &adapterDesc.Description[0], typeDesc);
			break;
		}
		else
		{
			LVERR(head, "Create device failed, enum: %d, adapter: %ls, type: %s", i, &adapterDesc.Description[0], typeDesc);
			return SErrorInternalError;
		}
	}

	return SSuccess;
}

EReturnCode D3D11::CreateSwapChain(
	const TRefCountPtr<ID3D11Device>& d3dDevice,
	HWND wnd, bool bWindowed, UINT width, UINT height,
	TRefCountPtr<IDXGISwapChain>& ppSwapChain)
{
	const char* head = "D3D11::CreateSwapChain";

	if (d3dDevice.GetReference() == nullptr)
	{
		return SErrorInvalidParameters;
	}

	TRefCountPtr<IDXGIFactory> factory;
	TRefCountPtr<IDXGIAdapter> adapter;
	TRefCountPtr<IDXGIDevice> device;

	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = width;
	desc.BufferDesc.Height = height;
	desc.BufferDesc.Format = SSwapChainFormat;
	desc.BufferDesc.RefreshRate.Numerator = 0;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = wnd;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = bWindowed;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;

	hr = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)device.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Get dxgi device failed, hr: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	hr = device->GetParent(__uuidof(IDXGIAdapter), (void**)adapter.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Get dxgi adapter failed, hr: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	hr = adapter->GetParent(__uuidof(IDXGIFactory), (void**)factory.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Get dxgi factory failed, hr: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	hr = factory->CreateSwapChain(device, &desc, ppSwapChain.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Create swapchain failed, hr: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, TRefCountPtr<ID3DBlob>& blob)
{
	const CHAR* head = "D3D11::CompileShader";

	if (!file || !entry || !target || !blob)
	{
		LVERR(head, "Invalid paramter: file(%ls), entry(%s), target(%s), blob(%x)", file, entry, target, blob);
		return SErrorInvalidParameters;
	}

	TRefCountPtr<ID3DBlob> errorBlob;
	UINT Flag1 = 0;
	UINT Flag2 = 0;
	Flag1 |= D3DCOMPILE_DEBUG;
	HRESULT Res = D3DCompileFromFile(file, NULL, NULL, entry, target, Flag1, Flag2,
		blob.GetInitReference(), errorBlob.GetInitReference());
	if (FAILED(Res))
	{
		LVERR(head, "Compile %ls(%s, %s) failed: %s", file, entry, target, errorBlob.IsValid() ? errorBlob->GetBufferPointer() : "");
		return SErrorInternalError;
	}
	else
	{
		LVMSG(head, "Compile %ls(%s, %s) successfully", file, entry, target);
	}

	return SSuccess;
}

EReturnCode D3D11::GetDesc_DefaultTexture2D(D3D11_TEXTURE2D_DESC * pDesc)
{
	if (pDesc == nullptr)
	{
		return SErrorInvalidParameters;
	}

	ZeroMemory(pDesc, sizeof(D3D11_TEXTURE2D_DESC));
	D3D11_TEXTURE2D_DESC& desc = *pDesc;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_B8G8R8A8_TYPELESS;
	desc.Width = 0;
	desc.Height = 0;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	return SSuccess;
}

EReturnCode D3D11::GetDesc_DefaultBuffer(D3D11_BUFFER_DESC * pDesc)
{
	if (pDesc == nullptr)
	{
		return SErrorInvalidParameters;
	}

	ZeroMemory(pDesc, sizeof(D3D11_BUFFER_DESC));
	D3D11_BUFFER_DESC& desc = *pDesc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	return SSuccess;
}

EReturnCode D3D11::CreateTexture2D(D3D11_TEXTURE2D_DESC * pDesc,
	const TRefCountPtr<ID3D11Device>& device,
	TRefCountPtr<ID3D11Texture2D>& ppTex,
	TRefCountPtr<ID3D11ShaderResourceView>& ppSRV,
	TRefCountPtr<ID3D11RenderTargetView>& ppRTV)
{
	const CHAR* head = "D3D11::CreateTexture2D";

	if (device.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInvalidParameters;
	}

	if (pDesc == nullptr)
	{
		LVERR(head, "Null description");
		return SErrorInvalidParameters;
	}

	HRESULT hr = S_FALSE;

	hr = device->CreateTexture2D(pDesc, nullptr, ppTex.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Create texture 2d failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = (*pDesc).Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (*pDesc).MipLevels;

	hr = device->CreateShaderResourceView(ppTex, &srvDesc, ppSRV.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "create shader resource view failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CreateShaderResourceView(
	const TRefCountPtr<ID3D11Texture2D>& tex, 
	TRefCountPtr<ID3D11ShaderResourceView>& srv)
{
	const CHAR* head = "D3D11::CreateShaderResourceView";

	if (tex.GetReference() == nullptr)
	{
		LVERR(head, "null source texture 2d");
		return SErrorInvalidParameters;
	}

	TRefCountPtr<ID3D11Device> dev;
	tex->GetDevice(dev.GetInitReference());
	if (dev.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInternalError;
	}

	D3D11_TEXTURE2D_DESC td;
	tex->GetDesc(&td);

	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = td.Format;
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sd.Texture2D.MipLevels = 1;
	HRESULT hr = (dev->CreateShaderResourceView(tex, &sd, srv.GetInitReference()));
	if (FAILED(hr))
	{
		LVERR(head, "Create shader resource view failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CreateRenderTargetView(
	const TRefCountPtr<ID3D11Texture2D>& tex, 
	TRefCountPtr<ID3D11RenderTargetView>& rtv)
{
	const CHAR* head = "D3D11::CreateRenderTargetView";
	
	if (tex.GetReference() == nullptr)
	{
		LVERR(head, "Null tex input when there is no available internal buffer");
		return SErrorInvalidParameters;
	}

	TRefCountPtr<ID3D11Device> dev;
	tex->GetDevice(dev.GetInitReference());
	if (dev.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInternalError;
	}

	CD3D11_RENDER_TARGET_VIEW_DESC crd(tex, D3D11_RTV_DIMENSION_TEXTURE2D);
	HRESULT hr = dev->CreateRenderTargetView(tex, &crd, rtv.GetInitReference());
	if (FAILED(hr))
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_TEXTURE2D_DESC td;
		tex->GetDesc(&td);
		if (td.Format == DXGI_FORMAT_R8G8B8A8_TYPELESS)
		{
			rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		else if (td.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS)
		{
			td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		}
		else
		{
			LVERR(head, "Unsupported texture format: %d", td.Format);
			return SErrorInternalError;
		}

		hr = dev->CreateRenderTargetView(tex, &rtvd, rtv.GetInitReference());
		if (FAILED(hr))
		{
			LVERR(head, "Create render target view failed: 0x%x(%d)", hr, hr);
			return SErrorInternalError;
		}
	}

	return SSuccess;
}

EReturnCode D3D11::CreatePrimitiveVertex(
	const TRefCountPtr<ID3D11Device>& device, 
	const void* buf, uint32 bytes, bool bDynamic, 
	TRefCountPtr<ID3D11Buffer>& vb)
{
	const CHAR* head = "D3D11::CreatePrimitiveVertex";
	
	if (device.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInvalidParameters;
	}

	if (buf == nullptr || bytes == 0)
	{
		LVERR(head, "Invalid input buf");
		return SErrorInvalidParameters;
	}

	// Vertex Buffer
	D3D11_BUFFER_DESC desc{ bytes, bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, bDynamic?D3D11_CPU_ACCESS_WRITE:0, 0, 0 };
	D3D11_SUBRESOURCE_DATA data{ buf, 0, 0 };
	HRESULT hr = device->CreateBuffer(&desc, &data, vb.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Failed to create vertex buffer: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CreatePrimitiveIndex(
	const TRefCountPtr<ID3D11Device>& device,
	const void* buf, uint32 bytes, bool bDynamic, 
	TRefCountPtr<ID3D11Buffer>& ib)
{
	const CHAR* head = "D3D11::CreatePrimitiveIndex";

	if (device.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInvalidParameters;
	}

	if (buf == nullptr || bytes == 0)
	{
		LVERR(head, "Invalid input buf");
		return SErrorInvalidParameters;
	}

	// Vertex Buffer
	D3D11_BUFFER_DESC desc{ bytes, bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA data{ buf, 0, 0 };
	HRESULT hr = device->CreateBuffer(&desc, &data, ib.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Failed to create vertex buffer: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

//EReturnCode D3D11::CreateMesh_Rect(ID3D11Device* device, float width, float height, UINT vertexSize, ID3D11Buffer ** ppVB, ID3D11Buffer ** ppIB)
//{
//	const CHAR* head = "D3D11::CreateMesh_Rect";
//
//	if (device == nullptr)
//	{
//		LVERR(head, "null device");
//		return SErrorInvalidParameters;
//	}
//
//	HRESULT hr = S_FALSE;
//	float halfw = 0.5f * width;
//	float halfh = 0.5f * height;
//	TRefCountPtr<ID3D11Buffer> vertexBuffer;
//	TRefCountPtr<ID3D11Buffer> indexBuffer;
//
//	// Mesh vertices
//	const MeshVertex rectVertices[4] =
//	{
//		{ FFloat3(-halfw, -halfh, 0.0f),	FFloat2(0.0f, 1.0f) },			// bottom left
//		{ FFloat3(-halfw, halfh, 0.0f),	FFloat2(0.0f, 0.0f) },			// top left
//		{ FFloat3(halfw, halfh, 0.0f),	FFloat2(1.0f, 0.0f) },			// top right
//		{ FFloat3(halfw, -halfh, 0.0f),	FFloat2(1.0f, 1.0f) },			// bottom right
//	};
//
//	// Mesh indices
//	const uint16 rectIndices[6] = { 0, 1, 2, 3, 0, 2 };
//
//	// Vertex Buffer
//	D3D11_BUFFER_DESC vdesc{ vertexSize * ARRAYSIZE(rectVertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
//	D3D11_SUBRESOURCE_DATA data{ &rectVertices[0], 0, 0 };
//	hr = device->CreateBuffer(&vdesc, &data, vertexBuffer.GetInitReference());
//	if (FAILED(hr))
//	{
//		LVERR(head, "failed to create vertex buffer: 0x%x(%d)", hr, hr);
//		return SErrorInternalError;
//	}
//
//	// Index Buffer
//	D3D11_BUFFER_DESC idesc{ sizeof(uint16) * ARRAYSIZE(rectIndices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
//	D3D11_SUBRESOURCE_DATA Data{ &rectIndices[0], 0, 0 };
//	hr = device->CreateBuffer(&idesc, &Data, indexBuffer.GetInitReference());
//	if (FAILED(hr))
//	{
//		LVERR(head, "failed to create index buffer: 0x%x(%d)", hr, hr);
//		return SErrorInternalError;
//	}
//
//	if (ppVB != nullptr)
//	{
//		*ppVB = vertexBuffer.GetReference();
//		vertexBuffer->AddRef();
//	}
//
//	if (ppIB != nullptr)
//	{
//		*ppIB = indexBuffer.GetReference();
//		indexBuffer->AddRef();
//	}
//
//	return SSuccess;
//}

EReturnCode D3D11::CreateBuffer(
	const TRefCountPtr<ID3D11Device>& device,
	D3D11_BUFFER_DESC * pDesc, 
	TRefCountPtr<ID3D11Buffer>& buf)
{
	const CHAR* head = "D3D11::CreateBuffer";

	if (device.GetReference() == nullptr)
	{
		LVERR(head, "null device");
		return SErrorInvalidParameters;
	}

	HRESULT hr = device->CreateBuffer(pDesc, nullptr, buf.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "create buffer failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CreateBlendState_AlphaBlend(
	const TRefCountPtr<ID3D11Device>& device,
	TRefCountPtr<ID3D11BlendState>& bs)
{
	const CHAR* head = "D3D11::CreateBlendState_AlphaBlend";

	if (device.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInvalidParameters;
	}

	HRESULT hr = S_FALSE;

	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.AlphaToCoverageEnable = TRUE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
	if (FAILED(hr = device->CreateBlendState(&bd, bs.GetInitReference())))
	{
		LVERR(head, "Create blend state(add) failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CreateBlendState_Add(
	const TRefCountPtr<ID3D11Device>& device,
	TRefCountPtr<ID3D11BlendState>& bs)
{
	const CHAR* head = "D3D11::CreateBlendState_Add";

	if (device.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInvalidParameters;
	}

	HRESULT hr = S_FALSE;

	D3D11_BLEND_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BLEND_DESC));
	bd.AlphaToCoverageEnable = TRUE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	if (FAILED(hr = device->CreateBlendState(&bd, bs.GetInitReference())))
	{
		LVERR(head, "Create blend state(add) failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}

EReturnCode D3D11::CreateRasterizer(
	const TRefCountPtr<ID3D11Device>& device,
	TRefCountPtr<ID3D11RasterizerState>& rs)
{
	const CHAR* head = "D3D11::GetDefaultRasterizer";

	if (device.GetReference() == nullptr)
	{
		LVERR(head, "Null device");
		return SErrorInvalidParameters;
	}

	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AntialiasedLineEnable = FALSE;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = FALSE;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.ScissorEnable = FALSE;
	desc.SlopeScaledDepthBias = 0.0f;
	HRESULT hr = device->CreateRasterizerState(&desc, rs.GetInitReference());
	if (FAILED(hr))
	{
		LVERR(head, "Create rasterizer failed: 0x%x(%d)", hr, hr);
		return SErrorInternalError;
	}

	return SSuccess;
}
