#pragma once

#include "resource.h"

#include "Renderer.h"

static const UINT SFrameNum = 3;

namespace lostvr 
{
	class LostVideoBuffer
	{
	public:
		bool						bLastRead;
		int							NextWriteIndex;
		int							NextReadIndex;
		uint8*						FrameBuffer[SFrameNum];
		UINT						BufferSize;

		UINT						Width;
		UINT						Height;
		DXGI_FORMAT					Format;

		ID3D11Texture2D*			Texture;
		ID3D11ShaderResourceView*	SRV;

		LostVideoBuffer(Direct3D11Helper* renderer, UINT w, UINT h, DXGI_FORMAT format) : bLastRead(true)
			, NextWriteIndex(0)
			, NextReadIndex(0)
			, Width(w)
			, Height(h)
			, Format(format)
			, Texture(nullptr)
			, SRV(nullptr)
		{
			UINT bytes = 0;
			switch (format)
			{
			case DXGI_FORMAT_UNKNOWN:
				break;
			case DXGI_FORMAT_R32G32B32A32_TYPELESS:
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
				bytes = 16;
				break;
			case DXGI_FORMAT_R32G32B32_TYPELESS:
			case DXGI_FORMAT_R32G32B32_FLOAT:
			case DXGI_FORMAT_R32G32B32_UINT:
			case DXGI_FORMAT_R32G32B32_SINT:
				bytes = 24;
				break;
			case DXGI_FORMAT_R16G16B16A16_TYPELESS:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_UINT:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R16G16B16A16_SINT:
			case DXGI_FORMAT_R32G32_TYPELESS:
			case DXGI_FORMAT_R32G32_FLOAT:
			case DXGI_FORMAT_R32G32_UINT:
			case DXGI_FORMAT_R32G32_SINT:
				bytes = 8;
				break;
			case DXGI_FORMAT_R32G8X24_TYPELESS:
				break;
			case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
				break;
			case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
				break;
			case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
				break;
			case DXGI_FORMAT_R10G10B10A2_TYPELESS:
			case DXGI_FORMAT_R10G10B10A2_UNORM:
			case DXGI_FORMAT_R10G10B10A2_UINT:
			case DXGI_FORMAT_R11G11B10_FLOAT:
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			case DXGI_FORMAT_R8G8B8A8_UINT:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_R8G8B8A8_SINT:
			case DXGI_FORMAT_R16G16_TYPELESS:
			case DXGI_FORMAT_R16G16_FLOAT:
			case DXGI_FORMAT_R16G16_UNORM:
			case DXGI_FORMAT_R16G16_UINT:
			case DXGI_FORMAT_R16G16_SNORM:
			case DXGI_FORMAT_R16G16_SINT:
			case DXGI_FORMAT_R32_TYPELESS:
			case DXGI_FORMAT_D32_FLOAT:
			case DXGI_FORMAT_R32_FLOAT:
			case DXGI_FORMAT_R32_UINT:
			case DXGI_FORMAT_R32_SINT:
			case DXGI_FORMAT_R24G8_TYPELESS:
			case DXGI_FORMAT_D24_UNORM_S8_UINT:
				bytes = 4;
				break;
			case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
				break;
			case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
				break;
			case DXGI_FORMAT_R8G8_TYPELESS:
				break;
			case DXGI_FORMAT_R8G8_UNORM:
				break;
			case DXGI_FORMAT_R8G8_UINT:
				break;
			case DXGI_FORMAT_R8G8_SNORM:
				break;
			case DXGI_FORMAT_R8G8_SINT:
				break;
			case DXGI_FORMAT_R16_TYPELESS:
				break;
			case DXGI_FORMAT_R16_FLOAT:
				break;
			case DXGI_FORMAT_D16_UNORM:
				break;
			case DXGI_FORMAT_R16_UNORM:
				break;
			case DXGI_FORMAT_R16_UINT:
				break;
			case DXGI_FORMAT_R16_SNORM:
				break;
			case DXGI_FORMAT_R16_SINT:
				break;
			case DXGI_FORMAT_R8_TYPELESS:
			case DXGI_FORMAT_R8_UNORM:
			case DXGI_FORMAT_R8_UINT:
			case DXGI_FORMAT_R8_SNORM:
			case DXGI_FORMAT_R8_SINT:
			case DXGI_FORMAT_A8_UNORM:
				bytes = 1;
				break;
			case DXGI_FORMAT_R1_UNORM:
				break;
			case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
				break;
			case DXGI_FORMAT_R8G8_B8G8_UNORM:
				break;
			case DXGI_FORMAT_G8R8_G8B8_UNORM:
				break;
			case DXGI_FORMAT_BC1_TYPELESS:
				break;
			case DXGI_FORMAT_BC1_UNORM:
				break;
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC2_TYPELESS:
				break;
			case DXGI_FORMAT_BC2_UNORM:
				break;
			case DXGI_FORMAT_BC2_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC3_TYPELESS:
				break;
			case DXGI_FORMAT_BC3_UNORM:
				break;
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC4_TYPELESS:
				break;
			case DXGI_FORMAT_BC4_UNORM:
				break;
			case DXGI_FORMAT_BC4_SNORM:
				break;
			case DXGI_FORMAT_BC5_TYPELESS:
				break;
			case DXGI_FORMAT_BC5_UNORM:
				break;
			case DXGI_FORMAT_BC5_SNORM:
				break;
			case DXGI_FORMAT_B5G6R5_UNORM:
				break;
			case DXGI_FORMAT_B5G5R5A1_UNORM:
				break;
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				break;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				break;
			case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
				break;
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
				bytes = 4;
				break;
			case DXGI_FORMAT_B8G8R8X8_TYPELESS:
				break;
			case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
				break;
			case DXGI_FORMAT_BC6H_TYPELESS:
				break;
			case DXGI_FORMAT_BC6H_UF16:
				break;
			case DXGI_FORMAT_BC6H_SF16:
				break;
			case DXGI_FORMAT_BC7_TYPELESS:
				break;
			case DXGI_FORMAT_BC7_UNORM:
				break;
			case DXGI_FORMAT_BC7_UNORM_SRGB:
				break;
			case DXGI_FORMAT_AYUV:
				break;
			case DXGI_FORMAT_Y410:
				break;
			case DXGI_FORMAT_Y416:
				break;
			case DXGI_FORMAT_NV12:
				break;
			case DXGI_FORMAT_P010:
				break;
			case DXGI_FORMAT_P016:
				break;
			case DXGI_FORMAT_420_OPAQUE:
				break;
			case DXGI_FORMAT_YUY2:
				break;
			case DXGI_FORMAT_Y210:
				break;
			case DXGI_FORMAT_Y216:
				break;
			case DXGI_FORMAT_NV11:
				break;
			case DXGI_FORMAT_AI44:
				break;
			case DXGI_FORMAT_IA44:
				break;
			case DXGI_FORMAT_P8:
				break;
			case DXGI_FORMAT_A8P8:
				break;
			case DXGI_FORMAT_B4G4R4A4_UNORM:
				break;
			case DXGI_FORMAT_FORCE_UINT:
				break;
			default:
				break;
			}

			if (bytes == 0)
			{
				return;
			}

			//BufferSize = w * h * bytes;
			BufferSize = w * (h);

			for (int i = 0; i < SFrameNum; ++i)
			{
				FrameBuffer[i] = new uint8[BufferSize];
			}

			D3D11_TEXTURE2D_DESC desc;
			renderer->GetDescriptionTemplate_DefaultTexture2D(desc);
			desc.Width = w;
			desc.Height = h;
			desc.Format = format;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;
			renderer->CreateTexture2D(&Texture, &SRV, &desc);
		}

