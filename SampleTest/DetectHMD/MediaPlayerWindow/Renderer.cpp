
#include "stdafx.h"
#include "Renderer.h"

using namespace lostvr;

static const CHAR* SLogPrefix = "Direct3D11Renderer";

const CHAR* GetEDirect3DString(EDirect3D Ver)
{
	if (Ver == EDirect3D::DeviceRef)
	{
		return "Device reference";
	}
	else if (Ver == EDirect3D::DXGI0)
	{
		return "DXGI 1.0";
	}
	else if (Ver == EDirect3D::DXGI1)
	{
		return "DXGI 1.1";
	}
	else if (Ver == EDirect3D::DXGI2)
	{
		return "DXGI 1.2";
	}
	else
	{
		return "unknown graphics interface";
	}
}

Direct3D11Helper::Direct3D11Helper(EDirect3D ver, IDirect3DDevice9 * device) : GIVer(ver)
{
	ZeroRHI();

	if (device == nullptr)
	{
		LVERROR("Direct3D11Helper::Direct3D11Helper", "null device");
		return;
	}

	if (GIVer == EDirect3D::DeviceRef)
	{
		LVERROR("Direct3D11Helper::Direct3D11Helper", "wrong interface version: %s", GetEDirect3DString(GIVer));
		return;
	}

	IDirect3DSurface9* surface = nullptr;
	device->GetRenderTarget(0, &surface);

	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);

	InitializeRHI(surfaceDesc.Width, surfaceDesc.Height, GetDirect9FormatMatch(surfaceDesc.Format));
}

Direct3D11Helper::Direct3D11Helper(EDirect3D ver) : GIVer(ver)
{
	ZeroRHI();

	if (GIVer != EDirect3D::DeviceRef)
	{
		InitializeRHI();
	}
}

Direct3D11Helper::Direct3D11Helper(EDirect3D ver, UINT width, UINT height, DXGI_FORMAT format) : GIVer(ver)
{
	ZeroRHI();

	if (GIVer != EDirect3D::DeviceRef)
	{
		InitializeRHI(width, height, format);
	}
}

bool Direct3D11Helper::UpdateRHIWithDevice(ID3D11Device * device)
{
	const CHAR* head = "Direct3D11Helper::UpdateRHIWithDevice";
	if (device == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	if (Device == device)
	{
		return true;
	}

	DestroyRHI();
	Device = device;
	Device->AddRef();
	Device->GetImmediateContext(&Context);

	return true;
}

bool Direct3D11Helper::UpdateRHIWithSwapChain(IDXGISwapChain* swapChain)
{
	const CHAR* head = "Direct3D11Helper::UpdateRHIWithSwapChain";
	if (swapChain == nullptr)
	{
		LVERROR(head, "null swap chain");
		return false;
	}

	if (SwapChain == swapChain)
	{
		return true;
	}

	DestroyRHI();

	SwapChain = swapChain;
	SwapChain->AddRef();

	LVASSERT(SUCCEEDED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&Buffer)), head, "failed to get buffer from swap chain(0x%x)", SwapChain);
	LVASSERT(SUCCEEDED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Device)), head, "failed to get device from swap chain(0x%x)", SwapChain);

	Device->GetImmediateContext(&Context);
	LVASSERT(Context != nullptr, head, "failed to get context from device(0x%x)", Device);
	return true;
}

bool Direct3D11Helper::UpdateBuffer_Direct3D9(IDirect3DDevice9 * device)
{
	const CHAR* head = "Direct3D11Helper::UpdateBuffer_Direct3D9";

	IDirect3DSurface9* surface = nullptr;
	device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);

	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);

	if (!EnsureDirect9CopyValid(surfaceDesc))
	{
		LVERROR(head, "invalid buffer");
		SAFE_RELEASE(surface);
		return false;
	}

	HRESULT hr = S_FALSE;
	IDirect3DSurface9* lockable = nullptr;
	IDirect3DTexture9* tex = nullptr;
	hr = device->CreateRenderTarget(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, D3DMULTISAMPLE_TYPE(0), 0, 1, &lockable, nullptr);
	if (FAILED(hr))
	{
		LVERROR(head, "create lockable render target failed, 0x%x", hr);
	}
	else
	{
		hr = device->StretchRect(surface, nullptr, lockable, nullptr, D3DTEXF_POINT);
		if (FAILED(hr))
		{
			LVERROR(head, "copy render target surface failed, 0x%x", hr);
		}
		else
		{
			D3DLOCKED_RECT locked = { 0 };
			hr = lockable->LockRect(&locked, nullptr, D3DLOCK_DISCARD);
			if (FAILED(hr))
			{
				LVERROR(head, "lock rect failed, 0x%x", hr);
			}
			else
			{
				D3D11_MAPPED_SUBRESOURCE mapped;
				hr = Context->Map(Buffer_Direct9Copy, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
				if (FAILED(hr))
				{
					LVERROR(head, "map failed: 0x%x", hr);
				}
				else
				{
					D3D11_TEXTURE2D_DESC mappedDesc;
					Buffer_Direct9Copy->GetDesc(&mappedDesc);
					UINT height = min(surfaceDesc.Height, mappedDesc.Height);
					UINT pitch = min(locked.Pitch, mapped.RowPitch);
					LVMSG(head, "ready to copy, height: %d, pitch: %d", height, pitch);
					for (UINT i = 0; i < height; ++i)
					{
						memcpy((CHAR*)mapped.pData + i * pitch, (CHAR*)locked.pBits + i * pitch, pitch);
					}

					Context->Unmap(Buffer_Direct9Copy, 0);

					SAFE_RELEASE(lockable);
					SAFE_RELEASE(tex);
					SAFE_RELEASE(surface);

#ifdef INCLUDE_DIRECTXTK
					static int SCaptureCountDown = 0;
					if ((SCaptureCountDown++ % 100) == 0)
					{
						DirectX::SaveDDSTextureToFile(Context, Buffer_Direct9Copy, L"D:/tank_2016.dds");
					}
#endif

					return true;
				}
			}
		}
	}

	SAFE_RELEASE(lockable);
	SAFE_RELEASE(tex);
	SAFE_RELEASE(surface);
	return false;
}

