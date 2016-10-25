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

	InitializeRHI(surfaceDesc.Width, surfaceDesc.Height);
}

lostvr::Direct3D11Helper::Direct3D11Helper(EDirect3D InVer) : GIVer(InVer)
, Device(nullptr)
, Context(nullptr)
, Buffer(nullptr)
, SwapChain(nullptr)
{
	if (GIVer != EDirect3D::DeviceRef)
	{
		InitializeRHI(0, 0);
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
	device->GetRenderTarget(0, &surface);

	/*typedef struct _D3DSURFACE_DESC
{
	D3DFORMAT           Format;
	D3DRESOURCETYPE     Type;
	DWORD               Usage;
	D3DPOOL             Pool;

	D3DMULTISAMPLE_TYPE MultiSampleType;
	DWORD               MultiSampleQuality;
	UINT                Width;
	UINT                Height;
} D3DSURFACE_DESC;*/

	D3DSURFACE_DESC surfaceDesc;
	surface->GetDesc(&surfaceDesc);
	LVMSG(head, "render target desc: ptr(0x%x), format(%d), type(%d), usage(%d), pool(%d), width(%d), height(%d), multi sample type(%d), multi sample quality(%d)", surface,
		surfaceDesc.Format, surfaceDesc.Type, surfaceDesc.Usage, surfaceDesc.Pool, surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.MultiSampleType, surfaceDesc.MultiSampleQuality);

	HRESULT hr = S_FALSE;
	IDirect3DSurface9* lockable = nullptr;
	hr = device->CreateOffscreenPlainSurface(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, D3DPOOL_SCRATCH, &lockable, nullptr);
	if (lockable != nullptr && SUCCEEDED(hr))
	{
		LVMSG(head, "created off screen plain surface");
		if (device->GetFrontBufferData(0, lockable))
		{
			LVMSG(head, "got buffer data");
			D3DLOCKED_RECT locked = { 0 };
			hr = lockable->LockRect(&locked, nullptr, D3DLOCK_READONLY);
			if (SUCCEEDED(hr))
			{
				LVMSG(head, "locked, pitch: %d, pos: 0x%x", locked.Pitch, locked.pBits);
			}
		}
	}

	//device->GetFrontBufferData();
	//device->GetBackBuffer();
	//device->GetSwapChain();
	//device->GetTexture();
	//IDirect3DSurface9* dst = nullptr;
	//device->GetRenderTargetData(surface, dst);

	//dst->GetDesc(&surfaceDesc);


	SAFE_RELEASE(surface);
	return false;
}

bool Direct3D11Helper::InitializeRHI(UINT width, UINT height)
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
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
		if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, &featureLevels[0], 1,
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

	LVASSERT(Context != nullptr, head, "null context");
	LVASSERT(Buffer != nullptr, head, "null buffer");

	Context->CopyResource(dst, Buffer);
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

	IDirect3DDevice9* device = nullptr;
	if (FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
		wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pp, &device)))
	{
		LVMSG("HookDirect3DDevice9Present", "CreateDevice failed");
		return nullptr;
	}

	return device;
}
