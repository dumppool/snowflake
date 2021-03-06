/******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015-2016 Baldur Karlsson
* Copyright (c) 2014 Crytek
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#pragma once

MIDL_INTERFACE("6f15aaf2-d208-4e89-9ab4-489535d34f9c") ID3D11Texture2D;
MIDL_INTERFACE("dc8e63f3-d12b-4952-b47b-5e45026a862d") ID3D11Resource;
MIDL_INTERFACE("db6f6ddb-ac77-4e88-8253-819df9bbf140") ID3D11Device;
MIDL_INTERFACE("696442be-a72e-4059-bc79-5b5c98040fad") ID3D12Resource;
MIDL_INTERFACE("189819f1-1db6-4b57-be54-1821339b85f7") ID3D12Device;

class RefCountDXGIObject : public IDXGIObject
{
	IDXGIObject *m_pReal;
	unsigned int m_iRefcount;

public:
	RefCountDXGIObject(IDXGIObject *real) : m_pReal(real), m_iRefcount(1) {}
	virtual ~RefCountDXGIObject() {}
	static bool HandleWrap(REFIID riid, void **ppvObject);
	static HRESULT WrapQueryInterface(IUnknown *real, REFIID riid, void **ppvObject);

	//////////////////////////////
	// implement IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(
		/* [in] */ REFIID riid,
		/* [annotation][iid_is][out] */
		__RPC__deref_out void **ppvObject)
	{
		if (riid == __uuidof(IUnknown))
		{
			AddRef();
			*ppvObject = (IUnknown *)(IDXGIObject *)this;
			return S_OK;
		}

		return WrapQueryInterface(m_pReal, riid, ppvObject);
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		InterlockedIncrement(&m_iRefcount);
		return m_iRefcount;
	}
	ULONG STDMETHODCALLTYPE Release()
	{
		unsigned int ret = InterlockedDecrement(&m_iRefcount);
		if (ret == 0)
			delete this;
		return ret;
	}

	//////////////////////////////
	// implement IDXGIObject

	virtual HRESULT STDMETHODCALLTYPE SetPrivateData(
		/* [in] */ REFGUID Name,
		/* [in] */ UINT DataSize,
		/* [in] */ const void *pData)
	{
		return m_pReal->SetPrivateData(Name, DataSize, pData);
	}

	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(
		/* [in] */ REFGUID Name,
		/* [in] */ const IUnknown *pUnknown)
	{
		return m_pReal->SetPrivateDataInterface(Name, pUnknown);
	}

	virtual HRESULT STDMETHODCALLTYPE GetPrivateData(
		/* [in] */ REFGUID Name,
		/* [out][in] */ UINT *pDataSize,
		/* [out] */ void *pData)
	{
		return m_pReal->GetPrivateData(Name, pDataSize, pData);
	}

	virtual HRESULT STDMETHODCALLTYPE GetParent(
		/* [in] */ REFIID riid,
		/* [retval][out] */ void **ppParent);
};

