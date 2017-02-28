/*
* file RenderContext.h
*
* author luoxw
* date 2017/01/22
*
*
*/

#pragma once

namespace D3D11
{
	class FRenderContext : public LostCore::IRenderContext
	{
	public:
		FRenderContext(LostCore::EContextID);

		// Í¨¹ý IRenderContext ¼Ì³Ð
		virtual ~FRenderContext() override;
		virtual bool Init(HWND wnd, bool bWindowed, int32 width, int32 height) override;
		virtual void Fini() override;
		virtual bool EnableShadeModel(EShadeModel sm)override;
		virtual EShadeModel GetShadeModel() const override;
		virtual LostCore::FMatrix GetViewProjectMatrix() const override;
		virtual void SetViewProjectMatrix(const LostCore::FMatrix & vp) override;
		virtual LostCore::EContextID GetContextID() const override;
		virtual const char * GetContextString() const override;
		virtual void BeginFrame(float sec) override;
		virtual void EndFrame(float sec) override;

	public:

		INLINE ID3D11Device* GetDevice()
		{
			return Device.GetReference();
		}

		INLINE ID3D11DeviceContext* GetDeviceContext()
		{
			return Context.GetReference();
		}

		INLINE IDXGISwapChain* GetSwapChain()
		{
			return SwapChain.GetReference();
		}

		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}

		void operator delete(void* p)
		{
			_mm_free(p);
		}

		INLINE static ID3D11Device* GetDevice(LostCore::IRenderContext* rc, const char* head)
		{
			FRenderContext* crc = static_cast<FRenderContext*>(rc);
			if (crc == nullptr)
			{
				if (head != nullptr)
				{
					LVERR(head, "null render context");
				}

				return nullptr;
			}

			ID3D11Device* dev = crc->GetDevice();
			if (dev == nullptr && head != nullptr)
			{
				LVERR(head, "null d3d11 device, render context may not be initialized or failed");
			}

			return dev;
		}

		INLINE static ID3D11DeviceContext* GetDeviceContext(LostCore::IRenderContext* rc, const char* head)
		{
			FRenderContext* crc = static_cast<FRenderContext*>(rc);
			if (crc == nullptr)
			{
				if (head != nullptr)
				{
					LVERR(head, "null render context");
				}
				
				return nullptr;
			}

			ID3D11DeviceContext* cxt = crc->GetDeviceContext();
			if (cxt == nullptr && head != nullptr)
			{
				LVERR(head, "null d3d11 device context, render context may not be initialized or failed");
			}

			return cxt;
		}

	private:
		LostCore::EContextID				ContextID;
		TRefCountPtr<ID3D11Device>			Device;
		TRefCountPtr<ID3D11DeviceContext>	Context;
		TRefCountPtr<IDXGISwapChain>		SwapChain;
		LostCore::FMatrix					ViewProject;
		LostCore::EShadeModel				ShadeModel;
	};
}