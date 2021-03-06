/*
 * file Direct3DRouters.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"
#include "RouteAPIs.h"
#include "Direct3D11Renderer.h"
#include "IRenderer.h"

using namespace lostvr;

static Direct3D11Helper* GetDirectHelper()
{
	static Direct3D11Helper* SDirectHelper = nullptr;
	if (SDirectHelper == nullptr)
	{
		SDirectHelper = new Direct3D11Helper(EDirect3D::DXGI0);
	}

	if (SDirectHelper == nullptr || SDirectHelper->GetSwapChain() == nullptr)
	{
		LVERROR("GetDirectHelper", "sth wrong, object: 0x%x, swap chain: 0x%x", SDirectHelper, SDirectHelper->GetSwapChain());
		return nullptr;
	}

	return SDirectHelper;
}

class Direct11DevicePresent : public MemberFunctionRouter
{
public:
	Direct11DevicePresent();

	virtual const CHAR* GetRouteString() const
	{
		return "[Direct11DevicePresent]";
	}

	virtual void* GetThisObject() override;
};

static Direct11DevicePresent* SRouter = new Direct11DevicePresent;

typedef HRESULT(STDMETHODCALLTYPE *PFN_DXGISWAPCHAIN_PRESENT)(IDXGISwapChain* This, UINT SyncInterval, UINT Flags);
static HRESULT STDMETHODCALLTYPE Direct3D11Present(IDXGISwapChain* This, UINT Sync, UINT Flags)
{
	const CHAR* head = "Direct3D11Present";
	HRESULT hr = S_FALSE;

	PFN_DXGISWAPCHAIN_PRESENT func = (PFN_DXGISWAPCHAIN_PRESENT)SRouter->GetOriginalEntry();
	if (func != nullptr)
	{
		LostVR::Get()->OnPresent_Direct3D11(This);

		hr = func(This, Sync, Flags);
		if (FAILED(hr))
		{
			if (hr == DXGI_ERROR_DEVICE_REMOVED)
			{
				ID3D11Device* dev = nullptr;
				This->GetDevice(__uuidof(ID3D11Device), (void**)&dev);
				HRESULT err = dev->GetDeviceRemovedReason();
				LVERROR(head, "device removed error: 0x%x(%d)", err, err);
			}
			else
			{
				LVERROR(head, "failed with 0x%x(%d)", hr, hr);
			}
		}
	}
	else
	{
		LVERROR(head, "null original entry");
	}

	return hr;
}

Direct11DevicePresent::Direct11DevicePresent() : 
	MemberFunctionRouter(Direct3D11Present, 8)
{
}

PVOID Direct11DevicePresent::GetThisObject()
{
	return (GetDirectHelper() != nullptr) ? GetDirectHelper()->GetSwapChain() : nullptr;
}

class Direct9DevicePresent : public MemberFunctionRouter
{
	Direct3D11Helper* Helper;

public:
	Direct9DevicePresent();

	virtual const CHAR* GetRouteString() const
	{
		return "[Direct9DevicePresent]";
	}

	virtual void* GetThisObject() override;
};

static Direct9DevicePresent* SRouter9 = new Direct9DevicePresent;

typedef HRESULT(STDMETHODCALLTYPE *PFN_DEVICE_PRESENT)(IDirect3DDevice9* This, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static HRESULT STDMETHODCALLTYPE Direct3D9Present(IDirect3DDevice9* This, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	const CHAR* head = "Direct3D9Present";
	HRESULT hr = S_FALSE;
	PFN_DEVICE_PRESENT func = (PFN_DEVICE_PRESENT)SRouter9->GetOriginalEntry();
	if (func != nullptr)
	{
		LostVR::Get()->OnPresent_Direct3D9(This);
		hr = func(This, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	else
	{
		LVERROR(head, "null original entry");
	}

	return hr;
}

Direct9DevicePresent::Direct9DevicePresent() : 
	MemberFunctionRouter(Direct3D9Present, 17)
{
}

void * Direct9DevicePresent::GetThisObject()
{
	return (GetDirectHelper() != nullptr) ? GetDirectHelper()->GetTemporaryDirect9Device() : nullptr;
}

/********************************************************/

class Direct9DeviceExPresent : public MemberFunctionRouter
{
	Direct3D11Helper* Helper;

public:
	Direct9DeviceExPresent();

	virtual const CHAR* GetRouteString() const
	{
		return "[Direct9DeviceExPresent]";
	}

	virtual void* GetThisObject() override;
};

static Direct9DeviceExPresent* SRouter9Ex = new Direct9DeviceExPresent;

typedef HRESULT(STDMETHODCALLTYPE *PFN_DEVICEEX_PRESENT)(IDirect3DDevice9Ex* This, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion);
static HRESULT STDMETHODCALLTYPE Direct3D9ExPresent(IDirect3DDevice9Ex* This, CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	const CHAR* head = "Direct3D9ExPresent";
	HRESULT hr = S_FALSE;
	PFN_DEVICEEX_PRESENT func = (PFN_DEVICEEX_PRESENT)SRouter9Ex->GetOriginalEntry();
	if (func != nullptr)
	{
		LostVR::Get()->OnPresent_Direct3D9Ex(This);
		hr = func(This, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	}
	else
	{
		LVERROR(head, "null original entry");
	}

	return hr;
}

Direct9DeviceExPresent::Direct9DeviceExPresent() :
	MemberFunctionRouter(Direct3D9ExPresent, 17)
{
}

void * Direct9DeviceExPresent::GetThisObject()
{
	return (GetDirectHelper() != nullptr) ? GetDirectHelper()->GetTemporaryDirect9DeviceEx() : nullptr;
}
