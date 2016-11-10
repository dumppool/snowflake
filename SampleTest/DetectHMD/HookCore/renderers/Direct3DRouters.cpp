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
		LVMSG("GetDirectHelper", "sth wrong, object: 0x%x, swap chain: 0x%x", SDirectHelper, SDirectHelper->GetSwapChain());
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
	HRESULT hr = S_FALSE;
	PFN_DXGISWAPCHAIN_PRESENT func = (PFN_DXGISWAPCHAIN_PRESENT)SRouter->GetOriginalEntry();
	if (func != nullptr)
	{
		LostVR::Get()->OnPresent_Direct3D11(This);
		hr = func(This, Sync, Flags);
	}
	else
	{
		LVMSG("Direct3D11Present", "null original entry");
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
		LVMSG(head, "null original entry");
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
