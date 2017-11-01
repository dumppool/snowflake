/*
* file TextureInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

namespace LostCore
{
	class ITexture
	{
	public:
		virtual ~ITexture() {}

		virtual void CommitShaderResource() = 0;

		virtual int32 GetWidth() const = 0;
		virtual int32 GetHeight() const = 0;

		virtual void SetShaderResourceSlot(int32 slot) = 0;
		virtual void SetRenderTargetSlot(int32 slot) = 0;

		//virtual bool Construct(
		//	IRenderContext* rc,
		//	uint32 width, 
		//	uint32 height, 
		//	uint32 format, 
		//	bool bIsDepthStencil, 
		//	bool bIsRenderTarget, 
		//	bool bIsShaderResource,
		//	bool bIsWritable) = 0;

		//virtual bool IsRenderTarget() const = 0;
		//virtual void BindShaderResource(IRenderContext* rc, uint32 slot) = 0;
		//virtual void * GetShaderResourceRHI() = 0;

		//virtual bool IsShaderResource() const = 0;
		//virtual void BindRenderTarget(IRenderContext* rc, uint32 slot) = 0;
		//virtual void * GetRenderTargetRHI() = 0;

		//virtual bool IsDepthStencil() const = 0;
		//virtual void BindDepthStencil(IRenderContext* rc, uint32 slot) = 0;
		//virtual void * GetDepthStencilRHI() = 0;

		//virtual bool IsWritable() const = 0;
		//virtual bool IsReadable() const = 0;
	};

	typedef shared_ptr<ITexture> ITexturePtr;
	typedef weak_ptr<ITexture> ITextureWeakPtr;
}