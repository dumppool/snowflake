#include "HookCorePCH.h"
#include "Projector.h"

using namespace lostvr;

#define ENABLETEST_TRANSFORM
static void TestMatrixVectorMultiply(const LVMatrix& Mat, const LVVec& Vec)
{
	DirectX::XMVECTOR Src, Dst;
	memcpy((void*)&Src.m128_f32[0], &Vec.x, sizeof(Vec.x) * 4);
	Dst = DirectX::XMVector4Transform(Src, Mat);
	LVMSG("TestMatrixVectorMultiply", "Vec: %f\t%f\t%f\t%f\t\tResult: %.2f\t%.2f\t%.2f\t%.2f", Vec.x, Vec.y, Vec.z, Vec.w, Dst.m128_f32[0], Dst.m128_f32[1], Dst.m128_f32[2], Dst.m128_f32[3]);
	LVMSG("TestMatrixVectorMultiply", "Mat %f\t%f\t%f\t%f", Mat.r[0].m128_f32[0], Mat.r[0].m128_f32[1], Mat.r[0].m128_f32[2], Mat.r[0].m128_f32[3]);
	LVMSG("TestMatrixVectorMultiply", "Mat %f\t%f\t%f\t%f", Mat.r[1].m128_f32[0], Mat.r[1].m128_f32[1], Mat.r[1].m128_f32[2], Mat.r[1].m128_f32[3]);
	LVMSG("TestMatrixVectorMultiply", "Mat %f\t%f\t%f\t%f", Mat.r[2].m128_f32[0], Mat.r[2].m128_f32[1], Mat.r[2].m128_f32[2], Mat.r[2].m128_f32[3]);
	LVMSG("TestMatrixVectorMultiply", "Mat %f\t%f\t%f\t%f", Mat.r[3].m128_f32[0], Mat.r[3].m128_f32[1], Mat.r[3].m128_f32[2], Mat.r[3].m128_f32[3]);
}

static bool CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob** blob)
{
	if (!file || !entry || !target || !blob)
	{
		LVASSERT(0, "CompileShader", "file(%ls), entry(%s), target(%s), blob(%x)", file, entry, target, blob);
		return false;
	}

	ID3DBlob* ShaderBlob = nullptr;
	ID3DBlob* ErrorBlob = nullptr;
	UINT Flag1 = 0;
	UINT Flag2 = 0;
	Flag1 |= D3DCOMPILE_DEBUG;
	HRESULT Res = D3DCompileFromFile(file, NULL, NULL, entry, target, Flag1, Flag2, &ShaderBlob, &ErrorBlob);
	if (FAILED(Res))
	{
		LVMSG("CompileShader", "compile %ls(%s, %s) failed: %s", file, entry, target, ErrorBlob?ErrorBlob->GetBufferPointer():"");

		if (ShaderBlob)
		{
			ShaderBlob->Release();
		}

		if (ErrorBlob)
		{
			ErrorBlob->Release();
		}

		return false;
	}
	else
	{
		LVMSG("CompileShader", "compile %ls(%s, %s) successfully", file, entry, target);
		*blob = ShaderBlob;
		return true;
	}
}

TextureProjector::TextureProjector() :
	DSS(nullptr)
	, RS(nullptr)
	, VB(nullptr)
	, IB(nullptr)
	, VL(nullptr)
	, VS(nullptr)
	, PS(nullptr)
	, Sampler(nullptr)
	, WVPCB(nullptr)
	, SRV(nullptr)
{
	for (int i = 0; i < 2; ++i)
	{
		BB[i] = nullptr;
		RTVs[i] = nullptr;
	}

	DestroyRHI();
}

TextureProjector::~TextureProjector()
{
	DestroyRHI();
}

bool TextureProjector::IsInitialized(IDXGISwapChain* swapChain) const
{
	return (Renderer != nullptr && Renderer->GetSwapChain() == swapChain);
}

