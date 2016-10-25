#include "stdafx.h"
#include "DirectXUtil_D3D9.h"

static const TCHAR* SVertexShaderPath = TEXT("C:\\Users\\Administrator\\Documents\\GitHub\\snowflake\\SampleTest\\DetectHMD\\HookCore3\\projector\\projector.hlsl");
static const TCHAR* SPixelShaderPath = SVertexShaderPath;

using namespace lostvrdx9;

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
	DeviceRef(nullptr)
	, ContextRef(nullptr)
	, SourceRef(nullptr)
	, DSS(nullptr)
	, RS(nullptr)
	, VB(nullptr)
	, IB(nullptr)
	, VL(nullptr)
	, VS(nullptr)
	, PS(nullptr)
	, Sampler(nullptr)
	, WVPCB(nullptr)
	, SourceCopy(nullptr)
	, SRV(nullptr)
	, BufferFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
	, BufferWidth(1280)
	, BufferHeight(720)
	, NearPlane(0.1f)
	, FarPlane(10000.f)
{
	for (int i = 0; i < 2; ++i)
	{
		BB[i] = nullptr;
		RTVs[i] = nullptr;
	}

	DebugWorld = DirectX::XMMatrixIdentity();
	Fini();
}

TextureProjector::~TextureProjector()
{
	Fini();
}

bool TextureProjector::OnPresent(const ID3D11Resource* Source)
{
	Update();
	return true;
}

