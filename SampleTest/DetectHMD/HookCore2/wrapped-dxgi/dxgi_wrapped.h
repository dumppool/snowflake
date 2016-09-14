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

#include <dxgi.h>
#include <d3d11.h>
#include <vector>
#include "common.h"

MIDL_INTERFACE("6f15aaf2-d208-4e89-9ab4-489535d34f9c") ID3D11Texture2D;
MIDL_INTERFACE("dc8e63f3-d12b-4952-b47b-5e45026a862d") ID3D11Resource;
MIDL_INTERFACE("db6f6ddb-ac77-4e88-8253-819df9bbf140") ID3D11Device;
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
    if(riid == __uuidof(IUnknown))
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
    if(ret == 0)
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

typedef ID3DDevice *(*D3DDeviceCallback)(IUnknown *dev);

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
  //ALLOCATE_WITH_WRAPPED_POOL(WrappedIDXGIDevice, AllocPoolCount);

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
    if(SUCCEEDED(ret))
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
    if(SUCCEEDED(ret))
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
    if(SUCCEEDED(ret))
      *ppAdapter = new WrappedIDXGIAdapter(*ppAdapter);
    return ret;
  }

  static HRESULT staticCreateSwapChain(IDXGIFactory *factory, IUnknown *pDevice,
                                       DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain);
};

class WrappedIDXGISwapChain : public IDXGISwapChain, public RefCountDXGIObject
{
	IDXGISwapChain *m_pReal;
	ID3DDevice *m_pDevice;
	unsigned int m_iRefcount;

	static std::vector<D3DDeviceCallback> m_D3DCallbacks;

	HWND m_Wnd;

	static const int MAX_NUM_BACKBUFFERS = 4;

	IUnknown *m_pBackBuffers[MAX_NUM_BACKBUFFERS];

	void ReleaseBuffersForResize();
	void WrapBuffersAfterResize();

public:
	WrappedIDXGISwapChain(IDXGISwapChain *real, HWND wnd, ID3DDevice *device);
	virtual ~WrappedIDXGISwapChain();

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
