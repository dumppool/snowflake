 * file Direct3D11Renderer.cpp
 *
 * author luoxw
 *
 * 
 */
#include "HookCorePCH.h"
#include "Direct3D11Renderer.h"

using namespace lostvr;

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
, Device(nullptr)
, Context(nullptr)
, Buffer(nullptr)
, Buffer_Direct9Copy(nullptr)
, SwapChain(nullptr)
{
	if (device == nullptr)
	{
		LVMSG("Direct3D11Helper::Direct3D11Helper", "null device");
		return;
	}

	if (GIVer == EDirect3D::DeviceRef)
	{
		LVMSG("Direct3D11Helper::Direct3D11Helper", "wrong interface version: %s", GetEDirect3DString(GIVer));
		return;
	}

	IDirect3DSurface9* surface = nullptr;
	device->GetRenderTarget(0, &surface);

	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);

	InitializeRHI(surfaceDesc.Width, surfaceDesc.Height, GetDirect9FormatMatch(surfaceDesc.Format));
}

Direct3D11Helper::Direct3D11Helper(EDirect3D ver) : GIVer(ver)
, Device(nullptr)
, Context(nullptr)
, Buffer(nullptr)
, Buffer_Direct9Copy(nullptr)
, SwapChain(nullptr)
{
	if (GIVer != EDirect3D::DeviceRef)
	{
		InitializeRHI();
	}
}

Direct3D11Helper::Direct3D11Helper(EDirect3D ver, UINT width, UINT height, DXGI_FORMAT format) : GIVer(ver)
, Device(nullptr)
, Context(nullptr)
, Buffer(nullptr)
, Buffer_Direct9Copy(nullptr)
, SwapChain(nullptr)
{
	if (GIVer != EDirect3D::DeviceRef)
	{
		InitializeRHI(width, height, format);
	}
}

