#include "stdafx.h"
#include "RenderContext_D3D11.h"

#include "Shader\RGB_PixelShader.h"
#include "Shader\RGB_VertexShader.h"

// Temporarily use this hard coded size.
const uint32 SViewportWidth = 1920;
const uint32 SViewportHeight = 1080;

RenderContext_D3D11* RenderContext_D3D11::Get()
{
	static RenderContext_D3D11 Inst;
	return &Inst;
}

RenderContext_D3D11::RenderContext_D3D11() : FrameIndex(0)
, bInited(false)
, Device(nullptr)
, Context(nullptr)
, BackBuffer(nullptr)
, DepthStencil(nullptr)
, DepthStencilState(nullptr)
, Rasterizer(nullptr)
, SwapChain(nullptr)
, Adapter(nullptr)
{

}

RenderContext_D3D11::~RenderContext_D3D11()
{
	// do sth...
}

bool RenderContext_D3D11::Initialize(HINSTANCE hInst, HWND hWnd)
{
	//assert(!bInited && "...");

	// Select most powerful adapter
	{
		IDXGIFactory* DXGIFactory;
		if (!SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&DXGIFactory))))
		{
			assert(0 && "failed to create dxgi factory.");
			return false;
		}
		 
		uint32 AdapterIndex = 0;
		IDXGIAdapter* LocalAdapter = nullptr;
		int32 MaxMem = 0;
		while (DXGIFactory->EnumAdapters(AdapterIndex, &LocalAdapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC Desc;
			LocalAdapter->GetDesc(&Desc);
			int32 Mem = (int32)Desc.DedicatedVideoMemory / 1024u / 1024u;
			if (Mem > MaxMem)
			{
				Adapter = LocalAdapter;
				MaxMem = Mem;
			}

			++AdapterIndex;
		}
	}

	assert(Adapter && "failed to get adapter.");

	// Create SwapChain
	{
		DXGI_SWAP_CHAIN_DESC Desc{ 0 };
		Desc.BufferCount = 2;
		Desc.BufferDesc.Width = SViewportWidth;
		Desc.BufferDesc.Height = SViewportHeight;
		Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.BufferDesc.RefreshRate.Numerator = 0;
		Desc.BufferDesc.RefreshRate.Denominator = 1;
		Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.OutputWindow = hWnd;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		Desc.Flags = 0;
		Desc.Windowed = true; 
		HRESULT Res = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, nullptr, 0,
			D3D11_SDK_VERSION, &Desc, &SwapChain, &Device, nullptr, &Context);
		VERIFY_HRESULT(Res, return false, "failed to create device and swap chain.");
	}

	// Create back buffer
	{
		ID3D11Texture2D* ColorBuffer = nullptr;
		HRESULT Res = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&ColorBuffer);
		VERIFY_HRESULT(Res, return false, "failed to get back buffer 0 from swap chain.");

		Res = Device->CreateRenderTargetView(ColorBuffer, nullptr, &BackBuffer);
		VERIFY_HRESULT(Res, return false, "failed to create render target view from swap chain color buffer.");
		ColorBuffer->Release();
	}

	// Create depth
	{
		D3D11_TEXTURE2D_DESC Desc{ 0 };
		Desc.Width = SViewportWidth;
		Desc.Height = SViewportHeight;
		Desc.ArraySize = 1;
		Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		Desc.CPUAccessFlags = 0;
		Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		Desc.MipLevels = 1;
		Desc.MiscFlags = 0;
		Desc.SampleDesc.Count = 1;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		ID3D11Texture2D* DepthTex = nullptr;
		HRESULT Res = Device->CreateTexture2D(&Desc, nullptr, &DepthTex);
		VERIFY_HRESULT(Res, return false, "failed to create texture 2d for depth stencil.");

		Res = Device->CreateDepthStencilView(DepthTex, nullptr, &DepthStencil);
		VERIFY_HRESULT(Res, return false, "failed to create depth stencil view.");

		D3D11_DEPTH_STENCIL_DESC Desc2{ 0 };
		Desc2.DepthEnable = TRUE;
		Desc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		Desc2.DepthFunc = D3D11_COMPARISON_LESS;
		Desc2.StencilEnable = FALSE;
		Res = Device->CreateDepthStencilState(&Desc2, &DepthStencilState);
		VERIFY_HRESULT(Res, return false, "failed to create depth stencil state.");
	}

	// Create rasterizer
	{
		D3D11_RASTERIZER_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.AntialiasedLineEnable = FALSE;
		Desc.CullMode = D3D11_CULL_NONE;
		Desc.DepthBias = 0;
		Desc.DepthBiasClamp = 0.0f;
		Desc.DepthClipEnable = TRUE;
		Desc.FillMode = D3D11_FILL_SOLID;
		Desc.FrontCounterClockwise = FALSE;
		Desc.MultisampleEnable = FALSE;
		Desc.ScissorEnable = FALSE;
		Desc.SlopeScaledDepthBias = 0.0f;
		HRESULT Res = Device->CreateRasterizerState(&Desc, &Rasterizer);
		VERIFY_HRESULT(Res, return false, "failed to create rasterizer.");
	}

	return true;
}