#define IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT                                      \
  ULONG STDMETHODCALLTYPE AddRef() { return RefCountDXGIObject::AddRef(); }                \
  ULONG STDMETHODCALLTYPE Release() { return RefCountDXGIObject::Release(); }              \
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)                  \
  {                                                                                        \
    return RefCountDXGIObject::QueryInterface(riid, ppvObject);                            \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE SetPrivateData(REFIID Name, UINT DataSize, const void *pData)  \
  {                                                                                        \
    return RefCountDXGIObject::SetPrivateData(Name, DataSize, pData);                      \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFIID Name, const IUnknown *pUnknown) \
  {                                                                                        \
    return RefCountDXGIObject::SetPrivateDataInterface(Name, pUnknown);                    \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE GetPrivateData(REFIID Name, UINT *pDataSize, void *pData)      \
  {                                                                                        \
    return RefCountDXGIObject::GetPrivateData(Name, pDataSize, pData);                     \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppvObject)                       \
  {                                                                                        \
    return RefCountDXGIObject::GetParent(riid, ppvObject);                                 \
  }

#define IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT_CUSTOMQUERY                          \
  ULONG STDMETHODCALLTYPE AddRef() { return RefCountDXGIObject::AddRef(); }                \
  ULONG STDMETHODCALLTYPE Release() { return RefCountDXGIObject::Release(); }              \
  HRESULT STDMETHODCALLTYPE SetPrivateData(REFIID Name, UINT DataSize, const void *pData)  \
  {                                                                                        \
    return RefCountDXGIObject::SetPrivateData(Name, DataSize, pData);                      \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFIID Name, const IUnknown *pUnknown) \
  {                                                                                        \
    return RefCountDXGIObject::SetPrivateDataInterface(Name, pUnknown);                    \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE GetPrivateData(REFIID Name, UINT *pDataSize, void *pData)      \
  {                                                                                        \
    return RefCountDXGIObject::GetPrivateData(Name, pDataSize, pData);                     \
  }                                                                                        \
  HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppvObject)                       \
  {                                                                                        \
    return RefCountDXGIObject::GetParent(riid, ppvObject);                                 \
  }

class WrappedIDXGISwapChain3;

#ifndef USE_FULLRDC
#define USE_FULLRDC 0
#endif

#if USE_FULLRDC
struct ID3DDevice
{
	// re-use IUnknown
	virtual ULONG STDMETHODCALLTYPE AddRef() = 0;
	virtual ULONG STDMETHODCALLTYPE Release() = 0;
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;

	virtual IUnknown *GetRealIUnknown() = 0;

	virtual IID GetBackbufferUUID() = 0;

	virtual IID GetDeviceUUID() = 0;
	virtual IUnknown *GetDeviceInterface() = 0;

	virtual void FirstFrame(WrappedIDXGISwapChain3 *swapChain) = 0;

	virtual void NewSwapchainBuffer(IUnknown *backbuffer) = 0;
	virtual void ReleaseSwapchainResources(WrappedIDXGISwapChain3 *swapChain) = 0;
	virtual IUnknown *WrapSwapchainBuffer(WrappedIDXGISwapChain3 *swap, DXGI_SWAP_CHAIN_DESC *swapDesc,
		UINT buffer, IUnknown *realSurface) = 0;

	virtual HRESULT Present(WrappedIDXGISwapChain3 *swapChain, UINT SyncInterval, UINT Flags) = 0;
};
#else
typedef ID3D11Device ID3DDevice;
#endif

typedef ID3DDevice *(*D3DDeviceCallback)(IUnknown *dev);

template <typename NestedType>
class WrappedDXGIInterface : public RefCountDXGIObject,
	public IDXGIKeyedMutex,
	public IDXGISurface2,
	public IDXGIResource1
{
public:
	ID3DDevice *m_pDevice;
	NestedType *m_pWrapped;

	WrappedDXGIInterface(NestedType *wrapped, ID3DDevice *device)
		: RefCountDXGIObject(NULL), m_pDevice(device), m_pWrapped(wrapped)
	{
		m_pWrapped->AddRef();
		m_pDevice->AddRef();
	}

	virtual ~WrappedDXGIInterface()
	{
		m_pWrapped->Release();
		m_pDevice->Release();
	}

	//////////////////////////////
	// Implement IUnknown
	ULONG STDMETHODCALLTYPE AddRef() { return RefCountDXGIObject::AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return RefCountDXGIObject::Release(); }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		// ensure the real object has this interface
		void *outObj;
		HRESULT hr = m_pWrapped->QueryInterface(riid, &outObj);

		IUnknown *unk = (IUnknown *)outObj;
		SAFE_RELEASE(unk);

		if (FAILED(hr))
		{
			return hr;
		}

		if (riid == __uuidof(IUnknown))
		{
			*ppvObject = (IUnknown *)(IDXGIKeyedMutex *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGIObject))
		{
			*ppvObject = (IDXGIObject *)(IDXGIKeyedMutex *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGIDeviceSubObject))
		{
			*ppvObject = (IDXGIDeviceSubObject *)(IDXGIKeyedMutex *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGIResource))
		{
			*ppvObject = (IDXGIResource *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGIKeyedMutex))
		{
			*ppvObject = (IDXGIKeyedMutex *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGISurface))
		{
			*ppvObject = (IDXGISurface *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGISurface1))
		{
			*ppvObject = (IDXGISurface1 *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGIResource1))
		{
			*ppvObject = (IDXGIResource1 *)this;
			AddRef();
			return S_OK;
		}
		if (riid == __uuidof(IDXGISurface2))
		{
			*ppvObject = (IDXGISurface2 *)this;
			AddRef();
			return S_OK;
		}

		return m_pWrapped->QueryInterface(riid, ppvObject);
	}

	//////////////////////////////
	// Implement IDXGIObject
	HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void *pData)
	{
		return m_pWrapped->SetPrivateData(Name, DataSize, pData);
	}

	HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown)
	{
		return m_pWrapped->SetPrivateDataInterface(Name, pUnknown);
	}

	HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData)
	{
		return m_pWrapped->GetPrivateData(Name, pDataSize, pData);
	}

	// this should only be called for adapters, devices, factories etc
	// so we pass it onto the device
	HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppParent)
	{
		return m_pDevice->QueryInterface(riid, ppParent);
	}

	//////////////////////////////
	// Implement IDXGIDeviceSubObject

	// same as GetParent
	HRESULT STDMETHODCALLTYPE GetDevice(REFIID riid, void **ppDevice)
	{
		return m_pDevice->QueryInterface(riid, ppDevice);
	}

	//////////////////////////////
	// Implement IDXGIKeyedMutex
	HRESULT STDMETHODCALLTYPE AcquireSync(UINT64 Key, DWORD dwMilliseconds)
	{
		// temporarily get the real interface
		IDXGIKeyedMutex *mutex = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&mutex);
		if (FAILED(hr))
		{
			SAFE_RELEASE(mutex);
			return hr;
		}

		hr = mutex->AcquireSync(Key, dwMilliseconds);
		SAFE_RELEASE(mutex);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE ReleaseSync(UINT64 Key)
	{
		// temporarily get the real interface
		IDXGIKeyedMutex *mutex = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&mutex);
		if (FAILED(hr))
		{
			SAFE_RELEASE(mutex);
			return hr;
		}

		hr = mutex->ReleaseSync(Key);
		SAFE_RELEASE(mutex);
		return hr;
	}

	//////////////////////////////
	// Implement IDXGIResource
	virtual HRESULT STDMETHODCALLTYPE GetSharedHandle(HANDLE *pSharedHandle)
	{
		// temporarily get the real interface
		IDXGIResource *res = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
		if (FAILED(hr))
		{
			SAFE_RELEASE(res);
			return hr;
		}

		hr = res->GetSharedHandle(pSharedHandle);
		SAFE_RELEASE(res);
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE GetUsage(DXGI_USAGE *pUsage)
	{
		// temporarily get the real interface
		IDXGIResource *res = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
		if (FAILED(hr))
		{
			SAFE_RELEASE(res);
			return hr;
		}

		hr = res->GetUsage(pUsage);
		SAFE_RELEASE(res);
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE SetEvictionPriority(UINT EvictionPriority)
	{
		// temporarily get the real interface
		IDXGIResource *res = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
		if (FAILED(hr))
		{
			SAFE_RELEASE(res);
			return hr;
		}

		hr = res->SetEvictionPriority(EvictionPriority);
		SAFE_RELEASE(res);
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE GetEvictionPriority(UINT *pEvictionPriority)
	{
		// temporarily get the real interface
		IDXGIResource *res = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
		if (FAILED(hr))
		{
			SAFE_RELEASE(res);
			return hr;
		}

		hr = res->GetEvictionPriority(pEvictionPriority);
		SAFE_RELEASE(res);
		return hr;
	}

	//////////////////////////////
	// Implement IDXGISurface
	virtual HRESULT STDMETHODCALLTYPE GetDesc(DXGI_SURFACE_DESC *pDesc)
	{
		// temporarily get the real interface
		IDXGISurface *surf = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface), (void **)&surf);
		if (FAILED(hr))
		{
			SAFE_RELEASE(surf);
			return hr;
		}

		hr = surf->GetDesc(pDesc);
		SAFE_RELEASE(surf);
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE Map(DXGI_MAPPED_RECT *pLockedRect, UINT MapFlags)
	{
		// temporarily get the real interface
		IDXGISurface *surf = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface), (void **)&surf);
		if (FAILED(hr))
		{
			SAFE_RELEASE(surf);
			return hr;
		}

		hr = surf->Map(pLockedRect, MapFlags);
		SAFE_RELEASE(surf);
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE Unmap(void)
	{
		// temporarily get the real interface
		IDXGISurface *surf = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface), (void **)&surf);
		if (FAILED(hr))
		{
			SAFE_RELEASE(surf);
			return hr;
		}

		hr = surf->Unmap();
		SAFE_RELEASE(surf);
		return hr;
	}

	//////////////////////////////
	// Implement IDXGISurface1
	virtual HRESULT STDMETHODCALLTYPE GetDC(BOOL Discard, HDC *phdc)
	{
		// temporarily get the real interface
		IDXGISurface1 *surf = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface1), (void **)&surf);
		if (FAILED(hr))
		{
			SAFE_RELEASE(surf);
			return hr;
		}

		hr = surf->GetDC(Discard, phdc);
		SAFE_RELEASE(surf);
		return hr;
	}

	virtual HRESULT STDMETHODCALLTYPE ReleaseDC(RECT *pDirtyRect)
	{
		// temporarily get the real interface
		IDXGISurface1 *surf = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface1), (void **)&surf);
		if (FAILED(hr))
		{
			SAFE_RELEASE(surf);
			return hr;
		}

		hr = surf->ReleaseDC(pDirtyRect);
		SAFE_RELEASE(surf);
		return hr;
	}

	//////////////////////////////
	// Implement IDXGIResource1
	virtual HRESULT STDMETHODCALLTYPE CreateSubresourceSurface(UINT index, IDXGISurface2 **ppSurface)
	{
		if (ppSurface == NULL)
			return E_INVALIDARG;

		// maybe this will work?!?
		AddRef();
		*ppSurface = (IDXGISurface2 *)this;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSharedHandle(const SECURITY_ATTRIBUTES *pAttributes,
		DWORD dwAccess, LPCWSTR lpName,
		HANDLE *pHandle)
	{
		// temporarily get the real interface
		IDXGIResource1 *res = NULL;
		HRESULT hr = m_pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource1), (void **)&res);
		if (FAILED(hr))
		{
			SAFE_RELEASE(res);
			return hr;
		}

		hr = res->CreateSharedHandle(pAttributes, dwAccess, lpName, pHandle);
		SAFE_RELEASE(res);
		return hr;
	}

	//////////////////////////////
	// Implement IDXGISurface2
	virtual HRESULT STDMETHODCALLTYPE GetResource(REFIID riid, void **ppParentResource,
		UINT *pSubresourceIndex)
	{
		// not really sure how to implement this :(.
		if (pSubresourceIndex)
			pSubresourceIndex = 0;
		return QueryInterface(riid, ppParentResource);
	}
};

