#include "stdafx.h"
#include "WrappedDXGI.h"

#if 1
bool LVRefCountDXGIObject::HandleWrap(REFIID riid, void** ppvObject)
{
	if (ppvObject == NULL || *ppvObject == NULL)
	{
		LVMSG("null ppvObject", "HandleWrap");
		return false;
	}

	if (riid == __uuidof(IDXGIDevice))
	{
		LVMSG("Unexpected uuid", "HandleWrap");
		return false;
	}
	else if (riid == __uuidof(IDXGIFactory))
	{
		IDXGIFactory1* real = (IDXGIFactory1*)(*ppvObject);
		*ppvObject = (IDXGIFactory)(new LVWrappedDXGIFactory1(real));
		return true;
	}
	else if (riid == __uuidof(IDXGIFactory1))
	{
		IDXGIFactory1* real = (IDXGIFactory1*)(*ppvObject);
		*ppvObject = (IDXGIFactory1)(new LVWrappedDXGIFactory1(real));
		return true;
	}
	else
	{
		LVMSG("Querying IDXGIObject for interface.", "HandleWrap");
		return false;
	}
}

HRESULT LVRefCountDXGIObject::WrapQueryInterface(IUnknown* real, REFIID riid, void** ppvObject)
{
	HRESULT ret = real->QueryInterface(riid, ppvObject);
	if (SUCCEEDED(ret))
	{
		HandleWrap(riid, ppvObject);
	}

	return ret;
}

HRESULT STDMETHODCALLTYPE LVRefCountDXGIObject::GetParent(
	/* [in] */ REFIID riid,
	/* [retval][out] */ void **ppParent)
{
	HRESULT ret = m_pReal->GetParent(riid, ppParent);

	if (SUCCEEDED(ret))
	{
		HandleWrap(riid, ppParent);
	}

	return ret;
}
#endif