/*
* file RenderContext.h
*
* author luoxw
* date 2017/01/22
*
*
*/

#pragma once

//#include "Texture.h"
//#include "ConstantBuffer.h"
#include "GdiFont.h"

namespace D3D11
{
	class FTexture2D;
	class FConstantBuffer;

	class FRenderContext : public LostCore::IRenderContext
	{
	public:
		FORCEINLINE static FRenderContext* Get()
		{
			static FRenderContext SInst;
			return &SInst;
		}

		FORCEINLINE static TRefCountPtr<ID3D11Device> GetDevice(const char* head)
		{
			FRenderContext* rc = (FRenderContext::Get());
			if (rc == nullptr)
			{
				if (head != nullptr)
				{
					LVERR(head, "null render context");
				}

				return nullptr;
			}

			TRefCountPtr<ID3D11Device> dev = rc->GetDevice();
			if (!dev.IsValid() && head != nullptr)
			{
				LVERR(head, "null d3d11 device, render context may not be initialized or failed");
			}

			return dev;
		}

		FORCEINLINE static TRefCountPtr<ID3D11DeviceContext> GetDeviceContext(const char* head)
		{
			FRenderContext* rc = static_cast<FRenderContext*>(FRenderContext::Get());
			if (rc == nullptr)
			{
				if (head != nullptr)
				{
					LVERR(head, "null render context");
				}

				return nullptr;
			}

			TRefCountPtr<ID3D11DeviceContext> cxt = rc->GetDeviceContext();
			if (!cxt.IsValid() && head != nullptr)
			{
				LVERR(head, "null d3d11 device context, render context may not be initialized or failed");
			}

			return cxt;
		}

	public:
		FRenderContext();

		// Í¨¹ý IRenderContext ¼Ì³Ð
		virtual ~FRenderContext() override;
		virtual bool Initialize(LostCore::EContextID id) override;
		virtual bool InitializeScreen(HWND wnd, bool bWindowed, int32 width, int32 height) override;
		virtual void SetViewProjectMatrix(const LostCore::FFloat4x4 & vp) override;
		virtual void BeginFrame(float sec) override;
		virtual void EndFrame(float sec) override;

	private:
		virtual void Destroy();

	public:
		FORCEINLINE TRefCountPtr<ID3D11Device> GetDevice()
		{
			return Device;
		}

		FORCEINLINE TRefCountPtr<ID3D11DeviceContext> GetDeviceContext()
		{
			return Context;
		}

		FORCEINLINE TRefCountPtr<IDXGISwapChain> GetSwapChain()
		{
			return SwapChain;
		}

		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}

		void operator delete(void* p)
		{
			_mm_free(p);
		}

		void EnableWireframe(bool bEnable)
		{
			bWireframe = bEnable;
		}

	private:
		void ReportLiveObjects();

		LostCore::EContextID					ContextID;
		TRefCountPtr<ID3D11Device>				Device;
		TRefCountPtr<ID3D11DeviceContext>		Context;
		TRefCountPtr<IDXGISwapChain>			SwapChain;
		LostCore::EShadeModel					ShadeModel;
		bool									bWireframe;
		FTexture2D*								RenderTarget;
		FTexture2D*								DepthStencil;
		D3D11_VIEWPORT							Viewport;
		LostCore::FGlobalParameter				Param;
		FConstantBuffer*						GlobalConstantBuffer;

		//FGdiFont* Font;
	};
}