class WrappedIDXGISwapChain3 : public IDXGISwapChain3, public RefCountDXGIObject
{
	IDXGISwapChain *m_pReal;
	IDXGISwapChain1 *m_pReal1;
	IDXGISwapChain2 *m_pReal2;
	IDXGISwapChain3 *m_pReal3;
	ID3DDevice *m_pDevice;
	unsigned int m_iRefcount;

	static std::vector<D3DDeviceCallback> m_D3DCallbacks;

	HWND m_Wnd;

	static const int MAX_NUM_BACKBUFFERS = 4;

	IUnknown *m_pBackBuffers[MAX_NUM_BACKBUFFERS];

	void ReleaseBuffersForResize();
	void WrapBuffersAfterResize();

public:
	WrappedIDXGISwapChain3(IDXGISwapChain *real, HWND wnd, ID3DDevice *device);
	virtual ~WrappedIDXGISwapChain3();

	static void RegisterD3DDeviceCallback(D3DDeviceCallback callback)
	{
		m_D3DCallbacks.push_back(callback);
	}

	static ID3DDevice *GetD3DDevice(IUnknown *dev)
	{
		for (size_t i = 0; i < m_D3DCallbacks.size(); i++)
		{
			ID3DDevice *d3d = m_D3DCallbacks[i](dev);
			if (d3d)
				return d3d;
		}

		return NULL;
	}

	int GetNumBackbuffers() { return MAX_NUM_BACKBUFFERS; }
	IUnknown **GetBackbuffers() { return m_pBackBuffers; }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT_CUSTOMQUERY;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

	//////////////////////////////
	// implement IDXGIDeviceSubObject

	virtual HRESULT STDMETHODCALLTYPE GetDevice(
		/* [in] */ REFIID riid,
		/* [retval][out] */ void **ppDevice);

	//////////////////////////////
	// implement IDXGISwapChain

	virtual HRESULT STDMETHODCALLTYPE Present(
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags);

	virtual HRESULT STDMETHODCALLTYPE GetBuffer(
		/* [in] */ UINT Buffer,
		/* [in] */ REFIID riid,
		/* [out][in] */ void **ppSurface);

	virtual HRESULT STDMETHODCALLTYPE SetFullscreenState(
		/* [in] */ BOOL Fullscreen,
		/* [in] */ IDXGIOutput *pTarget);

	virtual HRESULT STDMETHODCALLTYPE GetFullscreenState(
		/* [out] */ BOOL *pFullscreen,
		/* [out] */ IDXGIOutput **ppTarget);

