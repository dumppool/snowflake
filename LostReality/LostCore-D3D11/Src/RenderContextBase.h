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
	enum class EDeviceType : uint8
	{
		Undefined,
		DXGI0,
		DXGI1,
		DXGI2,
	};

	extern EReturnCode CreateDevice(
		LostCore::EContextID id,
		ID3D11Device** ppDevice,
		ID3D11DeviceContext** ppContext);

	extern EReturnCode CreateSwapChain(
		ID3D11Device* d3dDevice,
		HWND wnd,
		bool bWindowed,
		UINT width,
		UINT height,
		IDXGISwapChain** ppSwapChain);

	extern EReturnCode CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob ** blob);
	extern EReturnCode GetDesc_DefaultTexture2D(D3D11_TEXTURE2D_DESC* pDesc);
	extern EReturnCode GetDesc_DefaultBuffer(D3D11_BUFFER_DESC* pDesc);
	extern EReturnCode CreateTexture2D(
		D3D11_TEXTURE2D_DESC* pDesc,
		ID3D11Device* device,
		ID3D11Texture2D** ppTex,
		ID3D11ShaderResourceView** ppSRV,
		ID3D11RenderTargetView** ppRTV);

	extern EReturnCode CreateShaderResourceView(
		ID3D11Texture2D* tex,
		ID3D11ShaderResourceView** ppSRV);

	extern EReturnCode CreateRenderTargetView(
		ID3D11Texture2D* tex,
		ID3D11RenderTargetView** ppRTV);

	extern EReturnCode CreatePrimitiveVertex(ID3D11Device* device, const void* buf, uint32 bytes, bool bDynamic, ID3D11Buffer** vb);
	extern EReturnCode CreatePrimitiveIndex(ID3D11Device* device, const void* buf, uint32 bytes, bool bDynamic, ID3D11Buffer** ib);
	extern EReturnCode CreateMesh_Rect(
		ID3D11Device* device,
		float width,
		float height,
		UINT vertexSize,
		ID3D11Buffer** ppVB,
		ID3D11Buffer** ppIB);

	extern EReturnCode CreateBuffer(
		ID3D11Device* device,
		D3D11_BUFFER_DESC* pDesc,
		ID3D11Buffer** ppBuf);

	extern EReturnCode CreateBlendState_AlphaBlend(
		ID3D11Device* device,
		ID3D11BlendState** ppBS);

	extern EReturnCode CreateBlendState_Add(
		ID3D11Device* device,
		ID3D11BlendState** ppBS);

	extern EReturnCode CreateRasterizer(ID3D11Device* device, ID3D11RasterizerState** ppRS);
}