bool Direct3D11Helper::InitializeRHI(UINT width, UINT height, DXGI_FORMAT format)
{
	const CHAR* head = "Direct3D11Helper::InitializeRHI";

	HRESULT hr;

	IDXGIFactory* factory = nullptr;
	if (GIVer == EDirect3D::DXGI0)
	{
		hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	}
	else if (GIVer == EDirect3D::DXGI1)
	{
		hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
	}
	else
	{
		LVERROR(head, "intent to create unsupport interface, ver: %s", GetEDirect3DString(GIVer));
		return false;
	}

	if (FAILED(hr))
	{
		LVERROR(head, "failed to create factory, result: %d, ver: %s", hr, GetEDirect3DString(GIVer));
		return false;
	}

	HWND hwnd = SGlobalSharedDataInst.GetDefaultWindow();
	IDXGIAdapter* adapter = nullptr;
	DXGI_ADAPTER_DESC adapterDesc;
	for (int i = 0;; ++i)
	{
		if (FAILED(factory->EnumAdapters(i, &adapter)))
		{
			LVERROR(head, "enum adapter ended at: %d, ver: %s", i, GetEDirect3DString(GIVer))
				break;
		}

		if (FAILED(hr = adapter->GetDesc(&adapterDesc)))
		{
			LVERROR(head, "failed to get description for adapter at: %d, result: %d, ver: %s", i, hr, GetEDirect3DString(GIVer));
		}

		IDXGISwapChain* swapChain = nullptr;
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
		//D3D_FEATURE_LEVEL FeatureLevels = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC desc{ 0 };
		desc.BufferCount = 1;
		desc.BufferDesc.Format = format;
		desc.BufferDesc.RefreshRate.Numerator = 0;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferDesc.Width = width;
		desc.BufferDesc.Height = height;
		desc.OutputWindow = hwnd;
		desc.Windowed = TRUE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = 0;

		UINT createFlags = 0;

#ifdef _DEBUG
		createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, createFlags, &featureLevels[0], 1,
			D3D11_SDK_VERSION, &desc, &swapChain, nullptr, nullptr, nullptr)))
		{
			LVMSG(head, "created device successfully, adapter: %ls, ver: %s", &adapterDesc.Description[0], GetEDirect3DString(GIVer));
			UpdateRHIWithSwapChain(swapChain);
			SAFE_RELEASE(swapChain);
			break;
		}
	}

	//return true;

	if (!CreateBlendStates())
	{
		return false;
	}

	return true;
}

bool Direct3D11Helper::OutputBuffer_Texture2D_Direct3D9(ID3D11Texture2D * dst)
{
	const CHAR* head = "Direct3D11Helper::OutputBuffer_Texture2D_Direct9";
	if (Context == nullptr || Buffer_Direct9Copy == nullptr)
	{
		LVERROR(head, "failed, context: 0x%x, buffer: 0x%x", Context, Buffer_Direct9Copy);
		return false;
	}

	D3D11_TEXTURE2D_DESC srcDesc, dstDesc;
	dst->GetDesc(&dstDesc);
	Buffer_Direct9Copy->GetDesc(&srcDesc);
#ifdef _DEBUG
	LVMSG(head, "width(%d), height(%d), format(%d), usage(%d), miplevels(%d), sampler count(%d)", srcDesc.Width, srcDesc.Height, srcDesc.Format, srcDesc.Usage, srcDesc.MipLevels, srcDesc.SampleDesc.Count);
	LVMSG(head, "width(%d), height(%d), format(%d), usage(%d), miplevels(%d), sampler count(%d)", dstDesc.Width, dstDesc.Height, dstDesc.Format, dstDesc.Usage, dstDesc.MipLevels, dstDesc.SampleDesc.Count);
#endif
	Context->CopyResource(dst, Buffer_Direct9Copy);
	return true;
}

IDirect3DDevice9 * Direct3D11Helper::GetTemporaryDirect9Device() const
{
	const CHAR* head = "Direct3D11Helper::GetTemporaryDirect9Device";
	HWND wnd = SGlobalSharedDataInst.GetDefaultWindow();
	//HWND wnd = NULL;

	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == nullptr)
	{
		LVERROR(head, "Direct3DCreate9 failed");
		return nullptr;
	}

	D3DDISPLAYMODE mode;
	d3d->GetAdapterDisplayMode(0, &mode);

	D3DPRESENT_PARAMETERS pp;
	ZeroMemory(&pp, sizeof(D3DPRESENT_PARAMETERS));
	//pp.BackBufferWidth = WinSizeW;
	//pp.BackBufferHeight = WinSizeH;
	pp.BackBufferFormat = mode.Format;
	pp.BackBufferCount = 1;
	pp.SwapEffect = D3DSWAPEFFECT_COPY;
	pp.Windowed = true;
	pp.hDeviceWindow = wnd;

	HRESULT hr;
	IDirect3DDevice9* device = nullptr;
	if (FAILED(hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device)))
	{
		LVERROR("HookDirect3DDevice9Present", "CreateDevice failed, 0x%x", hr);
		return nullptr;
	}

	return device;
}