	virtual HRESULT STDMETHODCALLTYPE GetDesc(
		/* [out] */ DXGI_SWAP_CHAIN_DESC *pDesc)
	{
		return m_pReal->GetDesc(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE ResizeBuffers(
		/* [in] */ UINT BufferCount,
		/* [in] */ UINT Width,
		/* [in] */ UINT Height,
		/* [in] */ DXGI_FORMAT NewFormat,
		/* [in] */ UINT SwapChainFlags);

	virtual HRESULT STDMETHODCALLTYPE ResizeTarget(
		/* [in] */ const DXGI_MODE_DESC *pNewTargetParameters)
	{
		return m_pReal->ResizeTarget(pNewTargetParameters);
	}

	virtual HRESULT STDMETHODCALLTYPE GetContainingOutput(IDXGIOutput **ppOutput)
	{
		return m_pReal->GetContainingOutput(ppOutput);
	}

	virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics(
		/* [out] */ DXGI_FRAME_STATISTICS *pStats)
	{
		return m_pReal->GetFrameStatistics(pStats);
	}

	virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount(
		/* [out] */ UINT *pLastPresentCount)
	{
		return m_pReal->GetLastPresentCount(pLastPresentCount);
	}

	//////////////////////////////
	// implement IDXGISwapChain1

	virtual HRESULT STDMETHODCALLTYPE GetDesc1(
		/* [annotation][out] */
		_Out_ DXGI_SWAP_CHAIN_DESC1 *pDesc)
	{
		return m_pReal2->GetDesc1(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE GetFullscreenDesc(
		/* [annotation][out] */
		_Out_ DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pDesc)
	{
		return m_pReal2->GetFullscreenDesc(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE GetHwnd(
		/* [annotation][out] */
		_Out_ HWND *pHwnd)
	{
		return m_pReal2->GetHwnd(pHwnd);
	}

	virtual HRESULT STDMETHODCALLTYPE GetCoreWindow(
		/* [annotation][in] */
		_In_ REFIID refiid,
		/* [annotation][out] */
		_Out_ void **ppUnk)
	{
		return m_pReal2->GetCoreWindow(refiid, ppUnk);
	}

	virtual HRESULT STDMETHODCALLTYPE Present1(
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT PresentFlags,
		/* [annotation][in] */
		_In_ const DXGI_PRESENT_PARAMETERS *pPresentParameters);

	virtual BOOL STDMETHODCALLTYPE IsTemporaryMonoSupported(void)
	{
		return m_pReal2->IsTemporaryMonoSupported();
	}

	virtual HRESULT STDMETHODCALLTYPE GetRestrictToOutput(
		/* [annotation][out] */
		_Out_ IDXGIOutput **ppRestrictToOutput)
	{
		return m_pReal2->GetRestrictToOutput(ppRestrictToOutput);
	}

	virtual HRESULT STDMETHODCALLTYPE SetBackgroundColor(
		/* [annotation][in] */
		_In_ const DXGI_RGBA *pColor)
	{
		return m_pReal2->SetBackgroundColor(pColor);
	}

	virtual HRESULT STDMETHODCALLTYPE GetBackgroundColor(
		/* [annotation][out] */
		_Out_ DXGI_RGBA *pColor)
	{
		return m_pReal2->GetBackgroundColor(pColor);
	}

	virtual HRESULT STDMETHODCALLTYPE SetRotation(
		/* [annotation][in] */
		_In_ DXGI_MODE_ROTATION Rotation)
	{
		return m_pReal2->SetRotation(Rotation);
	}

	virtual HRESULT STDMETHODCALLTYPE GetRotation(
		/* [annotation][out] */
		_Out_ DXGI_MODE_ROTATION *pRotation)
	{
		return m_pReal2->GetRotation(pRotation);
	}

	//////////////////////////////
	// implement IDXGISwapChain2

	virtual HRESULT STDMETHODCALLTYPE SetSourceSize(UINT Width, UINT Height)
	{
		return m_pReal2->SetSourceSize(Width, Height);
	}

	virtual HRESULT STDMETHODCALLTYPE GetSourceSize(
		/* [annotation][out] */
		_Out_ UINT *pWidth,
		/* [annotation][out] */
		_Out_ UINT *pHeight)
	{
		return m_pReal2->GetSourceSize(pWidth, pHeight);
	}

	virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(UINT MaxLatency)
	{
		return m_pReal2->SetMaximumFrameLatency(MaxLatency);
	}

	virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(
		/* [annotation][out] */
		_Out_ UINT *pMaxLatency)
	{
		return m_pReal2->GetMaximumFrameLatency(pMaxLatency);
	}

	virtual HANDLE STDMETHODCALLTYPE GetFrameLatencyWaitableObject(void)
	{
		return m_pReal2->GetFrameLatencyWaitableObject();
	}

	virtual HRESULT STDMETHODCALLTYPE SetMatrixTransform(const DXGI_MATRIX_3X2_F *pMatrix)
	{
		return m_pReal2->SetMatrixTransform(pMatrix);
	}

	virtual HRESULT STDMETHODCALLTYPE GetMatrixTransform(
		/* [annotation][out] */
		_Out_ DXGI_MATRIX_3X2_F *pMatrix)
	{
		return m_pReal2->GetMatrixTransform(pMatrix);
	}

	//////////////////////////////
	// implement IDXGISwapChain3

	virtual UINT STDMETHODCALLTYPE GetCurrentBackBufferIndex(void)
	{
		return m_pReal3->GetCurrentBackBufferIndex();
	}

	virtual HRESULT STDMETHODCALLTYPE CheckColorSpaceSupport(
		/* [annotation][in] */
		_In_ DXGI_COLOR_SPACE_TYPE ColorSpace,
		/* [annotation][out] */
		_Out_ UINT *pColorSpaceSupport)
	{
		return m_pReal3->CheckColorSpaceSupport(ColorSpace, pColorSpaceSupport);
	}

	virtual HRESULT STDMETHODCALLTYPE SetColorSpace1(
		/* [annotation][in] */
		_In_ DXGI_COLOR_SPACE_TYPE ColorSpace)
	{
		return m_pReal3->SetColorSpace1(ColorSpace);
	}

	virtual HRESULT STDMETHODCALLTYPE ResizeBuffers1(
		/* [annotation][in] */
		_In_ UINT BufferCount,
		/* [annotation][in] */
		_In_ UINT Width,
		/* [annotation][in] */
		_In_ UINT Height,
		/* [annotation][in] */
		_In_ DXGI_FORMAT Format,
		/* [annotation][in] */
		_In_ UINT SwapChainFlags,
		/* [annotation][in] */
		_In_reads_(BufferCount) const UINT *pCreationNodeMask,
		/* [annotation][in] */
		_In_reads_(BufferCount) IUnknown *const *ppPresentQueue);
};

//////////////////////////////////////////////////////////////////////////////
// Crap classes we don't really care about, except capturing the swap chain

class WrappedIDXGIAdapter : public IDXGIAdapter, public RefCountDXGIObject
{
	IDXGIAdapter *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIAdapter(IDXGIAdapter *real) : RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIAdapter() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIAdapter

	virtual HRESULT STDMETHODCALLTYPE EnumOutputs(
		/* [in] */ UINT Output,
		/* [annotation][out][in] */
		__out IDXGIOutput **ppOutput)
	{
		return m_pReal->EnumOutputs(Output, ppOutput);
	}

	virtual HRESULT STDMETHODCALLTYPE GetDesc(
		/* [annotation][out] */
		__out DXGI_ADAPTER_DESC *pDesc)
	{
		return m_pReal->GetDesc(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(
		/* [annotation][in] */
		__in REFGUID InterfaceName,
		/* [annotation][out] */
		__out LARGE_INTEGER *pUMDVersion)
	{
		return m_pReal->CheckInterfaceSupport(InterfaceName, pUMDVersion);
	}
};

class WrappedIDXGIDevice : public IDXGIDevice, public RefCountDXGIObject
{
	IDXGIDevice *m_pReal;
	ID3DDevice *m_pD3DDevice;

public:
	WrappedIDXGIDevice(IDXGIDevice *real, ID3DDevice *d3d)
		: RefCountDXGIObject(real), m_pReal(real), m_pD3DDevice(d3d)
	{
		m_pD3DDevice->AddRef();
	}

	virtual ~WrappedIDXGIDevice()
	{
		SAFE_RELEASE(m_pReal);
		SAFE_RELEASE(m_pD3DDevice);
	}

	static const int AllocPoolCount = 4;
	ALLOCATE_WITH_WRAPPED_POOL(WrappedIDXGIDevice, AllocPoolCount);

	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT_CUSTOMQUERY;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

	ID3DDevice *GetD3DDevice() { return m_pD3DDevice; }
	//////////////////////////////
	// implement IDXGIDevice

	virtual HRESULT STDMETHODCALLTYPE GetAdapter(
		/* [annotation][out] */
		__out IDXGIAdapter **pAdapter)
	{
		HRESULT ret = m_pReal->GetAdapter(pAdapter);
		if (SUCCEEDED(ret))
			*pAdapter = new WrappedIDXGIAdapter(*pAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSurface(
		/* [annotation][in] */
		__in const DXGI_SURFACE_DESC *pDesc,
		/* [in] */ UINT NumSurfaces,
		/* [in] */ DXGI_USAGE Usage,
		/* [annotation][in] */
		__in_opt const DXGI_SHARED_RESOURCE *pSharedResource,
		/* [annotation][out] */
		__out IDXGISurface **ppSurface)
	{
		return m_pReal->CreateSurface(pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(
		/* [annotation][size_is][in] */
		__in_ecount(NumResources) IUnknown *const *ppResources,
		/* [annotation][size_is][out] */
		__out_ecount(NumResources) DXGI_RESIDENCY *pResidencyStatus,
		/* [in] */ UINT NumResources)
	{
		return m_pReal->QueryResourceResidency(ppResources, pResidencyStatus, NumResources);
	}

	virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(
		/* [in] */ INT Priority)
	{
		return m_pReal->SetGPUThreadPriority(Priority);
	}

	virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(
		/* [annotation][retval][out] */
		__out INT *pPriority)
	{
		return m_pReal->GetGPUThreadPriority(pPriority);
	}
};

class WrappedIDXGIFactory : public IDXGIFactory, public RefCountDXGIObject
{
	IDXGIFactory *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIFactory(IDXGIFactory *real) : RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIFactory() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIFactory

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags)
	{
		return m_pReal->MakeWindowAssociation(WindowHandle, Flags);
	}

	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
		/* [annotation][out] */
		__out HWND *pWindowHandle)
	{
		return m_pReal->GetWindowAssociation(pWindowHandle);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
		/* [annotation][in] */
		__in IUnknown *pDevice,
		/* [annotation][in] */
		__in DXGI_SWAP_CHAIN_DESC *pDesc,
		/* [annotation][out] */
		__out IDXGISwapChain **ppSwapChain)
	{
		return WrappedIDXGIFactory::staticCreateSwapChain(m_pReal, pDevice, pDesc, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
		/* [in] */ HMODULE Module,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->CreateSoftwareAdapter(Module, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	static HRESULT staticCreateSwapChain(IDXGIFactory *factory, IUnknown *pDevice,
		DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain);
};

// version 1

class WrappedIDXGIAdapter1 : public IDXGIAdapter1, public RefCountDXGIObject
{
	IDXGIAdapter1 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIAdapter1(IDXGIAdapter1 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIAdapter1() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIAdapter

	virtual HRESULT STDMETHODCALLTYPE EnumOutputs(
		/* [in] */ UINT Output,
		/* [annotation][out][in] */
		__out IDXGIOutput **ppOutput)
	{
		return m_pReal->EnumOutputs(Output, ppOutput);
	}

	virtual HRESULT STDMETHODCALLTYPE GetDesc(
		/* [annotation][out] */
		__out DXGI_ADAPTER_DESC *pDesc)
	{
		return m_pReal->GetDesc(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(
		/* [annotation][in] */
		__in REFGUID InterfaceName,
		/* [annotation][out] */
		__out LARGE_INTEGER *pUMDVersion)
	{
		return m_pReal->CheckInterfaceSupport(InterfaceName, pUMDVersion);
	}

	//////////////////////////////
	// implement IDXGIAdapter1

	virtual HRESULT STDMETHODCALLTYPE GetDesc1(
		/* [out] */ DXGI_ADAPTER_DESC1 *pDesc)
	{
		return m_pReal->GetDesc1(pDesc);
	}
};

class WrappedIDXGIDevice1 : public IDXGIDevice1, public RefCountDXGIObject
{
	IDXGIDevice1 *m_pReal;
	ID3DDevice *m_pD3DDevice;

public:
	WrappedIDXGIDevice1(IDXGIDevice1 *real, ID3DDevice *d3d)
		: RefCountDXGIObject(real), m_pReal(real), m_pD3DDevice(d3d)
	{
		m_pD3DDevice->AddRef();
	}
	virtual ~WrappedIDXGIDevice1()
	{
		SAFE_RELEASE(m_pReal);
		SAFE_RELEASE(m_pD3DDevice);
	}

	static const int AllocPoolCount = 4;
	ALLOCATE_WITH_WRAPPED_POOL(WrappedIDXGIDevice1, AllocPoolCount);

	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT_CUSTOMQUERY;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

	ID3DDevice *GetD3DDevice() { return m_pD3DDevice; }
	//////////////////////////////
	// implement IDXGIDevice

	virtual HRESULT STDMETHODCALLTYPE GetAdapter(
		/* [annotation][out] */
		__out IDXGIAdapter **pAdapter)
	{
		HRESULT ret = m_pReal->GetAdapter(pAdapter);
		if (SUCCEEDED(ret))
			*pAdapter = new WrappedIDXGIAdapter(*pAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSurface(
		/* [annotation][in] */
		__in const DXGI_SURFACE_DESC *pDesc,
		/* [in] */ UINT NumSurfaces,
		/* [in] */ DXGI_USAGE Usage,
		/* [annotation][in] */
		__in_opt const DXGI_SHARED_RESOURCE *pSharedResource,
		/* [annotation][out] */
		__out IDXGISurface **ppSurface)
	{
		return m_pReal->CreateSurface(pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(
		/* [annotation][size_is][in] */
		__in_ecount(NumResources) IUnknown *const *ppResources,
		/* [annotation][size_is][out] */
		__out_ecount(NumResources) DXGI_RESIDENCY *pResidencyStatus,
		/* [in] */ UINT NumResources)
	{
		return m_pReal->QueryResourceResidency(ppResources, pResidencyStatus, NumResources);
	}

	virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(
		/* [in] */ INT Priority)
	{
		return m_pReal->SetGPUThreadPriority(Priority);
	}

	virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(
		/* [annotation][retval][out] */
		__out INT *pPriority)
	{
		return m_pReal->GetGPUThreadPriority(pPriority);
	}

	//////////////////////////////
	// implement IDXGIDevice1

	virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(
		/* [in] */ UINT MaxLatency)
	{
		return m_pReal->SetMaximumFrameLatency(MaxLatency);
	}

	virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(
		/* [annotation][out] */
		__out UINT *pMaxLatency)
	{
		return m_pReal->GetMaximumFrameLatency(pMaxLatency);
	}
};

class WrappedIDXGIFactory1 : public IDXGIFactory1, public RefCountDXGIObject
{
	IDXGIFactory1 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIFactory1(IDXGIFactory1 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIFactory1() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIFactory

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags)
	{
		return m_pReal->MakeWindowAssociation(WindowHandle, Flags);
	}

	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
		/* [annotation][out] */
		__out HWND *pWindowHandle)
	{
		return m_pReal->GetWindowAssociation(pWindowHandle);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
		/* [annotation][in] */
		__in IUnknown *pDevice,
		/* [annotation][in] */
		__in DXGI_SWAP_CHAIN_DESC *pDesc,
		/* [annotation][out] */
		__out IDXGISwapChain **ppSwapChain)
	{
		return WrappedIDXGIFactory::staticCreateSwapChain(m_pReal, pDevice, pDesc, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
		/* [in] */ HMODULE Module,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->CreateSoftwareAdapter(Module, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	//////////////////////////////
	// implement IDXGIFactory1

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter1 **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters1(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter1(*ppAdapter);
		return ret;
	}

	virtual BOOL STDMETHODCALLTYPE IsCurrent(void) { return m_pReal->IsCurrent(); }
};

class WrappedIDXGIDevice2 : public IDXGIDevice2, public RefCountDXGIObject
{
	IDXGIDevice2 *m_pReal;
	ID3DDevice *m_pD3DDevice;

public:
	WrappedIDXGIDevice2(IDXGIDevice2 *real, ID3DDevice *d3d)
		: RefCountDXGIObject(real), m_pReal(real), m_pD3DDevice(d3d)
	{
		m_pD3DDevice->AddRef();
	}
	virtual ~WrappedIDXGIDevice2()
	{
		SAFE_RELEASE(m_pReal);
		SAFE_RELEASE(m_pD3DDevice);
	}

	static const int AllocPoolCount = 4;
	ALLOCATE_WITH_WRAPPED_POOL(WrappedIDXGIDevice2, AllocPoolCount);

	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT_CUSTOMQUERY;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

	ID3DDevice *GetD3DDevice() { return m_pD3DDevice; }
	//////////////////////////////
	// implement IDXGIDevice

	virtual HRESULT STDMETHODCALLTYPE GetAdapter(
		/* [annotation][out] */
		__out IDXGIAdapter **pAdapter)
	{
		HRESULT ret = m_pReal->GetAdapter(pAdapter);
		if (SUCCEEDED(ret))
			*pAdapter = new WrappedIDXGIAdapter(*pAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSurface(
		/* [annotation][in] */
		__in const DXGI_SURFACE_DESC *pDesc,
		/* [in] */ UINT NumSurfaces,
		/* [in] */ DXGI_USAGE Usage,
		/* [annotation][in] */
		__in_opt const DXGI_SHARED_RESOURCE *pSharedResource,
		/* [annotation][out] */
		__out IDXGISurface **ppSurface)
	{
		return m_pReal->CreateSurface(pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(
		/* [annotation][size_is][in] */
		__in_ecount(NumResources) IUnknown *const *ppResources,
		/* [annotation][size_is][out] */
		__out_ecount(NumResources) DXGI_RESIDENCY *pResidencyStatus,
		/* [in] */ UINT NumResources)
	{
		return m_pReal->QueryResourceResidency(ppResources, pResidencyStatus, NumResources);
	}

	virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(
		/* [in] */ INT Priority)
	{
		return m_pReal->SetGPUThreadPriority(Priority);
	}

	virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(
		/* [annotation][retval][out] */
		__out INT *pPriority)
	{
		return m_pReal->GetGPUThreadPriority(pPriority);
	}

	//////////////////////////////
	// implement IDXGIDevice1

	virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(
		/* [in] */ UINT MaxLatency)
	{
		return m_pReal->SetMaximumFrameLatency(MaxLatency);
	}

	virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(
		/* [annotation][out] */
		__out UINT *pMaxLatency)
	{
		return m_pReal->GetMaximumFrameLatency(pMaxLatency);
	}

	//////////////////////////////
	// implement IDXGIDevice2
	virtual HRESULT STDMETHODCALLTYPE OfferResources(
		/* [annotation][in] */
		_In_ UINT NumResources,
		/* [annotation][size_is][in] */
		_In_reads_(NumResources) IDXGIResource *const *ppResources,
		/* [annotation][in] */
		_In_ DXGI_OFFER_RESOURCE_PRIORITY Priority)
	{
		return m_pReal->OfferResources(NumResources, ppResources, Priority);
	}

	virtual HRESULT STDMETHODCALLTYPE ReclaimResources(
		/* [annotation][in] */
		_In_ UINT NumResources,
		/* [annotation][size_is][in] */
		_In_reads_(NumResources) IDXGIResource *const *ppResources,
		/* [annotation][size_is][out] */
		_Out_writes_all_opt_(NumResources) BOOL *pDiscarded)
	{
		return m_pReal->ReclaimResources(NumResources, ppResources, pDiscarded);
	}

	virtual HRESULT STDMETHODCALLTYPE EnqueueSetEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent)
	{
		return m_pReal->EnqueueSetEvent(hEvent);
	}
};
class WrappedIDXGIDevice3 : public IDXGIDevice3, public RefCountDXGIObject
{
	IDXGIDevice3 *m_pReal;
	ID3DDevice *m_pD3DDevice;

public:
	WrappedIDXGIDevice3(IDXGIDevice3 *real, ID3DDevice *d3d)
		: RefCountDXGIObject(real), m_pReal(real), m_pD3DDevice(d3d)
	{
		m_pD3DDevice->AddRef();
	}
	virtual ~WrappedIDXGIDevice3()
	{
		SAFE_RELEASE(m_pReal);
		SAFE_RELEASE(m_pD3DDevice);
	}

	static const int AllocPoolCount = 4;
	ALLOCATE_WITH_WRAPPED_POOL(WrappedIDXGIDevice3, AllocPoolCount);

	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT_CUSTOMQUERY;
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

	ID3DDevice *GetD3DDevice() { return m_pD3DDevice; }
	//////////////////////////////
	// implement IDXGIDevice

	virtual HRESULT STDMETHODCALLTYPE GetAdapter(
		/* [annotation][out] */
		__out IDXGIAdapter **pAdapter)
	{
		HRESULT ret = m_pReal->GetAdapter(pAdapter);
		if (SUCCEEDED(ret))
			*pAdapter = new WrappedIDXGIAdapter(*pAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSurface(
		/* [annotation][in] */
		__in const DXGI_SURFACE_DESC *pDesc,
		/* [in] */ UINT NumSurfaces,
		/* [in] */ DXGI_USAGE Usage,
		/* [annotation][in] */
		__in_opt const DXGI_SHARED_RESOURCE *pSharedResource,
		/* [annotation][out] */
		__out IDXGISurface **ppSurface)
	{
		return m_pReal->CreateSurface(pDesc, NumSurfaces, Usage, pSharedResource, ppSurface);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency(
		/* [annotation][size_is][in] */
		__in_ecount(NumResources) IUnknown *const *ppResources,
		/* [annotation][size_is][out] */
		__out_ecount(NumResources) DXGI_RESIDENCY *pResidencyStatus,
		/* [in] */ UINT NumResources)
	{
		return m_pReal->QueryResourceResidency(ppResources, pResidencyStatus, NumResources);
	}

	virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority(
		/* [in] */ INT Priority)
	{
		return m_pReal->SetGPUThreadPriority(Priority);
	}

	virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority(
		/* [annotation][retval][out] */
		__out INT *pPriority)
	{
		return m_pReal->GetGPUThreadPriority(pPriority);
	}

	//////////////////////////////
	// implement IDXGIDevice1

	virtual HRESULT STDMETHODCALLTYPE SetMaximumFrameLatency(
		/* [in] */ UINT MaxLatency)
	{
		return m_pReal->SetMaximumFrameLatency(MaxLatency);
	}

	virtual HRESULT STDMETHODCALLTYPE GetMaximumFrameLatency(
		/* [annotation][out] */
		__out UINT *pMaxLatency)
	{
		return m_pReal->GetMaximumFrameLatency(pMaxLatency);
	}

	//////////////////////////////
	// implement IDXGIDevice2

	virtual HRESULT STDMETHODCALLTYPE OfferResources(
		/* [annotation][in] */
		_In_ UINT NumResources,
		/* [annotation][size_is][in] */
		_In_reads_(NumResources) IDXGIResource *const *ppResources,
		/* [annotation][in] */
		_In_ DXGI_OFFER_RESOURCE_PRIORITY Priority)
	{
		return m_pReal->OfferResources(NumResources, ppResources, Priority);
	}

	virtual HRESULT STDMETHODCALLTYPE ReclaimResources(
		/* [annotation][in] */
		_In_ UINT NumResources,
		/* [annotation][size_is][in] */
		_In_reads_(NumResources) IDXGIResource *const *ppResources,
		/* [annotation][size_is][out] */
		_Out_writes_all_opt_(NumResources) BOOL *pDiscarded)
	{
		return m_pReal->ReclaimResources(NumResources, ppResources, pDiscarded);
	}

	virtual HRESULT STDMETHODCALLTYPE EnqueueSetEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent)
	{
		return m_pReal->EnqueueSetEvent(hEvent);
	}

	//////////////////////////////
	// implement IDXGIDevice3

	virtual void STDMETHODCALLTYPE Trim() { m_pReal->Trim(); }
};

class WrappedIDXGIAdapter2 : public IDXGIAdapter2, public RefCountDXGIObject
{
	IDXGIAdapter2 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIAdapter2(IDXGIAdapter2 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIAdapter2() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIAdapter

	virtual HRESULT STDMETHODCALLTYPE EnumOutputs(
		/* [in] */ UINT Output,
		/* [annotation][out][in] */
		__out IDXGIOutput **ppOutput)
	{
		return m_pReal->EnumOutputs(Output, ppOutput);
	}

	virtual HRESULT STDMETHODCALLTYPE GetDesc(
		/* [annotation][out] */
		__out DXGI_ADAPTER_DESC *pDesc)
	{
		return m_pReal->GetDesc(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(
		/* [annotation][in] */
		__in REFGUID InterfaceName,
		/* [annotation][out] */
		__out LARGE_INTEGER *pUMDVersion)
	{
		return m_pReal->CheckInterfaceSupport(InterfaceName, pUMDVersion);
	}

	//////////////////////////////
	// implement IDXGIAdapter1

	virtual HRESULT STDMETHODCALLTYPE GetDesc1(
		/* [out] */ DXGI_ADAPTER_DESC1 *pDesc)
	{
		return m_pReal->GetDesc1(pDesc);
	}

	//////////////////////////////
	// implement IDXGIAdapter2

	virtual HRESULT STDMETHODCALLTYPE GetDesc2(
		/* [annotation][out] */
		_Out_ DXGI_ADAPTER_DESC2 *pDesc)
	{
		return m_pReal->GetDesc2(pDesc);
	}
};

class WrappedIDXGIFactory2 : public IDXGIFactory2, public RefCountDXGIObject
{
	IDXGIFactory2 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIFactory2(IDXGIFactory2 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIFactory2() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIFactory

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags)
	{
		return m_pReal->MakeWindowAssociation(WindowHandle, Flags);
	}

	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
		/* [annotation][out] */
		__out HWND *pWindowHandle)
	{
		return m_pReal->GetWindowAssociation(pWindowHandle);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
		/* [annotation][in] */
		__in IUnknown *pDevice,
		/* [annotation][in] */
		__in DXGI_SWAP_CHAIN_DESC *pDesc,
		/* [annotation][out] */
		__out IDXGISwapChain **ppSwapChain)
	{
		return WrappedIDXGIFactory::staticCreateSwapChain(m_pReal, pDevice, pDesc, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
		/* [in] */ HMODULE Module,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->CreateSoftwareAdapter(Module, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	//////////////////////////////
	// implement IDXGIFactory1

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter1 **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters1(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter1(*ppAdapter);
		return ret;
	}

	virtual BOOL STDMETHODCALLTYPE IsCurrent(void) { return m_pReal->IsCurrent(); }
	//////////////////////////////
	// implement IDXGIFactory2

	virtual BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled(void)
	{
		return m_pReal->IsWindowedStereoEnabled();
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ HWND hWnd,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForHwnd(
			m_pReal, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ IUnknown *pWindow,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForCoreWindow(
			m_pReal, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(
		/* [annotation] */
		_In_ HANDLE hResource,
		/* [annotation] */
		_Out_ LUID *pLuid)
	{
		return m_pReal->GetSharedResourceAdapterLuid(hResource, pLuid);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(
		/* [annotation][in] */
		_In_ HWND WindowHandle,
		/* [annotation][in] */
		_In_ UINT wMsg,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterStereoStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterStereoStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterStereoStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(
		/* [annotation][in] */
		_In_ HWND WindowHandle,
		/* [annotation][in] */
		_In_ UINT wMsg,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterOcclusionStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterOcclusionStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Outptr_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForComposition(
			m_pReal, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
	}

	// static functions to share implementation between this and WrappedIDXGIFactory3

	static HRESULT staticCreateSwapChainForHwnd(IDXGIFactory2 *factory, IUnknown *pDevice, HWND hWnd,
		const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
		IDXGIOutput *pRestrictToOutput,
		IDXGISwapChain1 **ppSwapChain);

	static HRESULT staticCreateSwapChainForCoreWindow(IDXGIFactory2 *factory, IUnknown *pDevice,
		IUnknown *pWindow,
		const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		IDXGIOutput *pRestrictToOutput,
		IDXGISwapChain1 **ppSwapChain);

	static HRESULT staticCreateSwapChainForComposition(IDXGIFactory2 *factory, IUnknown *pDevice,
		const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		IDXGIOutput *pRestrictToOutput,
		IDXGISwapChain1 **ppSwapChain);
};

class WrappedIDXGIFactory3 : public IDXGIFactory3, public RefCountDXGIObject
{
	IDXGIFactory3 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIFactory3(IDXGIFactory3 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIFactory3() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIFactory

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags)
	{
		return m_pReal->MakeWindowAssociation(WindowHandle, Flags);
	}

	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
		/* [annotation][out] */
		__out HWND *pWindowHandle)
	{
		return m_pReal->GetWindowAssociation(pWindowHandle);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
		/* [annotation][in] */
		__in IUnknown *pDevice,
		/* [annotation][in] */
		__in DXGI_SWAP_CHAIN_DESC *pDesc,
		/* [annotation][out] */
		__out IDXGISwapChain **ppSwapChain)
	{
		return WrappedIDXGIFactory::staticCreateSwapChain(m_pReal, pDevice, pDesc, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
		/* [in] */ HMODULE Module,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->CreateSoftwareAdapter(Module, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	//////////////////////////////
	// implement IDXGIFactory1

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter1 **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters1(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter1(*ppAdapter);
		return ret;
	}

	virtual BOOL STDMETHODCALLTYPE IsCurrent(void) { return m_pReal->IsCurrent(); }
	//////////////////////////////
	// implement IDXGIFactory2

	virtual BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled(void)
	{
		return m_pReal->IsWindowedStereoEnabled();
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ HWND hWnd,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForHwnd(
			m_pReal, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ IUnknown *pWindow,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForCoreWindow(
			m_pReal, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(
		/* [annotation] */
		_In_ HANDLE hResource,
		/* [annotation] */
		_Out_ LUID *pLuid)
	{
		return m_pReal->GetSharedResourceAdapterLuid(hResource, pLuid);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(
		/* [annotation][in] */
		_In_ HWND WindowHandle,
		/* [annotation][in] */
		_In_ UINT wMsg,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterStereoStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterStereoStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterStereoStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(
		/* [annotation][in] */
		_In_ HWND WindowHandle,
		/* [annotation][in] */
		_In_ UINT wMsg,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterOcclusionStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterOcclusionStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Outptr_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForComposition(
			m_pReal, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
	}

	// static functions to share implementation between this and WrappedIDXGIFactory3

	static HRESULT staticCreateSwapChainForHwnd(
		IDXGIFactory2 *factory,
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ HWND hWnd,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain);

	static HRESULT staticCreateSwapChainForCoreWindow(IDXGIFactory2 *factory,
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ IUnknown *pWindow,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain);

	static HRESULT staticCreateSwapChainForComposition(IDXGIFactory2 *factory,
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Outptr_ IDXGISwapChain1 **ppSwapChain);

	//////////////////////////////
	// implement IDXGIFactory3

	virtual UINT STDMETHODCALLTYPE GetCreationFlags(void) { return m_pReal->GetCreationFlags(); }
};

class WrappedIDXGIAdapter3 : public IDXGIAdapter3, public RefCountDXGIObject
{
	IDXGIAdapter3 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIAdapter3(IDXGIAdapter3 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIAdapter3() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIAdapter

	virtual HRESULT STDMETHODCALLTYPE EnumOutputs(
		/* [in] */ UINT Output,
		/* [annotation][out][in] */
		__out IDXGIOutput **ppOutput)
	{
		return m_pReal->EnumOutputs(Output, ppOutput);
	}

	virtual HRESULT STDMETHODCALLTYPE GetDesc(
		/* [annotation][out] */
		__out DXGI_ADAPTER_DESC *pDesc)
	{
		return m_pReal->GetDesc(pDesc);
	}

	virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport(
		/* [annotation][in] */
		__in REFGUID InterfaceName,
		/* [annotation][out] */
		__out LARGE_INTEGER *pUMDVersion)
	{
		return m_pReal->CheckInterfaceSupport(InterfaceName, pUMDVersion);
	}

	//////////////////////////////
	// implement IDXGIAdapter1

	virtual HRESULT STDMETHODCALLTYPE GetDesc1(
		/* [out] */ DXGI_ADAPTER_DESC1 *pDesc)
	{
		return m_pReal->GetDesc1(pDesc);
	}

	//////////////////////////////
	// implement IDXGIAdapter2

	virtual HRESULT STDMETHODCALLTYPE GetDesc2(
		/* [annotation][out] */
		_Out_ DXGI_ADAPTER_DESC2 *pDesc)
	{
		return m_pReal->GetDesc2(pDesc);
	}

	//////////////////////////////
	// implement IDXGIAdapter3

	virtual HRESULT STDMETHODCALLTYPE RegisterHardwareContentProtectionTeardownStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterHardwareContentProtectionTeardownStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterHardwareContentProtectionTeardownStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterHardwareContentProtectionTeardownStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE QueryVideoMemoryInfo(
		/* [annotation][in] */
		_In_ UINT NodeIndex,
		/* [annotation][in] */
		_In_ DXGI_MEMORY_SEGMENT_GROUP MemorySegmentGroup,
		/* [annotation][out] */
		_Out_ DXGI_QUERY_VIDEO_MEMORY_INFO *pVideoMemoryInfo)
	{
		return m_pReal->QueryVideoMemoryInfo(NodeIndex, MemorySegmentGroup, pVideoMemoryInfo);
	}

	virtual HRESULT STDMETHODCALLTYPE SetVideoMemoryReservation(
		/* [annotation][in] */
		_In_ UINT NodeIndex,
		/* [annotation][in] */
		_In_ DXGI_MEMORY_SEGMENT_GROUP MemorySegmentGroup,
		/* [annotation][in] */
		_In_ UINT64 Reservation)
	{
		return m_pReal->SetVideoMemoryReservation(NodeIndex, MemorySegmentGroup, Reservation);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterVideoMemoryBudgetChangeNotificationEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterVideoMemoryBudgetChangeNotificationEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterVideoMemoryBudgetChangeNotification(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterVideoMemoryBudgetChangeNotification(dwCookie);
	}
};

class WrappedIDXGIFactory4 : public IDXGIFactory4, public RefCountDXGIObject
{
	IDXGIFactory4 *m_pReal;
	unsigned int m_iRefcount;

public:
	WrappedIDXGIFactory4(IDXGIFactory4 *real)
		: RefCountDXGIObject(real), m_pReal(real), m_iRefcount(1)
	{
	}
	virtual ~WrappedIDXGIFactory4() { SAFE_RELEASE(m_pReal); }
	IMPLEMENT_IDXGIOBJECT_WITH_REFCOUNTDXGIOBJECT;

	//////////////////////////////
	// implement IDXGIFactory

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation(HWND WindowHandle, UINT Flags)
	{
		return m_pReal->MakeWindowAssociation(WindowHandle, Flags);
	}

	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation(
		/* [annotation][out] */
		__out HWND *pWindowHandle)
	{
		return m_pReal->GetWindowAssociation(pWindowHandle);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain(
		/* [annotation][in] */
		__in IUnknown *pDevice,
		/* [annotation][in] */
		__in DXGI_SWAP_CHAIN_DESC *pDesc,
		/* [annotation][out] */
		__out IDXGISwapChain **ppSwapChain)
	{
		return WrappedIDXGIFactory::staticCreateSwapChain(m_pReal, pDevice, pDesc, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter(
		/* [in] */ HMODULE Module,
		/* [annotation][out] */
		__out IDXGIAdapter **ppAdapter)
	{
		HRESULT ret = m_pReal->CreateSoftwareAdapter(Module, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
		return ret;
	}

	//////////////////////////////
	// implement IDXGIFactory1

	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1(
		/* [in] */ UINT Adapter,
		/* [annotation][out] */
		__out IDXGIAdapter1 **ppAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapters1(Adapter, ppAdapter);
		if (SUCCEEDED(ret))
			*ppAdapter = new WrappedIDXGIAdapter1(*ppAdapter);
		return ret;
	}

	virtual BOOL STDMETHODCALLTYPE IsCurrent(void) { return m_pReal->IsCurrent(); }
	//////////////////////////////
	// implement IDXGIFactory2

	virtual BOOL STDMETHODCALLTYPE IsWindowedStereoEnabled(void)
	{
		return m_pReal->IsWindowedStereoEnabled();
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForHwnd(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ HWND hWnd,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForHwnd(
			m_pReal, pDevice, hWnd, pDesc, pFullscreenDesc, pRestrictToOutput, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForCoreWindow(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ IUnknown *pWindow,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForCoreWindow(
			m_pReal, pDevice, pWindow, pDesc, pRestrictToOutput, ppSwapChain);
	}

	virtual HRESULT STDMETHODCALLTYPE GetSharedResourceAdapterLuid(
		/* [annotation] */
		_In_ HANDLE hResource,
		/* [annotation] */
		_Out_ LUID *pLuid)
	{
		return m_pReal->GetSharedResourceAdapterLuid(hResource, pLuid);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusWindow(
		/* [annotation][in] */
		_In_ HWND WindowHandle,
		/* [annotation][in] */
		_In_ UINT wMsg,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterStereoStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterStereoStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterStereoStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterStereoStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusWindow(
		/* [annotation][in] */
		_In_ HWND WindowHandle,
		/* [annotation][in] */
		_In_ UINT wMsg,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusWindow(WindowHandle, wMsg, pdwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE RegisterOcclusionStatusEvent(
		/* [annotation][in] */
		_In_ HANDLE hEvent,
		/* [annotation][out] */
		_Out_ DWORD *pdwCookie)
	{
		return m_pReal->RegisterOcclusionStatusEvent(hEvent, pdwCookie);
	}

	virtual void STDMETHODCALLTYPE UnregisterOcclusionStatus(
		/* [annotation][in] */
		_In_ DWORD dwCookie)
	{
		return m_pReal->UnregisterOcclusionStatus(dwCookie);
	}

	virtual HRESULT STDMETHODCALLTYPE CreateSwapChainForComposition(
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Outptr_ IDXGISwapChain1 **ppSwapChain)
	{
		return WrappedIDXGIFactory2::staticCreateSwapChainForComposition(
			m_pReal, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
	}

	// static functions to share implementation between this and WrappedIDXGIFactory3

	static HRESULT staticCreateSwapChainForHwnd(
		IDXGIFactory2 *factory,
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ HWND hWnd,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ const DXGI_SWAP_CHAIN_FULLSCREEN_DESC *pFullscreenDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain);

	static HRESULT staticCreateSwapChainForCoreWindow(IDXGIFactory2 *factory,
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ IUnknown *pWindow,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Out_ IDXGISwapChain1 **ppSwapChain);

	static HRESULT staticCreateSwapChainForComposition(IDXGIFactory2 *factory,
		/* [annotation][in] */
		_In_ IUnknown *pDevice,
		/* [annotation][in] */
		_In_ const DXGI_SWAP_CHAIN_DESC1 *pDesc,
		/* [annotation][in] */
		_In_opt_ IDXGIOutput *pRestrictToOutput,
		/* [annotation][out] */
		_Outptr_ IDXGISwapChain1 **ppSwapChain);

	//////////////////////////////
	// implement IDXGIFactory3

	virtual UINT STDMETHODCALLTYPE GetCreationFlags(void) { return m_pReal->GetCreationFlags(); }
	//////////////////////////////
	// implement IDXGIFactory4

	virtual HRESULT STDMETHODCALLTYPE EnumAdapterByLuid(
		/* [annotation] */
		_In_ LUID AdapterLuid,
		/* [annotation] */
		_In_ REFIID riid,
		/* [annotation] */
		_COM_Outptr_ void **ppvAdapter)
	{
		HRESULT ret = m_pReal->EnumAdapterByLuid(AdapterLuid, riid, ppvAdapter);
		if (SUCCEEDED(ret) && ppvAdapter && *ppvAdapter)
		{
			IDXGIAdapter *adapter = (IDXGIAdapter *)*ppvAdapter;
			*ppvAdapter = (IDXGIAdapter *)(new WrappedIDXGIAdapter(adapter));
		}
		return ret;
	}

	virtual HRESULT STDMETHODCALLTYPE EnumWarpAdapter(
		/* [annotation] */
		_In_ REFIID riid,
		/* [annotation] */
		_COM_Outptr_ void **ppvAdapter)
	{
		HRESULT ret = m_pReal->EnumWarpAdapter(riid, ppvAdapter);
		if (SUCCEEDED(ret) && ppvAdapter && *ppvAdapter)
		{
			IDXGIAdapter *adapter = (IDXGIAdapter *)*ppvAdapter;
			*ppvAdapter = (IDXGIAdapter *)(new WrappedIDXGIAdapter(adapter));
		}
		return ret;
	}
};