bool RenderContext_D3D11::InitializeRHI()
{
	HRESULT Res;

	// Create Mesh
	{
		const MeshVertex RectVertices[4] =
		{
			{ LVVec3(-1.0f, -1.0f, 0.0f), LVVec2(0.0f, 1.0f) },			// bottom left
			{ LVVec3(-1.0f, 1.0f, 0.0f), LVVec2(0.0f, 0.0f) },			// top left
			{ LVVec3(1.0f, 1.0f, 0.0f), LVVec2(1.0f, 0.0f) },			// top right
			{ LVVec3(1.0f, -1.0f, 0.0f), LVVec2(1.0f, 1.0f) },			// bottom right
		};
		const uint16 RectIndices[6] = { 0, 1, 2, 3, 0, 2 };

		// Vertex Buffer
		{
			uint32 Sz = sizeof(MeshVertex) * ARRAYSIZE(RectVertices);
			D3D11_BUFFER_DESC Desc{ Sz, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &RectVertices[0], 0, 0 };
			Res = Device->CreateBuffer(&Desc, &Data, &RectVB);
			VERIFY_HRESULT(Res, return false, "failed to create rect vertex buffer.");
		}

		// Index Buffer
		{
			uint32 Sz = (int32)sizeof(uint16) * ARRAYSIZE(RectIndices);
			D3D11_BUFFER_DESC Desc{ Sz, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &RectIndices[0], 0, 0 };
			Res = Device->CreateBuffer(&Desc, &Data, &RectIB);
			VERIFY_HRESULT(Res, return false, "failed to create rect index buffer.");
		}
	}

	// Create frame buffer
	{
		D3D11_BUFFER_DESC Desc{ 0 };
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.ByteWidth = sizeof(FrameBufferWVP);
		Desc.CPUAccessFlags = 0;
		Res = Device->CreateBuffer(&Desc, nullptr, &FrameBuffer_WVP);
		VERIFY_HRESULT(Res, return false, "failed to create frame buffer wvp.");
	}

	// Create shader
	{
		// VS
		ID3DBlob* VSBlob = nullptr;
		if (!CompileShader(L"vrplayer.hlsl", "vs_main", "vs_5_0", &VSBlob) || !VSBlob)
		{
			return false;
		}
		else
		{
			Res = Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &VS);
			VERIFY_HRESULT(Res, return false, "failed to create vertex shader.");

		}

		// PS
		ID3DBlob* PSBlob = nullptr;
		if (!CompileShader(L"vrplayer.hlsl", "ps_main", "ps_5_0", &PSBlob) || !PSBlob)
		{
			return false;
		}
		else
		{
			Res = Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &PS);
			VERIFY_HRESULT(Res, return false, "failed to create pixel shader.");
		}

		// Vertex Layout
		D3D11_INPUT_ELEMENT_DESC Desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, offsetof(MeshVertex, Texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Res = Device->CreateInputLayout(Desc, ARRAYSIZE(Desc), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &VertexLayout);
		VERIFY_HRESULT(Res, return false, "failed to create vertex layout.");
	}

	// Create texture 2D & shader resource view
	{
		D3D11_TEXTURE2D_DESC Desc{ 0 };
		Desc.Width = SViewportWidth;
		Desc.Height = SViewportHeight;
		Desc.ArraySize = 1;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MipLevels = 1;
		Desc.MiscFlags = 0;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.Format = DXGI_FORMAT_R8_UNORM;

		D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
		ZeroMemory(&DescSRV, sizeof(DescSRV));
		DescSRV.Format = Desc.Format;
		DescSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		DescSRV.Texture2D.MipLevels = 1;

		for (int32 i = 0; i < ARRAYSIZE(Tex); ++i)
		{
			Res = Device->CreateTexture2D(&Desc, nullptr, &Tex[i]);
			VERIFY_HRESULT(Res, return false, "failed to create texture 2D.");

			Res = Device->CreateShaderResourceView(Tex[i], &DescSRV, &SRV[i]);
			VERIFY_HRESULT(Res, return false, "failed to create shader resource view.");
		}
	}

	// Create sampler
	{
		D3D11_SAMPLER_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.MipLODBias = 0.0f;
		Desc.MaxAnisotropy = 1;
		Desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		Desc.BorderColor[0] = 0.0f;
		Desc.BorderColor[1] = 0.0f;
		Desc.BorderColor[2] = 0.0f;
		Desc.BorderColor[3] = 0.0f;
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Res = Device->CreateSamplerState(&Desc, &Sampler);
		VERIFY_HRESULT(Res, return false, "failed to create sampler.");
	}

	return true;
}