bool TextureProjector::Init(ID3D11Device* InDevice)
{
	HRESULT Res;
	
	ReleaseComObjectRef(DeviceRef);
	DeviceRef = InDevice;
	DeviceRef->AddRef();

	ReleaseComObjectRef(ContextRef);
	DeviceRef->GetImmediateContext(&ContextRef);

	for (int i=0; i<2; ++i)
	{
		{
			// projector's back buffer
			D3D11_TEXTURE2D_DESC Desc{ 0 };
			Desc.Format = BufferFormat;
			Desc.Width = BufferWidth;
			Desc.Height = BufferHeight;
			Desc.ArraySize = 1;
			Desc.BindFlags = D3D11_BIND_RENDER_TARGET;
			//Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			Desc.MipLevels = 1;
			Desc.MiscFlags = 0;
			Desc.SampleDesc.Count = 1;
			Desc.SampleDesc.Quality = 0;
			//Desc.Usage = D3D11_USAGE_DEFAULT;
			Desc.Usage = D3D11_USAGE_DEFAULT;
			VERIFY_HRESULT(DeviceRef->CreateTexture2D(&Desc, nullptr, &BB[i]), return false, "failed to create projector's back buffer.");
		}

		{
			// render target view of projector's back buffer
			D3D11_RENDER_TARGET_VIEW_DESC Desc;
			ZeroMemory(&Desc, sizeof(Desc));
			Desc.Format = BufferFormat;
			Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			VERIFY_HRESULT(DeviceRef->CreateRenderTargetView(BB[i], &Desc, &RTVs[i]), return false, "failed to create render target view of the projector's back buffer.");
		}
	}

	{
		// projector's depth-stencil state
		D3D11_DEPTH_STENCIL_DESC Desc{ 0 };
		Desc.DepthEnable = FALSE;
		Desc.StencilEnable = FALSE;
		VERIFY_HRESULT(DeviceRef->CreateDepthStencilState(&Desc, &DSS), return false, "failed to create projector's depth-stencil state.");
	}

	{
		// projector's rasterize state
		D3D11_RASTERIZER_DESC Desc;
		ZeroMemory(&Desc, sizeof(Desc));
		Desc.AntialiasedLineEnable = FALSE;
		Desc.CullMode = D3D11_CULL_NONE;
		Desc.DepthBias = 0;
		Desc.DepthBiasClamp = 0.0f;
		Desc.DepthClipEnable = FALSE;
		Desc.FillMode = D3D11_FILL_SOLID;
		Desc.FrontCounterClockwise = FALSE;
		Desc.MultisampleEnable = FALSE;
		Desc.ScissorEnable = FALSE;
		Desc.SlopeScaledDepthBias = 0.0f;
		VERIFY_HRESULT(DeviceRef->CreateRasterizerState(&Desc, &RS), return false, "failed to create projector's rasterize state.");
	}

	{
		// Create Mesh
		const MeshVertex RectVertices[4] =
		{
			{ LVVec3(-1.0f, -1.0f, 0.0f), LVVec2(0.0f, 1.0f) },		// bottom left
			{ LVVec3(-1.0f, 1.0f, 0.0f), LVVec2(0.0f, 0.0f) },			// top left
			{ LVVec3(1.0f, 1.0f, 0.0f), LVVec2(1.0f, 0.0f) },			// top right
			{ LVVec3(1.0f, -1.0f, 0.0f), LVVec2(1.0f, 1.0f) },			// bottom right
		};

		const uint16 RectIndices[6] = { 0, 1, 2, 3, 0, 2 };

		{
			// Vertex Buffer
			uint32 Sz = sizeof(MeshVertex) * ARRAYSIZE(RectVertices);
			D3D11_BUFFER_DESC Desc{ Sz, D3D11_USAGE_DEFAULT	, D3D11_BIND_VERTEX_BUFFER, 0, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &RectVertices[0], 0, 0 };
			Res = DeviceRef->CreateBuffer(&Desc, &Data, &VB);
			VERIFY_HRESULT(Res, return false, "failed to create rect vertex buffer.");
		}

		{
			// Index Buffer
			uint32 Sz = (int32)sizeof(uint16) * ARRAYSIZE(RectIndices);
			D3D11_BUFFER_DESC Desc{ Sz, D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0, 0 };
			D3D11_SUBRESOURCE_DATA Data{ &RectIndices[0], 0, 0 };
			Res = DeviceRef->CreateBuffer(&Desc, &Data, &IB);
			VERIFY_HRESULT(Res, return false, "failed to create rect index buffer.");
		}
	}

	{
		// Create frame buffer
		D3D11_BUFFER_DESC Desc{ 0 };
		Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		Desc.Usage = D3D11_USAGE_DEFAULT;
		Desc.ByteWidth = sizeof(FrameBufferWVP);
		Desc.CPUAccessFlags = 0;
		Res = DeviceRef->CreateBuffer(&Desc, nullptr, &WVPCB);
		VERIFY_HRESULT(Res, return false, "failed to create frame buffer wvp.");
	}

	{
		// Create shader
		// VS
		ID3DBlob* VSBlob = nullptr;
		if (!CompileShader(SVertexShaderPath, "vs_main", "vs_5_0", &VSBlob) || !VSBlob)
		{
			return false;
		}
		else
		{
			Res = DeviceRef->CreateVertexShader(VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), nullptr, &VS);
			VERIFY_HRESULT(Res, return false, "failed to create vertex shader.");
		}

		// PS
		ID3DBlob* PSBlob = nullptr;
		if (!CompileShader(SPixelShaderPath, "ps_main", "ps_5_0", &PSBlob) || !PSBlob)
		{
			return false;
		}
		else
		{
			Res = DeviceRef->CreatePixelShader(PSBlob->GetBufferPointer(), PSBlob->GetBufferSize(), nullptr, &PS);
			VERIFY_HRESULT(Res, return false, "failed to create pixel shader.");
		}

		// Vertex Layout
		D3D11_INPUT_ELEMENT_DESC Desc[]
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(MeshVertex, Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT   , 0, offsetof(MeshVertex, Texcoord), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Res = DeviceRef->CreateInputLayout(Desc, ARRAYSIZE(Desc), VSBlob->GetBufferPointer(), VSBlob->GetBufferSize(), &VL);
		VERIFY_HRESULT(Res, return false, "failed to create vertex layout.");
	}

	{
		// Create sampler
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
		Res = DeviceRef->CreateSamplerState(&Desc, &Sampler);
		VERIFY_HRESULT(Res, return false, "failed to create sampler.");
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
			D3D11_TEXTURE2D_DESC SrcDesc;
			SourceRef->GetDesc(&SrcDesc);

			D3D11_SHADER_RESOURCE_VIEW_DESC Desc;
			ZeroMemory(&Desc, sizeof(Desc));
			Desc.Format = SrcDesc.Format;
			Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			Desc.Texture2D.MipLevels = 1;
			VERIFY_HRESULT(DeviceRef->CreateShaderResourceView(SourceRef, &Desc, &SRV), return false, "failed to create source's shader resource view.");
		}
	}

	return true;
}

void TextureProjector::Fini()
{
	for (int i = 0; i < 2; ++i)
	{
		ReleaseComObjectRef(RTVs[i]);
		ReleaseComObjectRef(BB[i]);
	}

	ReleaseComObjectRef(DSS);
	ReleaseComObjectRef(RS);

	ReleaseComObjectRef(VB);
	ReleaseComObjectRef(IB);
	ReleaseComObjectRef(VL);
	ReleaseComObjectRef(VS);
	ReleaseComObjectRef(PS);
	ReleaseComObjectRef(Sampler);
	ReleaseComObjectRef(WVPCB);
	ReleaseComObjectRef(SourceCopy);
	ReleaseComObjectRef(SRV);

	ReleaseComObjectRef(ContextRef);
	ReleaseComObjectRef(DeviceRef);
	ReleaseComObjectRef(SourceRef);
}