bool TextureProjector::InitializeProjector(
	IDXGISwapChain* swapChain,
	uint32 width, uint32 height,
	const LVMatrix* leftView,
	const LVMatrix* leftProj,
	const LVMatrix* rightView,
	const LVMatrix* rightProj)
{
	const CHAR* head = "TextureProjector::InitializeProjector";

	DestroyRHI();

	RecommendWidth = width;
	RecommendHeight = height;

	if (swapChain == nullptr)
	{
		LVMSG(head, "null swap chain");
		return false;
	}

	DXGI_SWAP_CHAIN_DESC scDesc;
	swapChain->GetDesc(&scDesc);

	DXGI_FORMAT format = scDesc.BufferDesc.Format;

	//EDirect3D nextVer;
	//if (!GetNextGIVersion(LastGIVersion, nextVer))
	//{
	//	LVMSG(head, "invalid graphics interface version: %s", GetEDirect3DString(nextVer));
	//	return false;
	//}
	
	Renderer = new Direct3D11Helper(EDirect3D::DeviceRef);
	Renderer->UpdateRHIWithSwapChain(swapChain);
	IDXGISwapChain* swapChainRef = Renderer->GetSwapChain();

	HRESULT hr = S_FALSE;

	if (leftView != nullptr && leftProj != nullptr)
	{
		SetEyePose(SEnumEyeL, *leftView, *leftProj);
	}

	if (rightView != nullptr && rightProj != nullptr)
	{
		SetEyePose(SEnumEyeR, *rightView, *rightProj);
	}

	ID3D11Device* deviceRef = Renderer->GetDevice();
	for (int i = 0; i < 2; ++i)
	{
		{
			// projector's back buffer
			D3D11_TEXTURE2D_DESC desc{ 0 };
			desc.Format = format;
			desc.Width = RecommendWidth;
			desc.Height = RecommendHeight;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET;
			desc.MipLevels = 1;
			desc.MiscFlags = 0;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			VERIFY_HRESULT(deviceRef->CreateTexture2D(&desc, nullptr, &BB[i]), return false, "failed to create projector's back buffer.");
		}

		{
			// render target view of projector's back buffer
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = format;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			VERIFY_HRESULT(deviceRef->CreateRenderTargetView(BB[i], &desc, &RTVs[i]), return false, "failed to create render target view of the projector's back buffer.");
		}
	}

	{
		// projector's depth-stencil state
		D3D11_DEPTH_STENCIL_DESC desc{ 0 };
		desc.DepthEnable = FALSE;
		desc.StencilEnable = FALSE;
		VERIFY_HRESULT(deviceRef->CreateDepthStencilState(&desc, &DSS), return false, "failed to create projector's depth-stencil state.");
	}

	{
		// projector's rasterize state
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AntialiasedLineEnable = FALSE;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.DepthClipEnable = FALSE;
		desc.FillMode = D3D11_FILL_SOLID;
		desc.FrontCounterClockwise = FALSE;
		desc.MultisampleEnable = FALSE;
		desc.ScissorEnable = FALSE;
		desc.SlopeScaledDepthBias = 0.0f;
		VERIFY_HRESULT(deviceRef->CreateRasterizerState(&desc, &RS), return false, "failed to create projector's rasterize state.");
	}

	{
		// Create Mesh
		const MeshVertex rectVertices[4] =
		{
			{ LVVec3(-1.0f, -1.0f, 0.0f), LVVec2(0.0f, 1.0f) },		// bottom left
			{ LVVec3(-1.0f, 1.0f, 0.0f), LVVec2(0.0f, 0.0f) },			// top left
			{ LVVec3(1.0f, 1.0f, 0.0f), LVVec2(1.0f, 0.0f) },			// top right
			{ LVVec3(1.0f, -1.0f, 0.0f), LVVec2(1.0f, 1.0f) },			// bottom right
		};

		const uint16 rectIndices[6] = { 0, 1, 2, 3, 0, 2 };

		{
			// Vertex Buffer
			uint32 sz = sizeof(MeshVertex) * ARRAYSIZE(rectVertices);
			D3D11_BUFFER_DESC Desc{ sz, D3D11_USAGE_DEFAULT	, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &rectVertices[0], 0, 0 };
			hr = deviceRef->CreateBuffer(&Desc, &Data, &VB);
			VERIFY_HRESULT(hr, return false, "failed to create rect vertex buffer.");
		}

		{
			// Index Buffer
			uint32 sz = (int32)sizeof(uint16) * ARRAYSIZE(rectIndices);
			D3D11_BUFFER_DESC Desc{ sz, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &rectIndices[0], 0, 0 };
			hr = deviceRef->CreateBuffer(&Desc, &Data, &IB);
			VERIFY_HRESULT(hr, return false, "failed to create rect index buffer.");
		}
	}

	{
		// Create frame buffer
		D3D11_BUFFER_DESC desc{ 0 };
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(FrameBufferWVP);
		desc.CPUAccessFlags = 0;
		hr = deviceRef->CreateBuffer(&desc, nullptr, &WVPCB);
		VERIFY_HRESULT(hr, return false, "failed to create frame buffer wvp.");
	}

	{
		// Create shader
		// VS
		ID3DBlob* VSBlob = nullptr;
		if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath(), "vs_main", "vs_5_0", &VSBlob) || !VSBlob)
		{
			return false;
		}
		else
		{
			hr = deviceRef->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &VS);
			VERIFY_HRESULT(hr, return false, "failed to create vertex shader.");
		}

		// PS
		ID3DBlob* PSBlob = nullptr;
		if (!CompileShader(SGlobalSharedDataInst.GetShaderFilePath(), "ps_main", "ps_5_0", &PSBlob) || !PSBlob)
		{
			return false;
		}
		else
		{
			hr = deviceRef->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &PS);
			VERIFY_HRESULT(hr, return false, "failed to create pixel shader.");
		}

		// Vertex Layout
		D3D11_INPUT_ELEMENT_DESC desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, offsetof(MeshVertex, Texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = deviceRef->CreateInputLayout(desc, ARRAYSIZE(desc), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &VL);
		VERIFY_HRESULT(hr, return false, "failed to create vertex layout.");
	}

	{
		// Create sampler
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 0.0f;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		hr = deviceRef->CreateSamplerState(&desc, &Sampler);
		VERIFY_HRESULT(hr, return false, "failed to create sampler.");
	}

	{
		//{
		//	// buffer of projector's shader resource view
		//	D3D11_TEXTURE2D_DESC Desc{ 0 };
		//	Desc.Format = BufferFormat;
		//	Desc.Width = BufferWidth;
		//	Desc.Height = BufferHeight;
		//	Desc.ArraySize = 1;
		//	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//	Desc.MipLevels = 1;
		//	Desc.MiscFlags = 0;
		//	Desc.SampleDesc.Count = 1;
		//	Desc.SampleDesc.Quality = 0;
		//	Desc.Usage = D3D11_USAGE_DEFAULT;
		//	//Desc.Usage = D3D11_USAGE_DYNAMIC;
		//	VERIFY_HRESULT(DeviceRef->CreateTexture2D(&Desc, nullptr, &SourceCopy), return false, "failed to create buffer of projector's shader resource view.");
		//}

		{
			D3D11_SHADER_RESOURCE_VIEW_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Format = format;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipLevels = 1;
			ID3D11Texture2D* tex = 0;
			swapChainRef->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex);
			VERIFY_HRESULT(deviceRef->CreateShaderResourceView(tex, &desc, &SRV), return false, "failed to create source's shader resource view.");
			SAFE_RELEASE(tex);
		}
	}

	return SUCCEEDED(hr);
}