bool Direct3D11Helper::EnsureDirect9CopyValid(D3DSURFACE_DESC surfaceDesc)
{
	const CHAR* head = "Direct3D11Helper::EnsureDirect9CopyValid";

	D3D11_TEXTURE2D_DESC desc = { 0 };

	if (Buffer_Direct9Copy != nullptr)
	{
		Buffer_Direct9Copy->GetDesc(&desc);
		if (desc.Width == surfaceDesc.Width &&
			desc.Height == surfaceDesc.Height &&
			desc.Format == GetDirect9FormatMatch(surfaceDesc.Format))
		{
			return true;
		}
		else
		{
			LVERROR(head, "the exist buffer is not fit the current direct9 buffer")
				SAFE_RELEASE(Buffer_Direct9Copy);
		}
	}

	if (Device == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	memset(&desc, 0, sizeof(desc));
	desc.Width = surfaceDesc.Width;
	desc.Height = surfaceDesc.Height;
	desc.Format = GetDirect9FormatMatch(surfaceDesc.Format);
	desc.MipLevels = 1;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ArraySize = 1;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = Device->CreateTexture2D(&desc, nullptr, &Buffer_Direct9Copy);
	return SUCCEEDED(hr);
}

DXGI_FORMAT Direct3D11Helper::GetDirect9FormatMatch(D3DFORMAT format)
{
	switch (format)
	{
	case D3DFMT_A8R8G8B8: return DXGI_FORMAT_B8G8R8A8_UNORM;
	case D3DFMT_A8B8G8R8: return DXGI_FORMAT_R8G8B8A8_UNORM;
	case D3DFMT_X8R8G8B8:;
	case D3DFMT_X8B8G8R8: return DXGI_FORMAT_R8G8B8A8_UNORM;
	default: return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

void Direct3D11Helper::GetDescriptionTemplate_DefaultTexture2D(D3D11_TEXTURE2D_DESC & desc)
{
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
}

void Direct3D11Helper::GetDescriptionTemplate_DefaultBuffer(D3D11_BUFFER_DESC & desc)
{
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
}

bool Direct3D11Helper::CreateTexture2D(ID3D11Texture2D ** ppTex, ID3D11ShaderResourceView ** ppSRV, D3D11_TEXTURE2D_DESC * pDesc,
	ID3D11Device* deviceOverride)
{
	const CHAR* head = "Direct3D11Helper::CreateTexture2D";

	ID3D11Device* dev = deviceOverride != nullptr ? deviceOverride : GetDevice();
	if (dev == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	if (ppTex == nullptr && ppSRV == nullptr)
	{
		LVERROR(head, "both texture2d and srv are invalid");
		return false;
	}

	if (pDesc == nullptr)
	{
		LVERROR(head, "null description input");
		return false;
	}

	HRESULT hr = S_FALSE;

	ID3D11Texture2D* tex = nullptr;
	hr = dev->CreateTexture2D(pDesc, nullptr, &tex);
	if (FAILED(hr))
	{
		LVERROR(head, "create texture 2d failed: 0x%x(%d)", hr, hr);
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = (*pDesc).Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (*pDesc).MipLevels;

	ID3D11ShaderResourceView* srv = nullptr;
	hr = dev->CreateShaderResourceView(tex, &srvDesc, &srv);
	if (FAILED(hr))
	{
		LVERROR(head, "create shader resource view failed: 0x%x(%d)", hr, hr);
		SAFE_RELEASE(tex);
		return false;
	}

	if (ppTex == nullptr)
	{
		SAFE_RELEASE(tex);
	}
	else
	{
		*ppTex = tex;
	}

	if (ppSRV == nullptr)
	{
		SAFE_RELEASE(srv);
	}
	else
	{
		*ppSRV = srv;
	}

	return true;
}

bool Direct3D11Helper::CreateShaderResourceView(ID3D11Texture2D * tex, ID3D11ShaderResourceView ** ppSRV)
{
	const CHAR* head = "Direct3D11Helper::CreateShaderResourceView";

	if (tex == nullptr)
	{
		LVERROR(head, "null source texture 2d");
		return false;
	}

	if (ppSRV == nullptr)
	{
		LVERROR(head, "null shader resource view output");
		return false;
	}

	ID3D11Device* dev = nullptr;
	tex->GetDevice(&dev);
	if (dev == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	D3D11_TEXTURE2D_DESC td;
	tex->GetDesc(&td);

	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = td.Format;
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sd.Texture2D.MipLevels = 1;
	HRESULT hr = (dev->CreateShaderResourceView(tex, &sd, ppSRV));
	if (FAILED(hr))
	{
		LVERROR(head, "create shader resource view failed: 0x%x(%d)", hr, hr);
		return false;
	}

	return true;
}

bool Direct3D11Helper::CreateRenderTargetView(ID3D11Texture2D * tex, ID3D11RenderTargetView ** rtv)
{
	const CHAR* head = "Direct3D11Helper::CreateRenderTargetView";

	if (rtv == nullptr)
	{
		LVERROR(head, "null rtv");
		return false;
	}

	if (tex == nullptr)
	{
		if (Buffer_Direct9Copy != nullptr)
		{
			tex = Buffer_Direct9Copy;
		}
		else if (Buffer != nullptr)
		{
			tex = Buffer;
		}
		else
		{
			LVERROR(head, "null tex input when there is no available internal buffer");
			return false;
		}
	}

	ID3D11Device* dev = nullptr;
	tex->GetDevice(&dev);
	if (dev == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	bool bSuccess = true;
	CD3D11_RENDER_TARGET_VIEW_DESC crd(tex, D3D11_RTV_DIMENSION_TEXTURE2D);
	HRESULT hr = dev->CreateRenderTargetView(tex, &crd, rtv);
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

		hr = dev->CreateRenderTargetView(tex, &rtvd, rtv);
		if (FAILED(hr))
		{
			LVERROR(head, "create render target view failed: 0x%x(%d)", hr, hr);
			bSuccess = false;
		}
	}

	SAFE_RELEASE(dev);
	return bSuccess;
}

void Direct3D11Helper::UpdateCursor(const LVVec3& areaSize, float scale, const LVMatrix& matView, const LVMatrix& matProj, bool bVisible)
{
	if (!bVisible)
	{
		return;
	}

	const CHAR* head = "Direct3D11Helper::UpdateCursor";

	if (Device == nullptr)
	{
		LVERROR(head, "null device");
		return;
	}

	if (Context == nullptr)
	{
		LVERROR(head, "null context");
		return;
	}

	// create if any requirement has not been prepared
	if (CursorVB == nullptr || CursorIB == nullptr || CursorCB == nullptr || CursorPS == nullptr)
	{
		SAFE_RELEASE(CursorVB);
		SAFE_RELEASE(CursorIB);
		SAFE_RELEASE(CursorCB);
		SAFE_RELEASE(CursorPS);
		if (!CreateMesh_Rect(1.f, 1.f, sizeof(MeshVertex), &CursorVB, &CursorIB))
		{
			LVERROR(head, "create mesh for cursor failed");
			return;
		}

		D3D11_BUFFER_DESC bdesc;
		GetDescriptionTemplate_DefaultBuffer(bdesc);
		bdesc.ByteWidth = sizeof(FrameBufferWVP);
		if (!CreateBuffer(bdesc, &CursorCB))
		{
			LVERROR(head, "create mesh for cursor failed");
			return;
		}

		ID3DBlob* psCode = nullptr;
		if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath(), "ps_main_cursor", "ps_5_0", &psCode) ||
			FAILED(Device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, &CursorPS)))
		{
			LVERROR(head, "create pixel shader for cursor failed");
			return;
		}

		// Make sure default variables available
		if (DefaultInputLayout == nullptr || DefaultRasterizer == nullptr)
		{
			GetDefaultShader(nullptr, nullptr, nullptr);
			GetDefaultRasterizer(nullptr);
		}

		if (BlendState_Add_RGB == nullptr || BlendState_Blend_RGB == nullptr)
		{
			if (!CreateBlendStates())
			{
				return;
			}
		}
	}

	// update cursor position
	HWND wnd = SGlobalSharedDataInst.GetTargetWindow();
	POINT pt;
	RECT wndRect;
	if (FALSE == GetCursorPos(&pt) ||
		FALSE == GetWindowRect(wnd, &wndRect) ||
		0 == (wndRect.right - wndRect.left) ||
		0 == (wndRect.bottom - wndRect.top))
	{
		LVERROR(head, "get cursor pos failed");
	}
	else
	{
		float rw = float(wndRect.right - wndRect.left);
		float rh = float(wndRect.bottom - wndRect.top);
		float ptx = float(pt.x - wndRect.left) / rw - 0.5f;
		float pty = 0.5f - float(pt.y - wndRect.top) / rh;

		LVMatrix matTrans2 = DirectX::XMMatrixTranslation(ptx * areaSize.x, pty * areaSize.y, areaSize.z);
		LVMatrix matScale2 = DirectX::XMMatrixScaling(scale, scale, scale);

		uint32 stride = sizeof(MeshVertex);
		uint32 offset = 0;
		Context->IASetVertexBuffers(0, 1, &CursorVB, &stride, &offset);
		Context->IASetIndexBuffer(CursorIB, DXGI_FORMAT_R16_UINT, 0);
		Context->IASetInputLayout(DefaultInputLayout);

		FrameBufferWVP wvp(DirectX::XMMatrixTranspose(matScale2 * matTrans2), matView, matProj);
		Context->UpdateSubresource(CursorCB, 0, nullptr, &wvp, 0, 0);
		Context->VSSetConstantBuffers(0, 1, &CursorCB);
		Context->VSSetShader(DefaultVS, nullptr, 0);
		Context->PSSetShader(CursorPS, nullptr, 0);
		Context->PSSetSamplers(0, 0, nullptr);
		Context->RSSetState(DefaultRasterizer);

		Context->OMSetBlendState(BlendState_Add_RGB, 0, 0xffffffff);

		Context->DrawIndexed(6, 0, 0);
	}
}

bool Direct3D11Helper::CreateMesh_Rect(float width, float height, UINT vertexSizeInBytes, ID3D11Buffer ** vb, ID3D11Buffer ** ib)
{
	const CHAR* head = "Direct3D11Helper::CreateMesh";

	if (Device == nullptr)
	{
		return false;
	}

	HRESULT hr = S_FALSE;
	float halfw = 0.5f * width;
	float halfh = 0.5f * height;
	ID3D11Buffer *vertexBuffer = nullptr;
	ID3D11Buffer *indexBuffer = nullptr;

	// Mesh vertices
	const MeshVertex rectVertices[4] =
	{
		{ LVVec3(-halfw, -halfh, 0.0f),	LVVec2(0.0f, 1.0f) },			// bottom left
		{ LVVec3(-halfw, halfh, 0.0f),	LVVec2(0.0f, 0.0f) },			// top left
		{ LVVec3(halfw, halfh, 0.0f),	LVVec2(1.0f, 0.0f) },			// top right
		{ LVVec3(halfw, -halfh, 0.0f),	LVVec2(1.0f, 1.0f) },			// bottom right
	};

	// Mesh indices
	const uint16 rectIndices[6] = { 0, 1, 2, 3, 0, 2 };

	// Vertex Buffer
	D3D11_BUFFER_DESC vdesc{ vertexSizeInBytes * ARRAYSIZE(rectVertices), D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA data{ &rectVertices[0], 0, 0 };
	hr = Device->CreateBuffer(&vdesc, &data, &vertexBuffer);
	if (FAILED(hr))
	{
		LVERROR(head, "failed to create vertex buffer: 0x%x(%d)", hr, hr);
		return false;
	}

	// Index Buffer
	D3D11_BUFFER_DESC idesc{ sizeof(uint16) * ARRAYSIZE(rectIndices), D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
	D3D11_SUBRESOURCE_DATA Data{ &rectIndices[0], 0, 0 };
	hr = Device->CreateBuffer(&idesc, &Data, &indexBuffer);
	VERIFY_HRESULT(hr, return false, "failed to create rect index buffer.");
	if (FAILED(hr))
	{
		SAFE_RELEASE(vertexBuffer);
		LVERROR(head, "failed to create index buffer: 0x%x(%d)", hr, hr);
		return false;
	}

	if (vb != nullptr)
	{
		*vb = vertexBuffer;
	}
	else
	{
		SAFE_RELEASE(vertexBuffer);
	}

	if (ib != nullptr)
	{
		*ib = indexBuffer;
	}
	else
	{
		SAFE_RELEASE(indexBuffer);
	}

	return true;
}

bool Direct3D11Helper::CreateBuffer(D3D11_BUFFER_DESC & desc, ID3D11Buffer ** ppBuffer)
{
	const CHAR* head = "Direct3D11Helper::CreateBuffer";
	if (Device == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	if (ppBuffer == nullptr)
	{
		return false;
	}

	ID3D11Buffer* buf = nullptr;
	HRESULT hr = Device->CreateBuffer(&desc, nullptr, &buf);
	if (FAILED(hr))
	{
		LVERROR(head, "create buffer failed: 0x%x(%d)", hr, hr);
		return false;
	}

	*ppBuffer = buf;
	return true;
}

bool Direct3D11Helper::GetDefaultShader(ID3D11VertexShader ** vs, ID3D11PixelShader ** ps, ID3D11InputLayout** layout)
{
	const CHAR* head = "Direct3D11Helper::GetDefaultShader";

	if (Device == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	if (DefaultVS != nullptr && DefaultPS != nullptr && DefaultInputLayout != nullptr)
	{
	}
	else
	{
		SAFE_RELEASE(DefaultVS);
		SAFE_RELEASE(DefaultPS);
		SAFE_RELEASE(DefaultInputLayout);

		HRESULT hr = S_OK;
		ID3DBlob* vsCode = nullptr;
		if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath(), "vs_main", "vs_5_0", &vsCode) || !vsCode ||
			FAILED(hr = Device->CreateVertexShader(vsCode->GetBufferPointer(), vsCode->GetBufferSize(), nullptr, &DefaultVS)))
		{
			SAFE_RELEASE(vsCode);
			LVERROR(head, "create(%ls) vertex shader failed: 0x%x(%d)", SGlobalSharedDataInst.GetShaderFilePath(), hr, hr);
			return false;
		}

		ID3DBlob* psCode = nullptr;
		if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath(), "ps_main", "ps_5_0", &psCode) || !psCode ||
			FAILED(hr = Device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, &DefaultPS)))
		{
			SAFE_RELEASE(vsCode);
			SAFE_RELEASE(psCode);
			LVERROR(head, "create(%ls) pixel shader failed: 0x%x(%d)", SGlobalSharedDataInst.GetShaderFilePath(), hr, hr);
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, offsetof(MeshVertex, Texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if (FAILED(Device->CreateInputLayout(desc, ARRAYSIZE(desc), vsCode->GetBufferPointer(), vsCode->GetBufferSize(), &DefaultInputLayout)))
		{
			SAFE_RELEASE(vsCode);
			SAFE_RELEASE(psCode);
			LVERROR(head, "create(%ls) input layout failed: 0x%x(%d)", SGlobalSharedDataInst.GetShaderFilePath(), hr, hr);
			return false;
		}

		SAFE_RELEASE(vsCode);
		SAFE_RELEASE(psCode);
	}

	if (vs != nullptr)
	{
		*vs = DefaultVS;
	}

	if (ps != nullptr)
	{
		*ps = DefaultPS;
	}

	if (layout != nullptr)
	{
		*layout = DefaultInputLayout;
	}

	return true;
}

bool Direct3D11Helper::GetDefaultShader_MediaPlayer(ID3D11VertexShader ** vs, ID3D11PixelShader ** ps, ID3D11InputLayout ** layout)
{
	const CHAR* head = "Direct3D11Helper::GetDefaultShader_MediaPlayer";

	if (Device == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	HRESULT hr = S_OK;
	ID3DBlob* vsCode = nullptr;
	if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath_MediaPlayer(), "vs_main", "vs_5_0", &vsCode) || !vsCode ||
		FAILED(hr = Device->CreateVertexShader(vsCode->GetBufferPointer(), vsCode->GetBufferSize(), nullptr, vs)))
	{
		SAFE_RELEASE(vsCode);
		LVERROR(head, "create(%ls) vertex shader failed: 0x%x(%d)", SGlobalSharedDataInst.GetShaderFilePath_MediaPlayer(), hr, hr);
		return false;
	}

	ID3DBlob* psCode = nullptr;
	if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath_MediaPlayer(), "ps_main", "ps_5_0", &psCode) || !psCode ||
		FAILED(hr = Device->CreatePixelShader(psCode->GetBufferPointer(), psCode->GetBufferSize(), nullptr, ps)))
	{
		SAFE_RELEASE(vsCode);
		SAFE_RELEASE(psCode);
		LVERROR(head, "create(%ls) pixel shader failed: 0x%x(%d)", SGlobalSharedDataInst.GetShaderFilePath_MediaPlayer(), hr, hr);
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC desc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, offsetof(MeshVertex, Texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (FAILED(Device->CreateInputLayout(desc, ARRAYSIZE(desc), vsCode->GetBufferPointer(), vsCode->GetBufferSize(), layout)))
	{
		SAFE_RELEASE(vsCode);
		SAFE_RELEASE(psCode);
		LVERROR(head, "create(%ls) input layout failed: 0x%x(%d)", SGlobalSharedDataInst.GetShaderFilePath_MediaPlayer(), hr, hr);
		return false;
	}

	SAFE_RELEASE(vsCode);
	SAFE_RELEASE(psCode);
	return true;
}

bool Direct3D11Helper::CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob ** blob)
{
	const CHAR* head = "Direct3D11Helper::CompileShader";

	if (!file || !entry || !target || !blob)
	{
		LVERROR(head, "invalid paramter: file(%ls), entry(%s), target(%s), blob(%x)", file, entry, target, blob);
		return false;
	}

	ID3DBlob* ShaderBlob = nullptr;
	ID3DBlob* ErrorBlob = nullptr;
	UINT Flag1 = 0;
	UINT Flag2 = 0;
	Flag1 |= D3DCOMPILE_DEBUG;
	HRESULT Res = D3DCompileFromFile(file, NULL, NULL, entry, target, Flag1, Flag2, &ShaderBlob, &ErrorBlob);
	if (FAILED(Res))
	{
		LVERROR(head, "compile %ls(%s, %s) failed: %s", file, entry, target, ErrorBlob ? ErrorBlob->GetBufferPointer() : "");
		SAFE_RELEASE(ShaderBlob);
		SAFE_RELEASE(ErrorBlob);
		return false;
	}
	else
	{
		LVMSG(head, "compile %ls(%s, %s) successfully", file, entry, target);
		if (blob == nullptr)
		{
			SAFE_RELEASE(ShaderBlob);
		}
		else
		{
			*blob = ShaderBlob;
		}

		return true;
	}
}

bool Direct3D11Helper::GetDefaultRasterizer(ID3D11RasterizerState ** rs)
{
	const CHAR* head = "Direct3D11Helper::GetDefaultRasterizer";

	if (Device == nullptr)
	{
		LVERROR(head, "null device");
		return false;
	}

	if (DefaultRasterizer == nullptr)
	{
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
		HRESULT hr = Device->CreateRasterizerState(&desc, &DefaultRasterizer);
		if (FAILED(hr))
		{
			LVERROR(head, "create rasterizer failed: 0x%x(%d)", hr, hr);
			return false;
		}
	}

	if (rs != nullptr)
	{
		*rs = DefaultRasterizer;
		DefaultRasterizer->AddRef();
	}

	return true;
}

bool Direct3D11Helper::CreateBlendStates()
{
	const CHAR* head = "Direct3D11Helper::CreateBlendStates";

	HRESULT hr = S_FALSE;

	// Create default blend states
	SAFE_RELEASE(BlendState_Add_RGB);
	SAFE_RELEASE(BlendState_Blend_RGB);

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
	if (FAILED(hr = Device->CreateBlendState(&bd, &BlendState_Add_RGB)))
	{
		LVERROR(head, "create blend state(add) failed: 0x%x(%d)", hr, hr);
		return false;
	}

	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_DEST_ALPHA;
	if (FAILED(hr = Device->CreateBlendState(&bd, &BlendState_Blend_RGB)))
	{
		LVERROR(head, "create blend state(blend) failed: 0x%x(%d)", hr, hr);
		return false;
	}

	return true;
}

ID3D11Device* Direct3D11Helper::GetDevice()
{
	return Device;
}

ID3D11DeviceContext* Direct3D11Helper::GetContext()
{
	return Context;
}

IDXGISwapChain* Direct3D11Helper::GetSwapChain()
{
	return SwapChain;
}

ID3D11Texture2D * Direct3D11Helper::GetSwapChainBuffer()
{
	return Buffer;
}

ID3D11Texture2D * Direct3D11Helper::GetBufferDirect3D9Copy()
{
	return Buffer_Direct9Copy;
}

bool Direct3D11Helper::GetSwapChainData(UINT & width, UINT & height, DXGI_FORMAT & format)
{
	if (SwapChain != nullptr)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		SwapChain->GetDesc(&desc);
		width = desc.BufferDesc.Width;
		height = desc.BufferDesc.Height;
		format = desc.BufferDesc.Format;
		return true;
	}
	else
	{
		return false;
	}
}

EDirect3D Direct3D11Helper::GetGraphicsInterfaceVersion()
{
	return GIVer;
}

bool lostvr::GetNextGIVersion(EDirect3D ver, EDirect3D& nextVer)
{
	nextVer = EDirect3D(((int)ver + 1) % (int)EDirect3D::Num);
	switch (ver)
	{
	case EDirect3D::Undefined:
	case EDirect3D::DeviceRef:
	case EDirect3D::DXGI2:
	case EDirect3D::DXGI3:
		return false;
	}

	return true;
}

const CHAR* lostvr::GetEDirect3DString(EDirect3D Ver)
{
	if (Ver == EDirect3D::DeviceRef)
	{
		return "Device reference";
	}
	else if (Ver == EDirect3D::DXGI0)
	{
		return "DXGI 1.0";
	}
	else if (Ver == EDirect3D::DXGI1)
	{
		return "DXGI 1.1";
	}
	else if (Ver == EDirect3D::DXGI2)
	{
		return "DXGI 1.2";
	}
	else
	{
		return "unknown graphics interface";
	}
}

lostvr::EDirect3D lostvr::GetInterfaceVersionFromSwapChain(IDXGISwapChain* swapChain)
{
	EDirect3D ret = EDirect3D::Undefined;
	if (swapChain == nullptr)
	{
		return ret;
	}

	ID3D11Device* device = nullptr;
	if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
	{
		ret = EDirect3D::DXGI0;

		IDXGIDevice* dxgiDevice = nullptr;
		IDXGIDevice1* dxgiDevice1 = nullptr;
		IDXGIFactory* dxgiFactory = nullptr;
		IDXGIFactory1* dxgiFactory1 = nullptr;
		/*			if (SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice3), (void**)&dxgiDevice)) && dxgiDevice != nullptr)
		{
		ret = EDirect3D::DXGI3;
		}
		else if (SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice2), (void**)&dxgiDevice)) && dxgiDevice != nullptr)
		{
		ret = EDirect3D::DXGI2;
		}
		else */
		if (SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)) && dxgiDevice != nullptr)
		{
			IDXGIAdapter* dxgiAdapter = nullptr;
			IDXGIAdapter1* dxgiAdapter1 = nullptr;
			if (SUCCEEDED(dxgiDevice->GetAdapter(&dxgiAdapter)))
			{
				if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter1), (void**)&dxgiAdapter1)) && dxgiAdapter1 != nullptr)
				{
					ret = EDirect3D::DXGI1;
					DXGI_ADAPTER_DESC1 desc1;
					dxgiAdapter1->GetDesc1(&desc1);
				}
			}
		}
	}

	return ret;
}