void TextureProjector::Update()
{
	LVASSERT(DeviceRef != nullptr, "TextureProjector::Update", "null device");
	LVASSERT(ContextRef != nullptr, "TextureProjector::Update", "null context");

	for (int i = 0; i < 2; ++i)
	{
		ContextRef->RSSetState(RS);
		ContextRef->OMSetRenderTargets(1, &RTVs[i], nullptr);
		D3D11_VIEWPORT Viewport{ 0.0f, 0.0f, (float)BufferWidth, (float)BufferHeight, 0.0f, 1.0f };
		ContextRef->RSSetViewports(1, &Viewport);

		ContextRef->ClearRenderTargetView(RTVs[i], DirectX::Colors::DarkOrange);

		uint32 Stride = sizeof(MeshVertex);
		uint32 Offset = 0;

		// input layout
		ContextRef->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
		ContextRef->IASetVertexBuffers(0, 1, &VB, &Stride, &Offset);
		ContextRef->IASetInputLayout(VL);
		ContextRef->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// vertex shader
		ContextRef->VSSetShader(VS, nullptr, 0);

		//LVMatrix Proj = DirectX::XMMatrixPerspectiveFovLH(10.f*3.14159f/180.f, 10.f / 16.f, NearPlane, FarPlane);
		LVMatrix Trans = DirectX::XMMatrixTranslation(0.0f, 0.0f, -10.f);
		LVMatrix Rot = DirectX::XMMatrixRotationAxis({ 0.f, 1.f, 0.f }, 0.f);
		LVMatrix Scale = DirectX::XMMatrixScaling(1.f, 1.f, 1.f);
		FrameBufferWVP WVP(DirectX::XMMatrixTranspose(DebugWorld * Trans), EyePose[i].V, EyePose[i].P);
		ContextRef->UpdateSubresource(WVPCB, 0, nullptr, &WVP, 0, 0);
		ContextRef->VSSetConstantBuffers(0, 1, &WVPCB);

		// pixel shader
		ContextRef->PSSetShader(PS, nullptr, 0);
		ContextRef->PSSetSamplers(0, 1, &Sampler);

		ID3D11ShaderResourceView* SRVs[] = { SRV };
		ContextRef->PSSetShaderResources(0, 1, &SRVs[0]);
		//ContextRef->PS

		ContextRef->DrawIndexed(6, 0, 0);
		//LVMSG("TextureProjector::Update", "");
	}
}

ID3D11Texture2D* TextureProjector::GetFinalBuffer(EnumEyeID Eye)
{
	return BB[Eye];
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
	LVMSG("TextureProjector::SetEyePose", "eye: %d **************", Eye);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[0].m128_f32[0], EyeView.r[0].m128_f32[1], EyeView.r[0].m128_f32[2], EyeView.r[0].m128_f32[3], Proj.r[0].m128_f32[0], Proj.r[0].m128_f32[1], Proj.r[0].m128_f32[2], Proj.r[0].m128_f32[3]);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[1].m128_f32[0], EyeView.r[1].m128_f32[1], EyeView.r[1].m128_f32[2], EyeView.r[1].m128_f32[3], Proj.r[1].m128_f32[0], Proj.r[1].m128_f32[1], Proj.r[1].m128_f32[2], Proj.r[1].m128_f32[3]);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[2].m128_f32[0], EyeView.r[2].m128_f32[1], EyeView.r[2].m128_f32[2], EyeView.r[2].m128_f32[3], Proj.r[2].m128_f32[0], Proj.r[2].m128_f32[1], Proj.r[2].m128_f32[2], Proj.r[2].m128_f32[3]);
	LVMSG("TextureProjector::SetEyePose", "EyeView %f\t%f\t%f\t%f\t|\t Proj %f\t%f\t%f\t%f", EyeView.r[3].m128_f32[0], EyeView.r[3].m128_f32[1], EyeView.r[3].m128_f32[2], EyeView.r[3].m128_f32[3], Proj.r[3].m128_f32[0], Proj.r[3].m128_f32[1], Proj.r[3].m128_f32[2], Proj.r[3].m128_f32[3]);

	EyePose[Eye].V = DirectX::XMMatrixIdentity();
	EyePose[Eye].P = EyeView * Proj;
	EyePose[Eye].P = DirectX::XMMatrixTranspose(EyePose[Eye].P);

#ifdef ENABLETEST_TRANSFORM
	TestMatrixVectorMultiply(EyePose[Eye].P, LVVec(-1.f, 1.f, 1.f, 1.f));
#endif
}

bool TextureProjector::SetSourceRef(ID3D11Texture2D* InSource)
{
	if (InSource == nullptr || InSource == SourceRef)
	{
		return true;
	}

	ReleaseComObjectRef(SourceRef);
	SourceRef = InSource;
	SourceRef->AddRef();

	ID3D11Device* Dev = nullptr;
	InSource->GetDevice(&Dev);
	if (Dev == nullptr)
	{
		LVMSG("TextureProjector::SetSourceRef", "failed to get device from the input buffer(%x)", InSource);
		return false;
	}

	if (Dev != DeviceRef)
	{
		Fini();
		return Init(Dev);
	}

	ReleaseComObjectRef(Dev);
	return true;
}