void RenderContext_D3D11::Draw(float DeltaSeconds)
{
	assert(Context && "Ptr to render context is null!");

	FrameIndex = (FrameIndex + 1) % SConstFrameCount;

	D3D11_VIEWPORT Viewport{ 0.0f, 0.0f, (float)SViewportWidth, (float)SViewportHeight, 0.0f, 1.0f };

	Context->RSSetState(Rasterizer);
	Context->OMSetRenderTargets(1, &BackBuffer, nullptr);
	Context->RSSetViewports(1, &Viewport);

	Context->ClearRenderTargetView(BackBuffer, DirectX::Colors::Gray);
	Context->ClearDepthStencilView(DepthStencil, D3D11_CLEAR_DEPTH, 1, 0);

	DrawRect();

	SwapChain->Present(0, 0);
}

void RenderContext_D3D11::DrawRect()
{
	LVMatrix ModelView;
	uint32 Stride = sizeof(MeshVertex);
	uint32 Offset = 0;

	// Setup input assembler
	Context->IASetIndexBuffer(RectIB, DXGI_FORMAT_R16_UINT, 0);
	Context->IASetVertexBuffers(0, 1, &RectVB, &Stride, &Offset);
	Context->IASetInputLayout(VertexLayout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setup vs shader & parameters
	Context->VSSetShader(VS, nullptr, 0);
	Context->VSSetConstantBuffers(0, 1, &FrameBuffer_WVP);

	// Setup ps shader & parameters
	Context->PSSetShader(PS, nullptr, 0);
	Context->PSSetSamplers(0, 1, &Sampler);
	Context->PSSetShaderResources(0, 1, &SRV[FrameIndex]);

	// Update parameter
	FrameBufferWVP WVP;
	WVP.texmat = DirectX::XMMatrixIdentity();
	WVP.modelview = DirectX::XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	WVP.Projection = DirectX::XMMatrixPerspectiveLH((float)SViewportWidth, (float)SViewportHeight, 0.1f, 1000.0f);
	Context->UpdateSubresource(FrameBuffer_WVP, 0, nullptr, &WVP, 0, 0);

	// Draw call
	Context->DrawIndexed(6, 0, 0);
}

bool RenderContext_D3D11::CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob** blob)
{
	if (!file || !entry || !target || !blob)
		return false;

	ID3DBlob* ShaderBlob = nullptr;
	ID3DBlob* ErrorBlob = nullptr;
	UINT Flag1 = 0;
	UINT Flag2 = 0;
	Flag1 |= D3DCOMPILE_DEBUG;
	HRESULT Res = D3DCompileFromFile(file, NULL, NULL, entry, target, Flag1, Flag2, &ShaderBlob, &ErrorBlob);
	if (FAILED(Res))
	{
		if (ErrorBlob)
		{
			OutputDebugStringA((char*)ErrorBlob->GetBufferPointer());
			ErrorBlob->Release();
		}

		if (ShaderBlob)
		{
			ShaderBlob->Release();
		}

		return false;
	}
	else
	{
		*blob = ShaderBlob;
		return true;
	}
}

