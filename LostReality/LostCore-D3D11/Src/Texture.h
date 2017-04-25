/*
* file Texture.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

namespace D3D11
{
	class FTexture2D : public LostCore::ITexture
	{
	public:
		FTexture2D();
		virtual ~FTexture2D() override;

		// Í¨¹ý ITexture ¼Ì³Ð

		virtual bool Construct(
			LostCore::IRenderContext* rc,
			uint32 width, 
			uint32 height, 
			uint32 format, 
			bool bIsDepthStencil, 
			bool bIsRenderTarget, 
			bool bIsShaderResource,
			bool bIsWritable,
			void* initialData, 
			uint32 initialPitch);

		virtual int32 GetWidth() const override
		{
			return Width;
		}

		virtual int32 GetHeight() const override
		{
			return Height;
		}

		virtual bool ConstructFromSwapChain(const TRefCountPtr<IDXGISwapChain>& swapChain);

	public:
		bool IsRenderTarget() const;
		TRefCountPtr<ID3D11ShaderResourceView> GetShaderResourceRHI();

		bool IsShaderResource() const;
		TRefCountPtr<ID3D11RenderTargetView> GetRenderTargetRHI();

		bool IsDepthStencil() const;
		TRefCountPtr<ID3D11DepthStencilView> GetDepthStencilRHI();

		TRefCountPtr<ID3D11SamplerState> GetSamplerRHI();

		bool IsWritable() const;
		bool IsReadable() const;

	private:
		TRefCountPtr<ID3D11Texture2D>			Texture;
		TRefCountPtr<ID3D11ShaderResourceView>	ShaderResource;
		TRefCountPtr<ID3D11RenderTargetView>	RenderTarget;
		TRefCountPtr<ID3D11DepthStencilView>	DepthStencil;
		TRefCountPtr<ID3D11SamplerState>		Sampler;

		uint32 BindFlags;
		uint32 AccessFlags;

		int32 Width;
		int32 Height;
	};
}