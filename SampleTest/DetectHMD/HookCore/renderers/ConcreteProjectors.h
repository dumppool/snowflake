#pragma once
#include "Projector.h"
#include "Direct3D11Renderer.h"

namespace lostvr {
	class TextureProjector0 : public BaseTextureProjector
	{
	protected:
		Direct3D11Helper* Renderer;
		ID3D11ShaderResourceView* SRV;

	public:
		TextureProjector0();
		virtual ~TextureProjector0();

		virtual bool IsInitialized(IDXGISwapChain* swapChain) const override;
		virtual bool InitializeRenderer(IDXGISwapChain* swapChain) override;
		virtual Direct3D11Helper * GetRenderer() override;
		virtual bool InitializeTextureSRV() override;
		virtual ID3D11ShaderResourceView* GetTextureSRV() override;

		// Í¨¹ý BaseTextureProjector ¼Ì³Ð
		virtual void DestroyRHI() override;
	};

	class TextureProjector1 : public BaseTextureProjector
	{
	protected:
		Direct3D11Helper* Renderer;
		ID3D11ShaderResourceView* SRV;

		ID3D11Texture2D* SharedTex_Self;
		ID3D11Texture2D* SharedTex_Other;
		IDXGISwapChain* SwapChainRef_Other;

	public:
		TextureProjector1();
		virtual ~TextureProjector1();

		virtual bool IsInitialized(IDXGISwapChain* swapChain) const override;
		virtual bool InitializeRenderer(IDXGISwapChain* swapChain) override;
		virtual Direct3D11Helper* GetRenderer() override;
		virtual bool InitializeTextureSRV() override;
		virtual ID3D11ShaderResourceView* GetTextureSRV() override;

		virtual void DestroyRHI() override;

		virtual bool PrepareSRV() override;
	};
}