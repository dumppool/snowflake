#pragma once

#include "Direct3D11Renderer.h"

namespace lostvr {
	typedef int EnumEyeID;
	const EnumEyeID SEnumEyeL = 0;
	const EnumEyeID SEnumEyeR = 1;

	class TextureProjector
	{
	public:
		TextureProjector();
		~TextureProjector();

		bool IsInitialized(IDXGISwapChain* swapChain) const;
		bool InitializeProjector(
			IDXGISwapChain* swapChain,
			uint32 width, uint32 height,
			const LVMatrix* leftView,
			const LVMatrix* leftProj,
			const LVMatrix* rightView,
			const LVMatrix* rightProj);

		void DestroyRHI();
		bool UpdateTexture();

		ID3D11Texture2D* GetFinalBuffer(EnumEyeID Eye);
		void SetEyePose(EnumEyeID Eye, const LVMatrix& EyeView, const LVMatrix& Proj);

		void* operator new(size_t i)
		{
			return _mm_malloc(i, 16);
		}

		void operator delete(void* p)
		{
			_mm_free(p);
		}

	protected:
		void Update();

		// renderer properties
		ID3D11Texture2D*			BB[2];
		ID3D11RenderTargetView*		RTVs[2];
		ID3D11DepthStencilState*	DSS;
		ID3D11RasterizerState*		RS;

		// scene properties
		ID3D11Buffer*				VB;
		ID3D11Buffer*				IB;
		ID3D11InputLayout*			VL;
		ID3D11VertexShader*			VS;
		ID3D11PixelShader*			PS;
		ID3D11SamplerState*			Sampler;
		ID3D11Buffer*				WVPCB;
		ID3D11ShaderResourceView*	SRV;

		Direct3D11Helper*			Renderer;
		EDirect3D					LastGIVersion;

		UINT RecommendWidth;
		UINT RecommendHeight;

		FrameBufferWVP	EyePose[2];
	};
}