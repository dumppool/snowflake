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

#include "ConstantBuffer.h"
#include "InstancingData.h"
#include "PrimitiveGroup.h"
#include "Texture.h"
#include "GdiFont.h"

namespace D3D11
{
	class IPipeline;

	class IUpdate
	{
	public:
		virtual void Update() = 0;
	};

	class FContextCommand
	{
	public:
		typedef function<void(void*)> FBody;

		FContextCommand() {}
		FContextCommand(void* p, const FBody& body)
			: This(p), Body(body)
		{
		}

		void Exec()
		{
			Body(This);
		}

		bool operator<(const FContextCommand& rhs) const
		{
			return This < rhs.This;
		}

		bool operator==(const FContextCommand& rhs) const
		{
			return This == rhs.This;
		}

	private:
		void* This;
		FBody Body;
	};

	class FRenderContext : public LostCore::IRenderContext
	{
	public:

		FORCEINLINE static FRenderContext*& Get()
		{
			static FRenderContext* SPtr = nullptr;
			return SPtr;
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
		virtual bool Initialize() override;
		virtual void Tick() override;
		virtual void Destroy() override;
		virtual void InitializeDevice(LostCore::EContextID id, HWND wnd, bool bWindowed, int32 width, int32 height) override;
		virtual void SetViewProjectMatrix(const LostCore::FFloat4x4 & vp) override;
		virtual void FirstCommit() override;
		virtual void FinishCommit() override;

	private:
		void ExecInitializeDevice(LostCore::EContextID id, HWND wnd, bool bWindowed, int32 width, int32 height);

		void BeginFrame();
		void RenderFrame();
		void EndFrame();

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

		void CommitPrimitiveGroup(FPrimitiveGroup* pg);
		void CommitBuffer(FConstantBuffer* buf);
		void CommitShaderResource(FTexture2D* srv);
		void CommitInstancingData(FInstancingData* buf);

		thread::id GetThreadId() const;
		bool InRenderThread() const;

		void PushCommand(const FContextCommand& cmd);
		void DeallocPrimitiveGroup(LostCore::IPrimitive* pg);
		void DeallocInstancingData(LostCore::IInstancingData* data);
		void DeallocConstantBuffer(LostCore::IConstantBuffer* cb);
		void DeallocGdiFont(LostCore::IFont* font);
		void FlushDeallocating();

		void AddUpdateCommand(const FContextCommand& obj);
		void RemoveUpdateCommand(const FContextCommand& obj);

	private:
		void ReportLiveObjects();
		void InitializeStateObjects();
		void DestroyStateObjects();
		void InitializePipelines();
		void DestroyPipelines();

		LostCore::EContextID					ContextID;
		TRefCountPtr<ID3D11Device>				Device;
		TRefCountPtr<ID3D11DeviceContext>		Context;
		TRefCountPtr<IDXGISwapChain>			SwapChain;
		FTexture2D*								RenderTarget;
		FTexture2D*								DepthStencil;
		D3D11_VIEWPORT							Viewport;
		LostCore::FGlobalParameter				Param;
		FConstantBuffer*						GlobalConstantBuffer;

		vector<FContextCommand>					UpdateGroup;

		IPipeline*								ActivedPipeline;
		map<EPipeline, IPipeline*>				Pipelines;

		vector<LostCore::IPrimitive*>			DeallocatingPrimitiveGroups;
		vector<LostCore::IInstancingData*>		DeallocatingInstancingDatas;
		vector<LostCore::IConstantBuffer*>		DeallocatingConstantBuffers;
		vector<LostCore::IFont*>				DeallocatingFonts;

		function<void()>						Initializer;
		LostCore::TSynchronizer<LostCore::FCommandQueue<FContextCommand>> Commands;
		LostCore::FTickThread*					Thread;

		//FGdiFont* Font;
	};
}