bool Direct3D11Helper::UpdateRHIWithSwapChain(IDXGISwapChain* swapChain)
{
	const CHAR* head = "Direct3D11Helper::UpdateRHIWithSwapChain";
	if (swapChain == nullptr)
	{
		LVMSG(head, "null swap chain");
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
		LVMSG(head, "invalid buffer");
		SAFE_RELEASE(surface);
		return false;
	}

	HRESULT hr = S_FALSE;
	IDirect3DSurface9* lockable = nullptr;
	IDirect3DTexture9* tex = nullptr;
	hr = device->CreateRenderTarget(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, D3DMULTISAMPLE_TYPE(0), 0, 1, &lockable, nullptr);
	if (FAILED(hr))
	{
		LVMSG(head, "create lockable render target failed, 0x%x", hr);
	}
	else
	{
		hr = device->StretchRect(surface, nullptr, lockable, nullptr, D3DTEXF_POINT);
		if (FAILED(hr))
		{
			LVMSG(head, "copy render target surface failed, 0x%x", hr);
		}
		else
		{
			D3DLOCKED_RECT locked = { 0 };
			hr = lockable->LockRect(&locked, nullptr, D3DLOCK_DISCARD);
			if (FAILED(hr))
			{
				LVMSG(head, "lock rect failed, 0x%x", hr);
			}
			else
			{
				D3D11_MAPPED_SUBRESOURCE mapped;
				hr = Context->Map(Buffer_Direct9Copy, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped); 
				if (FAILED(hr))
				{
					LVMSG(head, "map failed: 0x%x", hr);
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
		LVMSG(head, "intent to create unsupport interface, ver: %s", GetEDirect3DString(GIVer));
		return false;
	}

	if (FAILED(hr))
	{
		LVMSG(head, "failed to create factory, result: %d, ver: %s", hr, GetEDirect3DString(GIVer));
		return false;
	}

	HWND hwnd = GetForegroundWindow();
	IDXGIAdapter* adapter = nullptr;
	DXGI_ADAPTER_DESC adapterDesc;
	for (int i = 0;; ++i)
	{
		if (FAILED(factory->EnumAdapters(i, &adapter)))
		{
			LVMSG(head, "enum adapter ended at: %d, ver: %s", i, GetEDirect3DString(GIVer))
				break;
		}

		if (FAILED(hr = adapter->GetDesc(&adapterDesc)))
		{
			LVMSG(head, "failed to get description for adapter at: %d, result: %d, ver: %s", i, hr, GetEDirect3DString(GIVer));
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
		desc.Windowed = (GetWindowLong(hwnd, GWL_STYLE) & WS_POPUP) != 0 ? FALSE : TRUE;;
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

	return true;
}

bool Direct3D11Helper::OutputBuffer_Texture2D(ID3D11Texture2D* dst)
{
	const CHAR* head = "Direct3D11Helper::OutputBuffer_Texture2D";
	if (Context == nullptr || Buffer == nullptr)
	{
		LVMSG(head, "failed, context: 0x%x, buffer: 0x%x", Context, Buffer);
		return false;
	}

	D3D11_TEXTURE2D_DESC srcDesc, dstDesc;
	dst->GetDesc(&dstDesc);
	Buffer->GetDesc(&srcDesc);

#ifdef _DEBUG
	LVMSG(head, "width(%d), height(%d), format(%d), usage(%d), miplevels(%d), sampler count(%d)", srcDesc.Width, srcDesc.Height, srcDesc.Format, srcDesc.Usage, srcDesc.MipLevels, srcDesc.SampleDesc.Count);
	LVMSG(head, "width(%d), height(%d), format(%d), usage(%d), miplevels(%d), sampler count(%d)", dstDesc.Width, dstDesc.Height, dstDesc.Format, dstDesc.Usage, dstDesc.MipLevels, dstDesc.SampleDesc.Count);
#endif
	Context->CopyResource(dst, Buffer);
	return true;
}

bool Direct3D11Helper::OutputBuffer_Texture2D_Direct9(ID3D11Texture2D * dst)
{
	const CHAR* head = "Direct3D11Helper::OutputBuffer_Texture2D_Direct9";
	if (Context == nullptr || Buffer_Direct9Copy == nullptr)
	{
		LVMSG(head, "failed, context: 0x%x, buffer: 0x%x", Context, Buffer_Direct9Copy);
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
	HWND wnd = GetForegroundWindow();

	IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == nullptr)
	{
		LVMSG(head, "Direct3DCreate9 failed");
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
		LVMSG("HookDirect3DDevice9Present", "CreateDevice failed, 0x%x", hr);
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
			LVMSG(head, "the exist buffer is not fit the current direct9 buffer")
			SAFE_RELEASE(Buffer_Direct9Copy);
		}
	}

	if (Device == nullptr)
	{
		LVMSG(head, "null device");
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
	case D3DFMT_X8R8G8B8: ;
	case D3DFMT_X8B8G8R8: return DXGI_FORMAT_R8G8B8A8_UNORM;
	default: return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

HRESULT Direct3D11Helper::CreateShaderResourceViewBySwapChain(void ** ppTex, void ** ppView)
{
	if (SwapChain == nullptr || ppView == nullptr || Device == nullptr)
	{
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scDesc;
	SwapChain->GetDesc(&scDesc);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = scDesc.BufferDesc.Format;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = 1;
	ID3D11Texture2D* tex = 0;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex);
	HRESULT hr = Device->CreateShaderResourceView(tex, &desc, (ID3D11ShaderResourceView**)ppView);

	if (ppTex != nullptr)
	{
		*ppTex = tex;
	}
	else
	{
		SAFE_RELEASE(tex);
	}

	return hr;
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

bool Direct3D11Helper::CreateTexture2D(ID3D11Texture2D ** ppTex, ID3D11ShaderResourceView ** ppSRV, D3D11_TEXTURE2D_DESC * pDesc,
	ID3D11Device* deviceOverride)
{
	const CHAR* head = "Direct3D11Helper::CreateTexture2D";

	ID3D11Device* dev = deviceOverride != nullptr ? deviceOverride : GetDevice();
	if (dev == nullptr)
	{
		LVMSG(head, "null device");
		return false;
	}

	if (ppTex == nullptr && ppSRV == nullptr)
	{
		LVMSG(head, "both texture2d and srv are invalid");
		return false;
	}

	if (pDesc == nullptr)
	{
		LVMSG(head, "null description input");
		return false;
	}

	HRESULT hr = S_FALSE;

	ID3D11Texture2D* tex = nullptr;
	hr = dev->CreateTexture2D(pDesc, nullptr, &tex);
	if (FAILED(hr))
	{
		LVMSG(head, "create texture 2d failed: 0x%x(%d)", hr, hr);
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
		LVMSG(head, "create shader resource view failed: 0x%x(%d)", hr, hr);
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
	ID3D11Device* dev = GetDevice();
	if (dev == nullptr)
	{
		LVMSG(head, "null device");
		return false;
	}

	if (tex == nullptr)
	{
		LVMSG(head, "null source texture 2d");
		return false;
	}

	if (ppSRV == nullptr)
	{
		LVMSG(head, "null shader resource view output");
		return false;
	}

	D3D11_TEXTURE2D_DESC td;
	tex->GetDesc(&td);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = td.Format;
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sd.Texture2D.MipLevels = 1;
	return SUCCEEDED(dev->CreateShaderResourceView(tex, &sd, ppSRV));
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

				snprintf(&buf[0], 1023, "adapter subsytem id:\t%d\n\tadapter revision:\t%d\n\tadapter video memory:\t%lldmb\n\tadapter system memory:\t%lldmb\n\tadapter shared memory:\t%lldmb",
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

void lostvr::ContextCopyResource(ID3D11DeviceContext * context, ID3D11Texture2D * dst, ID3D11Texture2D * src, const CHAR* msgHead, bool bCheckContext)
{
	const CHAR* head = "ContextCopyResource";
	if (context == nullptr || dst == nullptr || src == nullptr)
	{
		return;
	}

	if (bCheckContext)
	{
		ID3D11Device *dstDevice, *srcDevice;
		ID3D11DeviceContext* cxt;
		dst->GetDevice(&dstDevice);
		src->GetDevice(&srcDevice);

		if (dstDevice != srcDevice)
		{
			LVMSG(head, "intent to copy between different context");
			SAFE_RELEASE(srcDevice);
			SAFE_RELEASE(dstDevice);
			return;
		}

		srcDevice->GetImmediateContext(&cxt);
		LVMSG(head, "%s, 0x%x, context(0x%x), input context(0x%x), flag(0x%x), feature(0x%x)",
			msgHead, srcDevice, cxt, context, srcDevice->GetCreationFlags(), srcDevice->GetFeatureLevel());
		SAFE_RELEASE(srcDevice);
		SAFE_RELEASE(dstDevice);
		SAFE_RELEASE(cxt);
	}

	context->CopyResource(dst, src);

	D3D11_TEXTURE2D_DESC dstDesc, srcDesc;
	dst->GetDesc(&dstDesc);
	src->GetDesc(&srcDesc);

	//context->ResolveSubresource(dst, 0, src, 0, dstDesc.Format);

	//LVMSG(head, "%s, dst width(%d), height(%d), format(%d)", msgHead, dstDesc.Width, dstDesc.Height, dstDesc.Format);
	//LVMSG(head, "%s, src width(%d), height(%d), format(%d)", msgHead, srcDesc.Width, srcDesc.Height, srcDesc.Format);
}
