#pragma once



class RenderContext_D3D11
{
public:
	RenderContext_D3D11();
	virtual ~RenderContext_D3D11();

	bool Initialize(HINSTANCE hInst, HWND hWnd);
	bool InitializeRHI();
	void Draw(float DeltaSeconds);

	static RenderContext_D3D11* Get();

public:
	bool CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob** blob);

private:
	void DrawRect();

private:
	static const uint32 SConstFrameCount = 2;

	uint32						FrameIndex;
	bool						bInited;

	ID3D11Device*				Device;
	ID3D11DeviceContext*		Context;
	ID3D11RenderTargetView*		BackBuffer;
	ID3D11DepthStencilView*		DepthStencil;
	ID3D11DepthStencilState*	DepthStencilState;
	ID3D11RasterizerState*		Rasterizer;

	IDXGIAdapter*				Adapter;
	IDXGISwapChain*				SwapChain;

	ID3D11Buffer*				FrameBuffer_WVP;

	//ID3D11Buffer*				MeshVB;
	//ID3D11Buffer*				MeshIB;
	ID3D11Buffer*				RectVB;
	ID3D11Buffer*				RectIB;
	ID3D11InputLayout*			VertexLayout;

	ID3D11VertexShader*			VS;
	ID3D11PixelShader*			PS;

	ID3D11SamplerState*			Sampler;
	ID3D11Texture2D*			Tex[SConstFrameCount];
	ID3D11ShaderResourceView*	SRV[SConstFrameCount];
};