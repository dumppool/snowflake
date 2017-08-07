/*
* file RenderContextBase.h
*
* author luoxw
* date 2017/02/13
*
*
*/

#pragma once

namespace D3D11 {

	static const DXGI_FORMAT SSwapChainFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	static const DXGI_FORMAT SSwapChainTextureFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
	static const DXGI_FORMAT SDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	enum class EDeviceType : uint8
	{
		Undefined,
		DXGI0,
		DXGI1,
		DXGI2,
	};

	extern EReturnCode CreateDevice(
		LostCore::EContextID id,
		TRefCountPtr<ID3D11Device>& ppDevice,
		TRefCountPtr<ID3D11DeviceContext>& ppContext);

	extern EReturnCode CreateSwapChain(
		const TRefCountPtr<ID3D11Device>& d3dDevice,
		HWND wnd,
		bool bWindowed,
		UINT width,
		UINT height,
		TRefCountPtr<IDXGISwapChain>& ppSwapChain);

	extern EReturnCode CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, TRefCountPtr<ID3DBlob>& blob);
	extern EReturnCode GetDesc_DefaultTexture2D(D3D11_TEXTURE2D_DESC* pDesc);
	extern EReturnCode GetDesc_DefaultBuffer(D3D11_BUFFER_DESC* pDesc);
	extern EReturnCode CreateTexture2D(
		D3D11_TEXTURE2D_DESC* pDesc,
		const TRefCountPtr<ID3D11Device>& device,
		TRefCountPtr<ID3D11Texture2D>& ppTex,
		TRefCountPtr<ID3D11ShaderResourceView>& ppSRV,
		TRefCountPtr<ID3D11RenderTargetView>& ppRTV);

	extern EReturnCode CreateShaderResourceView(
		const TRefCountPtr<ID3D11Texture2D>& tex,
		TRefCountPtr<ID3D11ShaderResourceView>& ppSRV);

	extern EReturnCode CreateRenderTargetView(
		const TRefCountPtr<ID3D11Texture2D>& tex,
		TRefCountPtr<ID3D11RenderTargetView>& ppRTV);

	extern EReturnCode CreatePrimitiveVertex(
		const TRefCountPtr<ID3D11Device>& device, 
		const void* buf, uint32 bytes, bool bDynamic, 
		TRefCountPtr<ID3D11Buffer>& vb);

	extern EReturnCode CreatePrimitiveIndex(
		const TRefCountPtr<ID3D11Device>& device, 
		const void* buf, uint32 bytes, bool bDynamic, 
		TRefCountPtr<ID3D11Buffer>& ib);

	extern EReturnCode CreateMesh_Rect(
		const TRefCountPtr<ID3D11Device>& device,
		float width,
		float height,
		UINT vertexSize,
		TRefCountPtr<ID3D11Buffer>& ppVB,
		TRefCountPtr<ID3D11Buffer>& ppIB);

	extern EReturnCode CreateBuffer(
		const TRefCountPtr<ID3D11Device>& device,
		D3D11_BUFFER_DESC* pDesc,
		TRefCountPtr<ID3D11Buffer>& ppBuf);

	extern EReturnCode CreateBlendState_AlphaBlend(
		const TRefCountPtr<ID3D11Device>& device,
		TRefCountPtr<ID3D11BlendState>& ppBS);

	extern EReturnCode CreateBlendState_Add(
		const TRefCountPtr<ID3D11Device>& device,
		TRefCountPtr<ID3D11BlendState>& ppBS);

	extern EReturnCode CreateRasterizer(
		const TRefCountPtr<ID3D11Device>& device,
		TRefCountPtr<ID3D11RasterizerState>& ppRS);
}