		~LostVideoBuffer()
		{
			SAFE_RELEASE(Texture);
			SAFE_RELEASE(SRV);
			for (int i = 0; i < SFrameNum; ++i)
			{
				if (FrameBuffer[i] != nullptr)
				{
					delete[] FrameBuffer[i];
				}
			}
		}

		bool IsValid()
		{
			return BufferSize != 0;
		}

		bool IsMatch(UINT w, UINT h, DXGI_FORMAT format)
		{
			return (w == Width && h == Height && format == Format);
		}

		bool CanWrite()
		{
			return NextWriteIndex < SFrameNum && 0 <= NextWriteIndex && (NextWriteIndex != NextReadIndex || bLastRead);
		}

		bool CanRead()
		{
			return NextReadIndex < SFrameNum && 0 <= NextReadIndex && (NextWriteIndex != NextReadIndex || !bLastRead);
		}

		void IncrementWrite()
		{
			NextWriteIndex = (1 + NextWriteIndex) % SFrameNum;
			bLastRead = false;
		}

		void IncrementRead()
		{
			NextReadIndex = (1 + NextReadIndex) % SFrameNum;
			bLastRead = true;
		}

		void Write(const uint8* buf, UINT sz = 0)
		{
			if (sz == 0 || sz == BufferSize)
			{
				memcpy(FrameBuffer[NextWriteIndex], buf, BufferSize);
				IncrementWrite();
			}
		}

