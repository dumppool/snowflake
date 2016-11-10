 * file ConcreteProjectors.h
 *
 * author luoxw
 *
 * 
 */
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
		ID3D11Texture2D* SharedTex_Target;
		IDXGISwapChain* SwapChainRef_Target;

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
	
	class TextureProjector9 : public BaseTextureProjector_Direct3D9
	{
	protected:
		Direct3D11Helper* Renderer;
		IDirect3DDevice9* DeviceRef_Target;

		ID3D11ShaderResourceView* SRV;
		ID3D11Texture2D* Tex;

	public:
		TextureProjector9();
		virtual ~TextureProjector9();

		virtual bool IsInitialized_Direct3D9(IDirect3DDevice9* device);
		virtual bool InitializeRenderer_Direct3D9(IDirect3DDevice9* device);
		virtual bool InitializeTextureSRV() override;

		virtual void DestroyRHI() override;
		virtual bool PrepareSRV() override;

		virtual Direct3D11Helper* GetRenderer() override
		{
			return Renderer;
		}

		virtual ID3D11ShaderResourceView* GetTextureSRV() override
		{
			return SRV;
		}
	};
}