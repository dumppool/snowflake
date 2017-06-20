/*
 * file Projector.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
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

BaseTextureProjector::BaseTextureProjector() :
	DSS(nullptr)
	, RS(nullptr)
	, VB(nullptr)
	, IB(nullptr)
	, VL(nullptr)
	, VS(nullptr)
	, PS(nullptr)
	, Sampler(nullptr)
	, WVPCB(nullptr)
	, Translation(0.f, 0.f, -10.f)
	, Scale(20.8f, 20.8f, 20.8f)
	, Rotation(0.f, 1.f, 0.f)
{
	for (int i = 0; i < 2; ++i)
	{
		BB[i] = nullptr;
		RTVs[i] = nullptr;
	}

	DestroyRHI();
}

BaseTextureProjector::~BaseTextureProjector()
{
	DestroyRHI();
}

bool BaseTextureProjector::InitializeProjector(
	IDXGISwapChain* swapChain,
	uint32 hmdWidth, uint32 hmdHeight,
	const LVMatrix& leftView,
	const LVMatrix& leftProj,
	const LVMatrix& rightView,
	const LVMatrix& rightProj)
{
	const CHAR* head = "TextureProjector::InitializeProjector";

	DestroyRHI();

	RecommendWidth = hmdWidth;
	RecommendHeight = hmdHeight;

	if (!InitializeRenderer(swapChain))
	{
		LVMSG(head, "initialize renderer failed");
		return false;
	}

	LVMSG(head, "dxgi version: %s", GetEDirect3DString(GetInterfaceVersionFromSwapChain(swapChain)));
	LVMSG(head, "%s", GetDescriptionFromSwapChain(swapChain).c_str());

	SetEyePose(SEnumEyeL, leftView, leftProj);
	SetEyePose(SEnumEyeR, rightView, rightProj);

	bool ret = InitializeRHI() && InitializeTextureSRV();
	return ret;
}

bool BaseTextureProjector::InitializeRHI()
{
	const CHAR* head = "BaseTextureProjector::InitializeRHI";

	HRESULT hr = S_FALSE;

	ID3D11Device* deviceRef = GetRenderer()->GetDevice();

	DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM;
	for (int i = 0; i < 2; ++i)
	{
		{
			// projector's back buffer
			D3D11_TEXTURE2D_DESC desc{ 0 };
			desc.Format = format;
			desc.Width = RecommendWidth;
			desc.Height = RecommendHeight;
			desc.ArraySize = 1;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET|D3D11_BIND_SHADER_RESOURCE;
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

	if (!GetRenderer()->GetDefaultRasterizer(&RS))
	{
		return false;
	}

	if (!GetRenderer()->CreateMesh_Rect(1.f, 1.f, sizeof(MeshVertex), &VB, &IB))
	{
		return false;
	}

	D3D11_BUFFER_DESC bdesc;
	GetRenderer()->GetDescriptionTemplate_DefaultBuffer(bdesc);
	bdesc.ByteWidth = sizeof(FrameBufferWVP);
	if (!GetRenderer()->CreateBuffer(bdesc, &WVPCB))
	{
		return false;
	}

	if (!GetRenderer()->GetDefaultShader(&VS, &PS, &VL))
	{
		return false;
	}
	else
	{
		VS->AddRef();
		PS->AddRef();
		VL->AddRef();
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

	return SUCCEEDED(hr);
}

void BaseTextureProjector::DestroyRHI()
{
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

}

#ifndef ENABLE_RESTORESTATE
#define ENABLE_RESTORESTATE
#endif

bool BaseTextureProjector::UpdateTexture()
{
	const CHAR* head = "TextureProjector::UpdateTexture";
	if (GetRenderer() == nullptr)
	{
		LVMSG(head, "null renderer");
		return false;
	}

	if (!PrepareSRV())
	{
		LVMSG(head, "prepare shader resource view failed");
		return false;
	}

	ID3D11DeviceContext* context = GetRenderer()->GetContext();

#ifdef ENABLE_RESTORESTATE
	D3D11_VIEWPORT oldVP;
	UINT numVPs = 1;
	context->RSGetViewports(&numVPs, &oldVP);

	ID3D11RasterizerState* oldRSState = nullptr;
	context->RSGetState(&oldRSState);

	ID3D11Buffer* oldBuffer = nullptr;
	context->VSGetConstantBuffers(0, 1, &oldBuffer);

	UINT oldStride, oldOffset;
	ID3D11Buffer* oldVB = nullptr;
	context->IAGetVertexBuffers(0, 1, &oldVB, &oldStride, &oldOffset);

	ID3D11Buffer* oldIB = nullptr;
	DXGI_FORMAT oldFormat;
	UINT oldOffset2;
	context->IAGetIndexBuffer(&oldIB, &oldFormat, &oldOffset2);

	D3D11_PRIMITIVE_TOPOLOGY oldTopo;
	context->IAGetPrimitiveTopology(&oldTopo);

	ID3D11InputLayout* oldIL = nullptr;
	context->IAGetInputLayout(&oldIL);

	ID3D11RenderTargetView* oldRTV[2] = {0};
	ID3D11DepthStencilView* oldDSV = nullptr;
	context->OMGetRenderTargets(1, &oldRTV[0], &oldDSV);

#endif // ENABLE_RESTORESTATE

	D3D11_VIEWPORT Viewport{ 0.0f, 0.0f, (float)RecommendWidth, (float)RecommendHeight, 0.0f, 1.0f };
	context->RSSetViewports(1, &Viewport);
	context->RSSetState(RS);

	for (int i = 0; i < 2; ++i)
	{
		context->OMSetRenderTargets(1, &RTVs[i], nullptr);
		context->ClearRenderTargetView(RTVs[i], DirectX::Colors::Black);

		uint32 stride = sizeof(MeshVertex);
		uint32 offset = 0;

		// input layout
		context->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
		context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
		context->IASetInputLayout(VL);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// vertex shader
		context->VSSetShader(VS, nullptr, 0);

		LVMatrix trans = DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
		LVMatrix rot = DirectX::XMMatrixRotationAxis({ Rotation.x, Rotation.y, Rotation.z }, 0.f);
		LVMatrix scale = DirectX::XMMatrixScaling(Scale.x, Scale.y, Scale.z);
		FrameBufferWVP wvp(DirectX::XMMatrixTranspose(scale * trans), EyePose[i].V, EyePose[i].P);
		context->UpdateSubresource(WVPCB, 0, nullptr, &wvp, 0, 0);
		context->VSSetConstantBuffers(0, 1, &WVPCB);

		// pixel shader
		context->PSSetShader(PS, nullptr, 0);
		context->PSSetSamplers(0, 1, &Sampler);

		ID3D11ShaderResourceView* srvs[] = { GetTextureSRV() };
		context->PSSetShaderResources(0, 1, &srvs[0]);

		context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

		context->DrawIndexed(6, 0, 0);

		GetRenderer()->UpdateCursor(LVVec3(Scale.x, Scale.y, Translation.z), 1.f, EyePose[i].V, EyePose[i].P, true);
	}

#ifdef ENABLE_RESTORESTATE
	context->RSSetViewports(numVPs, &oldVP);
	context->RSSetState(oldRSState);
	context->VSSetConstantBuffers(0, 1, &oldBuffer);
	context->IASetVertexBuffers(0, 1, &oldVB, &oldStride, &oldOffset);
	context->IASetIndexBuffer(oldIB, oldFormat, oldOffset2);
	context->IASetPrimitiveTopology(oldTopo);
	context->IASetInputLayout(oldIL);
	context->OMSetRenderTargets(1, &oldRTV[0], oldDSV);
#endif // ENABLE_RESTORESTATE

	return true;
}

ID3D11Texture2D* BaseTextureProjector::GetFinalBuffer(EnumEyeID eye)
{
	return BB[eye];
}

ID3D11RenderTargetView * BaseTextureProjector::GetFinalBuffer_RTV(EnumEyeID eye)
{
	return RTVs[eye];
}

void BaseTextureProjector::SetEyePose(EnumEyeID Eye, const LVMatrix& EyeView, const LVMatrix& Proj)
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

bool BaseTextureProjector_Direct3D9::InitializeProjector_Direct3D9(IDirect3DDevice9 * device, uint32 hmdWidth, uint32 hmdHeight,
	const LVMatrix & leftView, const LVMatrix & leftProj, const LVMatrix & rightView, const LVMatrix & rightProj)
{
	const CHAR* head = "TextureProjector::InitializeProjector_Direct3D9";

	DestroyRHI();

	RecommendWidth = hmdWidth;
	RecommendHeight = hmdHeight;

	LVMSG(head, "%s", GetDescriptionFromDevice(device).c_str());

	if (!InitializeRenderer_Direct3D9(device))
	{
		LVMSG(head, "initialize renderer failed");
		return false;
	}

	SetEyePose(SEnumEyeL, leftView, leftProj);
	SetEyePose(SEnumEyeR, rightView, rightProj);

	bool ret = InitializeRHI() && InitializeTextureSRV();
	return ret;
}