void TextureProjector::DestroyRHI()
{
	LastGIVersion = (Renderer != nullptr ? Renderer->GetGraphicsInterfaceVersion() : EDirect3D::DeviceRef);

	if (Renderer != nullptr)
	{
		delete Renderer;
		Renderer = nullptr;
	}

	for (int i = 0; i < 2; ++i)
	{
		SAFE_RELEASE(RTVs[i]);
		SAFE_RELEASE(BB[i]);
	}

	SAFE_RELEASE(DSS);
	SAFE_RELEASE(RS);

	SAFE_RELEASE(VB);
	SAFE_RELEASE(IB);
	SAFE_RELEASE(VL);
	SAFE_RELEASE(VS);
	SAFE_RELEASE(PS);
	SAFE_RELEASE(Sampler);
	SAFE_RELEASE(WVPCB);
	SAFE_RELEASE(SRV);

}

bool TextureProjector::UpdateTexture()
{
	const CHAR* head = "TextureProjector::UpdateTexture";
	if (Renderer == nullptr)
	{
		LVMSG(head, "null renderer");
		return false;
	}

	ID3D11DeviceContext* context = Renderer->GetContext();

	D3D11_VIEWPORT Viewport{ 0.0f, 0.0f, (float)RecommendWidth, (float)RecommendHeight, 0.0f, 1.0f };
	context->RSSetViewports(1, &Viewport);

	for (int i = 0; i < 2; ++i)
	{
		context->RSSetState(RS);
		context->OMSetRenderTargets(1, &RTVs[i], nullptr);

		context->ClearRenderTargetView(RTVs[i], DirectX::Colors::DarkOrange);

		uint32 stride = sizeof(MeshVertex);
		uint32 offset = 0;

		// input layout
		context->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
		context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
		context->IASetInputLayout(VL);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// vertex shader
		context->VSSetShader(VS, nullptr, 0);

		LVMatrix trans = DirectX::XMMatrixTranslation(0.0f, 0.0f, -10.0f);
		LVMatrix rot = DirectX::XMMatrixRotationAxis({ 0.f, 1.f, 0.f }, 0.f);
		LVMatrix scale = DirectX::XMMatrixScaling(10.404f, 10.404f, 10.404f);
		FrameBufferWVP WVP(DirectX::XMMatrixTranspose(scale * trans), EyePose[i].V, EyePose[i].P);
		context->UpdateSubresource(WVPCB, 0, nullptr, &WVP, 0, 0);
		context->VSSetConstantBuffers(0, 1, &WVPCB);

		// pixel shader
		context->PSSetShader(PS, nullptr, 0);
		context->PSSetSamplers(0, 1, &Sampler);

		ID3D11ShaderResourceView* srvs[] = { SRV };
		context->PSSetShaderResources(0, 1, &srvs[0]);
		//ContextRef->PS

		context->DrawIndexed(6, 0, 0);
		//LVMSG("TextureProjector::Update", "");
	}

	return true;
}