bool RenderContext_D3D11::Initialize_Test(HINSTANCE hInst, HWND hWnd)
{
	HRESULT Res;
	IDXGIFactory1* DXGIFactory;
	// Select most powerful adapter
	{
		if (!SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&DXGIFactory))))
		{
			assert(0 && "failed to create dxgi factory.");
			return false;
		}

		//uint32 AdapterIndex = 0;
		//IDXGIAdapter* LocalAdapter = nullptr;
		//int32 MaxMem = 0;
		//while (DXGIFactory->EnumAdapters(AdapterIndex, &LocalAdapter) != DXGI_ERROR_NOT_FOUND)
		//{
		//	DXGI_ADAPTER_DESC Desc;
		//	LocalAdapter->GetDesc(&Desc);
		//	int32 Mem = (int32)Desc.DedicatedVideoMemory / 1024u / 1024u;
		//	if (Mem > MaxMem)
		//	{
		//		Adapter = LocalAdapter;
		//		MaxMem = Mem;
		//	}

		//	++AdapterIndex;
		//}
	}

	//assert(Adapter && "failed to get adapter.");

	// Create SwapChain
	{
		//HRESULT Res = D3D11CreateDeviceAndSwapChain(Adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, nullptr, 0,
		//	D3D11_SDK_VERSION, &Desc, &SwapChain, &Device, nullptr, &Context);
		//VERIFY_HRESULT(Res, return false, "failed to create device and swap chain.");

		Res = DXGIFactory->EnumAdapters1(0, &Adapter1);
		VERIFY_HRESULT(Res, return false, "failed to enumerate adapter.");

		D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_UNKNOWN;
		D3D_FEATURE_LEVEL FeatureLevels[] {
			D3D_FEATURE_LEVEL_11_0,
		};
		D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_1;

		UINT CreationFlag = D3D11_CREATE_DEVICE_DEBUG;
		Res = D3D11CreateDevice(Adapter1, D3D_DRIVER_TYPE_UNKNOWN, nullptr, CreationFlag, &FeatureLevels[0],
			ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION, &Device, &FeatureLevel, &Context);
		VERIFY_HRESULT(Res, return false, "failed to create device & context.");

		DXGI_SWAP_CHAIN_DESC Desc{ 0 };
		Desc.BufferCount = 2;
		Desc.BufferDesc.Width = SViewportWidth;
		Desc.BufferDesc.Height = SViewportHeight;
		Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		Desc.BufferDesc.RefreshRate.Numerator = 0;
		Desc.BufferDesc.RefreshRate.Denominator = 0;
		Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		Desc.OutputWindow = hWnd;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		Desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		Desc.Flags = 2;
		Desc.Windowed = true;
		Res = DXGIFactory->CreateSwapChain(Device, &Desc, &SwapChain);
		VERIFY_HRESULT(Res, return false, "failed to create swap chain.");
	}

	// Create back buffer
	{
		ID3D11Texture2D* ColorBuffer = nullptr;
		Res = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&ColorBuffer);
		VERIFY_HRESULT(Res, return false, "failed to get back buffer 0 from swap chain.");

		Res = Device->CreateRenderTargetView(ColorBuffer, nullptr, &BackBuffer);
		VERIFY_HRESULT(Res, return false, "failed to create render target view from swap chain color buffer.");
		ColorBuffer->Release();
	}

	// Create depth
	{
		D3D11_TEXTURE2D_DESC Desc{ 0 };
		Desc.Width = SViewportWidth;
		Desc.Height = SViewportHeight;
		Desc.ArraySize = 1;
		Desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		Desc.CPUAccessFlags = 0;
		Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		Desc.MipLevels = 1;
		Desc.MiscFlags = 0;
		Desc.SampleDesc.Count = 1;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		ID3D11Texture2D* DepthTex = nullptr;
		Res = Device->CreateTexture2D(&Desc, nullptr, &DepthTex);
		VERIFY_HRESULT(Res, return false, "failed to create texture 2d for depth stencil.");

		Res = Device->CreateDepthStencilView(DepthTex, nullptr, &DepthStencil);
		VERIFY_HRESULT(Res, return false, "failed to create depth stencil view.");

		D3D11_DEPTH_STENCIL_DESC Desc2{ 0 };
		Desc2.DepthEnable = TRUE;
		Desc2.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		Desc2.DepthFunc = D3D11_COMPARISON_LESS;
		Desc2.StencilEnable = FALSE;
		Res = Device->CreateDepthStencilState(&Desc2, &DepthStencilState);
		VERIFY_HRESULT(Res, return false, "failed to create depth stencil state.");
	}

	// Create rasterizer
	{
		D3D11_RASTERIZER_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.FillMode = D3D11_FILL_SOLID;
		Desc.CullMode = D3D11_CULL_NONE;
		Desc.FrontCounterClockwise = FALSE;
		Desc.DepthBias = 0;
		Desc.DepthBiasClamp = 0.0f;
		Desc.SlopeScaledDepthBias = 0.0f;
		Desc.ScissorEnable = FALSE;
		Desc.DepthClipEnable = TRUE;
		Desc.MultisampleEnable = FALSE;
		Desc.AntialiasedLineEnable = FALSE;
		HRESULT Res = Device->CreateRasterizerState(&Desc, &Rasterizer);
		VERIFY_HRESULT(Res, return false, "failed to create rasterizer.");
	}

	// Create Mesh
	{
		const MeshVertex RectVertices[4] =
		{
			{ LVVec3(-0.5f, -0.9f, 10.0f), LVVec2(0.0f, 1.0f) },			// bottom left
			{ LVVec3(-0.5f, 0.9f, 10.0f), LVVec2(0.0f, 0.0f) },			// top left
			{ LVVec3(0.9f, 0.5f, 10.0f), LVVec2(1.0f, 0.0f) },			// top right
			{ LVVec3(0.5f, -0.5f, 10.0f), LVVec2(1.0f, 1.0f) },			// bottom right
		};
		const uint16 RectIndices[6] = { 0, 1, 2, 3, 2, 0 };

		// Vertex Buffer
		{
			uint32 Sz = sizeof(MeshVertex) * ARRAYSIZE(RectVertices);
			D3D11_BUFFER_DESC Desc{ Sz, D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &RectVertices[0], 0, 0 };
			Res = Device->CreateBuffer(&Desc, &Data, &RectVB);
			VERIFY_HRESULT(Res, return false, "failed to create rect vertex buffer.");
		}

		// Index Buffer
		{
			uint32 Sz = (int32)sizeof(uint16) * ARRAYSIZE(RectIndices);
			D3D11_BUFFER_DESC Desc{ Sz, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &RectIndices[0], 0, 0 };
			Res = Device->CreateBuffer(&Desc, &Data, &RectIB);
			VERIFY_HRESULT(Res, return false, "failed to create rect index buffer.");
		}
	}

	// Create frame buffer
	{
		D3D11_BUFFER_DESC Desc{ 0 };
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.ByteWidth = sizeof(FrameBufferWVP);
		Desc.CPUAccessFlags = 0;
		Res = Device->CreateBuffer(&Desc, nullptr, &FrameBuffer_WVP);
		VERIFY_HRESULT(Res, return false, "failed to create frame buffer wvp.");
	}

	// Create shader
	{
		// VS
		ID3DBlob* VSBlob = nullptr;
		if (!CompileShader(L"vrplayer.hlsl", "vs_main", "vs_5_0", &VSBlob) || !VSBlob)
		{
			return false;
		}
		else
		{
			Res = Device->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &VS);
			VERIFY_HRESULT(Res, return false, "failed to create vertex shader.");

		}

		// PS
		ID3DBlob* PSBlob = nullptr;
		if (!CompileShader(L"vrplayer.hlsl", "ps_main", "ps_5_0", &PSBlob) || !PSBlob)
		{
			return false;
		}
		else
		{
			Res = Device->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &PS);
			VERIFY_HRESULT(Res, return false, "failed to create pixel shader.");
		}

		// Vertex Layout
		D3D11_INPUT_ELEMENT_DESC Desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, offsetof(MeshVertex, Texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Res = Device->CreateInputLayout(Desc, ARRAYSIZE(Desc), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &VertexLayout);
		VERIFY_HRESULT(Res, return false, "failed to create vertex layout.");
	}

	// Create texture 2D & shader resource view
	{
		D3D11_TEXTURE2D_DESC Desc{ 0 };
		Desc.Width = SViewportWidth;
		Desc.Height = SViewportHeight;
		Desc.ArraySize = 1;
		Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		Desc.MipLevels = 1;
		Desc.MiscFlags = 0;
		Desc.SampleDesc.Count = 1;
		Desc.SampleDesc.Quality = 0;
		Desc.Usage = D3D11_USAGE_DYNAMIC;
		Desc.Format = DXGI_FORMAT_R8_UNORM;

		D3D11_SHADER_RESOURCE_VIEW_DESC DescSRV;
		ZeroMemory(&DescSRV, sizeof(DescSRV));
		DescSRV.Format = Desc.Format;
		DescSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		DescSRV.Texture2D.MipLevels = 1;

		for (int32 i = 0; i < ARRAYSIZE(Tex); ++i)
		{
			Res = Device->CreateTexture2D(&Desc, nullptr, &Tex[i]);
			VERIFY_HRESULT(Res, return false, "failed to create texture 2D.");

			Res = Device->CreateShaderResourceView(Tex[i], &DescSRV, &SRV[i]);
			VERIFY_HRESULT(Res, return false, "failed to create shader resource view.");
		}
	}

	// Create sampler
	{
		D3D11_SAMPLER_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		Desc.MipLODBias = 0.0f;
		Desc.MaxAnisotropy = 1;
		Desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		Desc.BorderColor[0] = 0.0f;
		Desc.BorderColor[1] = 0.0f;
		Desc.BorderColor[2] = 0.0f;
		Desc.BorderColor[3] = 0.0f;
		Desc.MinLOD = -FLT_MAX;
		Desc.MaxLOD = FLT_MAX;
		Res = Device->CreateSamplerState(&Desc, &Sampler);
		VERIFY_HRESULT(Res, return false, "failed to create sampler.");
	}

	return true;
}

