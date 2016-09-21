#pragma once

//#include <dxgi.h>

#if 1
class LVRefCountDXGIObject : public IDXGIObject
{
	IDXGIObject* m_pReal;
	unsigned int m_iRefcount;

public:
	LVRefCountDXGIObject(IDXGIObject* real) : m_pReal(real), m_iRefcount(1) {}
	virtual ~LVRefCountDXGIObject() {}
	static bool HandleWrap(REFIID riid, void** ppvObject);
	static HRESULT WrapQueryInterface(IUnknown* real, REFIID riid, void** ppvObject);

public:
	// IUnknown
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void** ppvObject)
	{
		if (riid == __uuidof(IUnknown))
		{
			AddRef();
			*ppvObject = (IUnknown*)(IDXGIObject*)this;
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
		{
			delete this;
		}

		return ret;
	}

public:
	// IDXGIObject
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

class LVWrappedIDXGISwapChain3;

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

	virtual void FirstFrame(LVWrappedIDXGISwapChain3 *swapChain) = 0;

	virtual void NewSwapchainBuffer(IUnknown *backbuffer) = 0;
	virtual void ReleaseSwapchainResources(LVWrappedIDXGISwapChain3 *swapChain) = 0;
	virtual IUnknown *WrapSwapchainBuffer(LVWrappedIDXGISwapChain3 *swap, DXGI_SWAP_CHAIN_DESC *swapDesc,
		UINT buffer, IUnknown *realSurface) = 0;

	virtual HRESULT Present(LVWrappedIDXGISwapChain3 *swapChain, UINT SyncInterval, UINT Flags) = 0;
};

typedef ID3DDevice *(*D3DDeviceCallback)(IUnknown *dev);

template <typename NestedType>
class LVWrappedDXGIInterface : public RefCountDXGIObject,
	public IDXGIKeyedMutex,
	public IDXGISurface2,
	public IDXGIResource1
{
public:
	ID3DDevice *pDevice;
	NestedType *pWrapped;

	LVWrappedDXGIInterface(NestedType* wrapped, ID3DDevice* device) :
		LVRefCountDXGIObject(NULL), pDevice(device), pWrapped(wrapped)
	{
		pWrapped->AddRef();
		pDevice->AddRef();
	}

	virtual ~LVWrappedDXGIInterface()
	{
		pWrapped->Release();
		pDevice->Release();
	}

	// IUnknown
	ULONG STDMETHODCALLTYPE AddRef() { return LVRefCountDXGIObject::AddRef(); }
	ULONG STDMETHODCALLTYPE Release() { return LVRefCountDXGIObject::Release(); }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
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

		return pWrapped->QueryInterface(riid, ppvObject);
	}

	// IDXGIObject
	HRESULT STDMETHODCALLTYPE SetPrivateData(REFGUID Name, UINT DataSize, const void *pData)
	{
		return pWrapped->SetPrivateData(Name, DataSize, pData);
	}

	HRESULT STDMETHODCALLTYPE SetPrivateDataInterface(REFGUID Name, const IUnknown *pUnknown)
	{
		return pWrapped->SetPrivateDataInterface(Name, pUnknown);
	}

	HRESULT STDMETHODCALLTYPE GetPrivateData(REFGUID Name, UINT *pDataSize, void *pData)
	{
		return pWrapped->GetPrivateData(Name, pDataSize, pData);
	}

	// this should only be called for adapters, devices, factories etc
	// so we pass it onto the device
	HRESULT STDMETHODCALLTYPE GetParent(REFIID riid, void **ppParent)
	{
		return pDevice->QueryInterface(riid, ppParent);
	}

	//////////////////////////////
	// Implement IDXGIDeviceSubObject

	// same as GetParent
	HRESULT STDMETHODCALLTYPE GetDevice(REFIID riid, void **ppDevice)
	{
		return pDevice->QueryInterface(riid, ppDevice);
	}

	//////////////////////////////
	// Implement IDXGIKeyedMutex
	HRESULT STDMETHODCALLTYPE AcquireSync(UINT64 Key, DWORD dwMilliseconds)
	{
		// temporarily get the real interface
		IDXGIKeyedMutex *mutex = NULL;
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&mutex);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&mutex);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource), (void **)&res);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface), (void **)&surf);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface), (void **)&surf);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface), (void **)&surf);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface1), (void **)&surf);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGISurface1), (void **)&surf);
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
		HRESULT hr = pWrapped->GetReal()->QueryInterface(__uuidof(IDXGIResource1), (void **)&res);
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

class LVWrappedDXGISwapChain3 : public IDXGISwapChain3, public LVRefCountDXGIObject
{
	IDXGISwapChain* m_pReal;
	IDXGISwapChain1* m_pReal1;
	IDXGISwapChain2* m_pReal2;
	IDXGISwapChain3* m_pReal3;
	ID3DDevice* m_pDevice;
	unsigned int m_iRefcount;
	HWND m_Wnd;

	static const int MAX_NUM_BACKBUFFERS = 4;
	IUnknown *m_pBackBuffers[MAX_NUM_BACKBUFFERS];

public:
	LVWrappedDXGISwapChain3(IDXGISwapChain* real, HWND wnd, ID3DDevice* dev);
	virtual ~LVWrappedDXGISwapChain3();

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
#endif