std::string lostvr::GetDescriptionFromSwapChain(IDXGISwapChain * swapChain)
{
	std::string ret;
	ret.reserve(1024);
	if (swapChain == nullptr)
	{
		return ret;
	}

	DXGI_SWAP_CHAIN_DESC scDesc;
	if (SUCCEEDED(swapChain->GetDesc(&scDesc)))
	{
		CHAR buf[1024];
		snprintf(buf, 1023, "swap chain width:\t%d \n\tswap chain height:\t%d \n\tswap chain format:\t%d\n\tswap chain flag:\t0x%x ",
			scDesc.BufferDesc.Width, scDesc.BufferDesc.Height, scDesc.BufferDesc.Format, scDesc.Flags);

		ret += "\n\t";
		ret += buf;
	}

	ID3D11Device* device = nullptr;
	IDXGIDevice* dxgiDevice = nullptr;
	D3D_FEATURE_LEVEL feature;
	if (SUCCEEDED(swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device)) &&
		SUCCEEDED(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
	{
		feature = device->GetFeatureLevel();
		CHAR buf[1024];
		snprintf(&buf[0], 1023, "feature level:\t\t0x%x", feature);

		ret += "\n\t";
		ret += buf;

		IDXGIAdapter* dxgiAdapter = nullptr;
		if (SUCCEEDED(dxgiDevice->GetAdapter(&dxgiAdapter)))
		{
			DXGI_ADAPTER_DESC desc;
			if (SUCCEEDED(dxgiAdapter->GetDesc(&desc)))
			{
				CHAR buf[1024];
				snprintf(&buf[0], 1023, "adapter:\t\t%d-%d\n\tadapter description:\t%ls\n\tadapter vendor id:\t%d\n\tadapter device id:\t%d",
					desc.AdapterLuid.HighPart, desc.AdapterLuid.LowPart, desc.Description, desc.VendorId, desc.DeviceId);

				ret += "\n\t";
				ret += buf;

				snprintf(&buf[0], 1023, "adapter subsytem id:\t%d\n\tadapter revision:\t%d\n\tadapter video memory:\t%Idmb\n\tadapter system memory:\t%Idmb\n\tadapter shared memory:\t%Idmb",
					desc.SubSysId, desc.Revision, desc.DedicatedVideoMemory / (1024 * 1024), desc.DedicatedSystemMemory / (1024 * 1024), desc.SharedSystemMemory / (1024 * 1024));

				ret += "\n\t";
				ret += buf;
			}

			SAFE_RELEASE(dxgiAdapter);
		}

	}

	SAFE_RELEASE(device);
	SAFE_RELEASE(dxgiDevice);
	return ret;
}

std::string lostvr::GetDescriptionFromDevice(IDirect3DDevice9 * device)
{
	IDirect3DDevice9Ex* deviceEx = nullptr;
	if (SUCCEEDED(device->QueryInterface(__uuidof(IDirect3DDevice9Ex), (void**)&deviceEx)) && deviceEx != nullptr)
	{
		SAFE_RELEASE(deviceEx);
		return "IDirect3DDevice9Ex";
	}
	else
	{
		return "IDirect3DDevice9";
	}
}

void lostvr::ContextCopyResource(ID3D11Texture2D * dst, ID3D11Texture2D * src, const CHAR* msgHead, bool bCheckContext)
{
	const CHAR* head = "ContextCopyResource";
	if (dst == nullptr || src == nullptr)
	{
		return;
	}

	ID3D11Device* device = nullptr;
	src->GetDevice(&device);
	if (device == nullptr)
	{
		return;
	}

	ID3D11DeviceContext* context = nullptr;
	device->GetImmediateContext(&context);
	if (context == nullptr)
	{
		SAFE_RELEASE(device);
		return;
	}

	if (bCheckContext)
	{
		ID3D11Device *dstDevice;
		dst->GetDevice(&dstDevice);

		if (dstDevice != device)
		{
			LVERROR(head, "intent to copy between different device");
			SAFE_RELEASE(device);
			SAFE_RELEASE(dstDevice);
			SAFE_RELEASE(context);
			return;
		}

		LVMSG(head, "%s, 0x%x, context(0x%x), flag(0x%x), feature(0x%x)", msgHead, device, context, device->GetCreationFlags(), device->GetFeatureLevel());

		D3D11_TEXTURE2D_DESC dstDesc, srcDesc;
		dst->GetDesc(&dstDesc);
		src->GetDesc(&srcDesc);

		//context->ResolveSubresource(dst, 0, src, 0, dstDesc.Format);

		LVMSG(head, "%s, dst width(%d), height(%d), format(%d)", msgHead, dstDesc.Width, dstDesc.Height, dstDesc.Format);
		LVMSG(head, "%s, src width(%d), height(%d), format(%d)", msgHead, srcDesc.Width, srcDesc.Height, srcDesc.Format);

		SAFE_RELEASE(dstDevice);
	}

	context->CopyResource(dst, src);

	SAFE_RELEASE(device);
	SAFE_RELEASE(context);
}

RenderStateCache::RenderStateCache(ID3D11DeviceContext * context) : ContextRef(context)
, NumViewports(1)
, RS(nullptr)
, CB(nullptr)
, VB(nullptr)
, IB(nullptr)
, InputLayout(nullptr)
, RTV(nullptr)
, DSV(nullptr)
, VS(nullptr)
, PS(nullptr)
{
	ContextRef->AddRef();
}

RenderStateCache::~RenderStateCache()
{
	SAFE_RELEASE(RS);
	SAFE_RELEASE(CB);
	SAFE_RELEASE(VB);
	SAFE_RELEASE(IB);
	SAFE_RELEASE(InputLayout);
	SAFE_RELEASE(RTV);
	SAFE_RELEASE(DSV);
	SAFE_RELEASE(VS);
	SAFE_RELEASE(PS);

	SAFE_RELEASE(ContextRef);
}

void RenderStateCache::Capture()
{
	if (ContextRef == nullptr)
	{
		return;
	}

	ContextRef->RSGetViewports(&NumViewports, &Viewport);
	ContextRef->RSGetState(&RS);
	ContextRef->VSGetConstantBuffers(0, 1, &CB);
	ContextRef->IAGetVertexBuffers(0, 1, &VB, &VBStride, &VBOffset);
	ContextRef->IAGetIndexBuffer(&IB, &IBFormat, &IBOffset);
	ContextRef->IAGetPrimitiveTopology(&Topology);
	ContextRef->IAGetInputLayout(&InputLayout);
	ContextRef->OMGetRenderTargets(1, &RTV, &DSV);
	ContextRef->VSGetShader(&VS, nullptr, 0);
	ContextRef->PSGetShader(&PS, nullptr, 0);
}

void RenderStateCache::Restore()
{
	if (ContextRef == nullptr)
	{
		return;
	}

	ContextRef->RSSetViewports(NumViewports, &Viewport);
	ContextRef->RSSetState(RS);
	ContextRef->VSSetConstantBuffers(0, 1, &CB);
	ContextRef->IASetVertexBuffers(0, 1, &VB, &VBStride, &VBOffset);
	ContextRef->IASetIndexBuffer(IB, IBFormat, IBOffset);
	ContextRef->IASetPrimitiveTopology(Topology);
	ContextRef->IASetInputLayout(InputLayout);
	ContextRef->OMSetRenderTargets(1, &RTV, DSV);
	ContextRef->VSSetShader(VS, nullptr, 0);
	ContextRef->PSSetShader(PS, nullptr, 0);
}