void RenderContext_D3D11::Draw_Test(float dtime)
{
	D3D11_VIEWPORT Viewport{ 0.0f, 0.0f, (float)SViewportWidth, (float)SViewportHeight, 0.0f, 1.0f };

	Context->RSSetState(Rasterizer);
	Context->OMSetRenderTargets(1, &BackBuffer, nullptr);
	Context->RSSetViewports(1, &Viewport);

	Context->ClearRenderTargetView(BackBuffer, DirectX::Colors::Gray);
	Context->ClearDepthStencilView(DepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

	LVMatrix ModelView;
	uint32 Stride = sizeof(MeshVertex);
	uint32 Offset = 0;

	// Setup input assembler
	Context->IASetIndexBuffer(RectIB, DXGI_FORMAT_R16_UINT, 0);
	Context->IASetVertexBuffers(0, 1, &RectVB, &Stride, &Offset);
	Context->IASetInputLayout(VertexLayout);
	Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Setup vs shader & parameters
	Context->VSSetShader(VS, nullptr, 0);
	Context->VSSetConstantBuffers(0, 1, &FrameBuffer_WVP);

	// Setup ps shader & parameters
	Context->PSSetShader(PS, nullptr, 0);
	Context->PSSetSamplers(0, 1, &Sampler);
	Context->PSSetShaderResources(0, 1, &SRV[FrameIndex]);

	// Update parameter
	FrameBufferWVP WVP;
	WVP.texmat = DirectX::XMMatrixIdentity();
	WVP.modelview = DirectX::XMMatrixTranslation(0.0f, 0.0f, 1.0f);
	WVP.Projection = DirectX::XMMatrixPerspectiveLH((float)SViewportWidth, (float)SViewportHeight, 0.1f, 1000.0f);
	Context->UpdateSubresource(FrameBuffer_WVP, 0, nullptr, &WVP, 0, 0);

	// Draw call
	Context->DrawIndexed(6, 0, 0);

	SwapChain->Present(0, 0);
}