		ID3D11ShaderResourceView* Read()
		{
			ID3D11Device* dev;
			Texture->GetDevice(&dev);

			ID3D11DeviceContext* context;
			dev->GetImmediateContext(&context);

			D3D11_MAPPED_SUBRESOURCE mapped;
			if (SUCCEEDED(context->Map(Texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
			{
				memcpy(mapped.pData, FrameBuffer[NextReadIndex], BufferSize);
				IncrementRead();
				context->Unmap(Texture, 0);
				return SRV;
			}
			else
			{
				LVERROR("LostVideoBuffer::Read", "Map failed");
				return nullptr;
			}
		}
	};

	class LostMediaPlayer
	{
	public:
		LostMediaPlayer(HWND hwnd);

		void Initialize(HWND hwnd);
		void Tick();

	protected:
		UINT Width;
		UINT Height;

		Direct3D11Helper* Renderer;

		// renderer properties
		ID3D11RenderTargetView*		RTV;
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

	public:
		bool CanWrite(UINT w, UINT h, DXGI_FORMAT format);
		void ProcessVideoBuffer(const uint8* buf, UINT sz = 0);
		
	protected:
		LostVideoBuffer*			Frames;
	};
}

using namespace lostvr;

LostMediaPlayer::LostMediaPlayer(HWND hwnd) : Renderer(nullptr)
, Frames(nullptr)
{
	Initialize(hwnd);
}

void LostMediaPlayer::Initialize(HWND hwnd)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);
	Width = rect.right - rect.left;
	Height = rect.bottom - rect.top;

	SGlobalSharedDataInst.SetDefaultWindow(hwnd);

	Renderer = new Direct3D11Helper(EDirect3D::DXGI1, Width, Height, DXGI_FORMAT_R8G8B8A8_UNORM);

	Renderer->CreateRenderTargetView(nullptr, &RTV);

	{
		D3D11_DEPTH_STENCIL_DESC desc{ 0 };
		desc.DepthEnable = FALSE;
		desc.StencilEnable = FALSE;
		Renderer->GetDevice()->CreateDepthStencilState(&desc, &DSS);
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
		Renderer->GetDevice()->CreateSamplerState(&desc, &Sampler);
	}
	
	{
		D3D11_BUFFER_DESC bdesc;
		Renderer->GetDescriptionTemplate_DefaultBuffer(bdesc);
		bdesc.ByteWidth = sizeof(FrameBufferWVP);
		Renderer->CreateBuffer(bdesc, &WVPCB);
	}

	Renderer->GetDefaultRasterizer(&RS);
	Renderer->CreateMesh_Rect(2.0f, 2.0f, sizeof(MeshVertex), &VB, &IB);
	Renderer->GetDefaultShader_MediaPlayer(&VS, &PS, &VL);
}

void LostMediaPlayer::Tick()
{
	if (Renderer == nullptr)
	{
		return;
	}

	if (Frames == nullptr || !Frames->IsValid() || !Frames->CanRead())
	{
		return;
	}

	ID3D11DeviceContext* context = Renderer->GetContext();
	D3D11_VIEWPORT Viewport{ 0.0f, 0.0f, (float)Width, (float)Height, 0.0f, 1.0f };
	context->RSSetViewports(1, &Viewport);
	context->RSSetState(RS);

	context->OMSetRenderTargets(1, &RTV, nullptr);
	context->ClearRenderTargetView(RTV, DirectX::Colors::Orange);		

	uint32 stride = sizeof(MeshVertex);
	uint32 offset = 0;
	context->IASetIndexBuffer(IB, DXGI_FORMAT_R16_UINT, 0);
	context->IASetVertexBuffers(0, 1, &VB, &stride, &offset);
	context->IASetInputLayout(VL);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// vertex shader
	context->VSSetShader(VS, nullptr, 0);

	// pixel shader
	context->PSSetShader(PS, nullptr, 0);
	context->PSSetSamplers(0, 1, &Sampler);

	ID3D11ShaderResourceView* srvs[] = { Frames->Read() };
	context->PSSetShaderResources(0, 1, &srvs[0]);

	context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

	context->DrawIndexed(6, 0, 0);

	Renderer->GetSwapChain()->Present(0, 0);
}

bool LostMediaPlayer::CanWrite(UINT w, UINT h, DXGI_FORMAT format)
{
	if (Frames != nullptr && !Frames->IsMatch(w, h, format))
	{
		delete Frames;
		Frames = nullptr;
	}

	if (Frames == nullptr)
	{
		Frames = new LostVideoBuffer(Renderer, w, h, format);
	}

	return Frames->CanWrite();
}

void LostMediaPlayer::ProcessVideoBuffer(const uint8 * buf, UINT sz)
{
	if (Frames == nullptr || !Frames->IsValid())
	{
		return;
	}

	Frames->Write(buf, sz);
}