ID3D11Texture2D* TextureProjector::GetFinalBuffer(EnumEyeID eye)
{
	return BB[eye];
}


//static LVMatrix LVMatrixMultiply(const LVMatrix& l, const LVMatrix& r)
//{
//	return LVMatrix(m[0] * n[0] + m[4] * n[1] + m[8] * n[2] + m[12] * n[3], m[1] * n[0] + m[5] * n[1] + m[9] * n[2] + m[13] * n[3], m[2] * n[0] + m[6] * n[1] + m[10] * n[2] + m[14] * n[3], m[3] * n[0] + m[7] * n[1] + m[11] * n[2] + m[15] * n[3],
//		m[0] * n[4] + m[4] * n[5] + m[8] * n[6] + m[12] * n[7], m[1] * n[4] + m[5] * n[5] + m[9] * n[6] + m[13] * n[7], m[2] * n[4] + m[6] * n[5] + m[10] * n[6] + m[14] * n[7], m[3] * n[4] + m[7] * n[5] + m[11] * n[6] + m[15] * n[7],
//		m[0] * n[8] + m[4] * n[9] + m[8] * n[10] + m[12] * n[11], m[1] * n[8] + m[5] * n[9] + m[9] * n[10] + m[13] * n[11], m[2] * n[8] + m[6] * n[9] + m[10] * n[10] + m[14] * n[11], m[3] * n[8] + m[7] * n[9] + m[11] * n[10] + m[15] * n[11],
//		m[0] * n[12] + m[4] * n[13] + m[8] * n[14] + m[12] * n[15], m[1] * n[12] + m[5] * n[13] + m[9] * n[14] + m[13] * n[15], m[2] * n[12] + m[6] * n[13] + m[10] * n[14] + m[14] * n[15], m[3] * n[12] + m[7] * n[13] + m[11] * n[14] + m[15] * n[15]);
//}

void TextureProjector::SetEyePose(EnumEyeID Eye, const LVMatrix& EyeView, const LVMatrix& Proj)
{
	EyePose[Eye].V = DirectX::XMMatrixIdentity();
	EyePose[Eye].P = EyeView * Proj;
	EyePose[Eye].P = DirectX::XMMatrixTranspose(EyePose[Eye].P);

#ifdef _DEBUG
	LVMSG("TextureProjector::SetEyePose", "eye: %d **************", Eye);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[0].m128_f32[0], EyeView.r[0].m128_f32[1], EyeView.r[0].m128_f32[2], EyeView.r[0].m128_f32[3], Proj.r[0].m128_f32[0], Proj.r[0].m128_f32[1], Proj.r[0].m128_f32[2], Proj.r[0].m128_f32[3]);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[1].m128_f32[0], EyeView.r[1].m128_f32[1], EyeView.r[1].m128_f32[2], EyeView.r[1].m128_f32[3], Proj.r[1].m128_f32[0], Proj.r[1].m128_f32[1], Proj.r[1].m128_f32[2], Proj.r[1].m128_f32[3]);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[2].m128_f32[0], EyeView.r[2].m128_f32[1], EyeView.r[2].m128_f32[2], EyeView.r[2].m128_f32[3], Proj.r[2].m128_f32[0], Proj.r[2].m128_f32[1], Proj.r[2].m128_f32[2], Proj.r[2].m128_f32[3]);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[3].m128_f32[0], EyeView.r[3].m128_f32[1], EyeView.r[3].m128_f32[2], EyeView.r[3].m128_f32[3], Proj.r[3].m128_f32[0], Proj.r[3].m128_f32[1], Proj.r[3].m128_f32[2], Proj.r[3].m128_f32[3]);

	TestMatrixVectorMultiply(EyePose[Eye].P, LVVec(-1.f, 1.f, 1.f, 1.f));
#endif
}