#pragma once

#include "OVR_CAPI.h"
#include "OVR_CAPI_D3D.h"
#include "OVR_Version.h"
#include "OVR_CAPI_Keys.h"
#include "OVR_ErrorCode.h"

#define NUMEYES 1

namespace lostvr
{
	static inline void PrintSwapChainDesc(DXGI_SWAP_CHAIN_DESC desc)
	{
		LVMSG("DXGI_SWAP_CHAIN_DESC", "BufferCount(%d), BufferDesc.Width(%d), BufferDesc.Height(%d), BufferDesc.Format(%d), BufferDesc.RefreshRate.Numerator(%d), ", desc.BufferCount, desc.BufferDesc.Width, desc.BufferDesc.Height, desc.BufferDesc.Format, desc.BufferDesc.RefreshRate.Numerator);
		LVMSG("DXGI_SWAP_CHAIN_DESC", "BufferDesc.RefreshRate.Denominator(%d), BufferUsage(%d), OutputWindow(%x), SampleDesc.Count(%d), SampleDesc.Quality(%d), SwapEffect(%d), Flags(%d)", desc.BufferDesc.RefreshRate.Denominator, (int)desc.BufferUsage, desc.OutputWindow, (int)desc.SampleDesc.Count, (int)desc.SampleDesc.Quality, (int)desc.SwapEffect, (int)desc.Flags);
	}

	static inline void PrintOvrSwapChainDesc(ovrTextureSwapChainDesc desc)
	{
		LVMSG("ovrTextureSwapChainDesc", "Type(%d), ArraySize(%d), Format(%d), Width(%d), Height(%d), ", desc.Type, desc.ArraySize, desc.Format, desc.Width, desc.Height);
		LVMSG("ovrTextureSwapChainDesc", "MipLevels(%d), SampleCount(%d), StaticImage(%x), MiscFlags(%d), BindFlags(%d)", desc.MipLevels, (int)desc.SampleCount, (int)desc.StaticImage, (int)desc.MiscFlags, (int)desc.BindFlags);
	}

	static inline void PrintOvrLayerEyeFov(ovrLayerEyeFov layer)
	{
		//typedef struct OVR_ALIGNAS(OVR_PTR_SIZE) ovrLayerHeader_
		//{
		//	ovrLayerType    Type;   ///< Described by ovrLayerType.
		//	unsigned        Flags;  ///< Described by ovrLayerFlags.
		//} ovrLayerHeader;    /// Header.Type must be ovrLayerType_EyeFov.
		//ovrLayerHeader      Header;

		///// ovrTextureSwapChains for the left and right eye respectively.
		///// The second one of which can be NULL for cases described above.
		//ovrTextureSwapChain  ColorTexture[ovrEye_Count];

		///// Specifies the ColorTexture sub-rect UV coordinates.
		///// Both Viewport[0] and Viewport[1] must be valid.
		//ovrRecti            Viewport[ovrEye_Count];

		///// The viewport field of view.
		//ovrFovPort          Fov[ovrEye_Count];

		///// Specifies the position and orientation of each eye view, with the position specified in meters.
		///// RenderPose will typically be the value returned from ovr_CalcEyePoses,
		///// but can be different in special cases if a different head pose is used for rendering.
		//ovrPosef            RenderPose[ovrEye_Count];

		///// Specifies the timestamp when the source ovrPosef (used in calculating RenderPose)
		///// was sampled from the SDK. Typically retrieved by calling ovr_GetTimeInSeconds
		///// around the instant the application calls ovr_GetTrackingState
		///// The main purpose for this is to accurately track app tracking latency.
		//double              SensorSampleTime;
		LVMSG("ovrLayerEyeFov", "Header.Type(%d), Header.Flags(%d), ColorTexture0(%x), ColorTexture1(%x)", layer.Header.Type, layer.Header.Flags, layer.ColorTexture[0], layer.ColorTexture[1]);
		LVMSG("ovrLayerEyeFov", "Viewport[0](%d, %d, %d, %d)", layer.Viewport[0].Pos.x, layer.Viewport[0].Pos.y, layer.Viewport[0].Size.w, layer.Viewport[0].Size.h);
		LVMSG("ovrLayerEyeFov", "RenderPose[0](%f, %f, %f, %f)", layer.RenderPose[0].Orientation.x, layer.RenderPose[0].Orientation.y, layer.RenderPose[0].Orientation.z, layer.RenderPose[0].Orientation.w);
	}

	//------------------------------------------------------------
	struct OculusTexture
	{
		ovrSession               Session;
		ovrTextureSwapChain      TextureChain;
		std::vector<ID3D11Texture2D*> SwapChainBuffer;
		int                      SizeW, SizeH;
		ovrTextureFormat Format;

		OculusTexture(DXGI_SWAP_CHAIN_DESC desc) :
			Session(nullptr),
			TextureChain(nullptr),
			SizeW(desc.BufferDesc.Width),
			SizeH(desc.BufferDesc.Height)
		{
			if (desc.BufferDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM)
			{
				Format = OVR_FORMAT_R8G8B8A8_UNORM;
			}
			else if (desc.BufferDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			{
				Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
			}
			else
			{
				LVMSG("OculusTexture", "unrecognized format(%d)", desc.BufferDesc.Format);
			}
		}

		bool Init(ID3D11Device* dev, ovrSession session, int sizeW, int sizeH, bool isItProtectedContent = false)
		{
			Session = session;
			//SizeW = sizeW;
			//SizeH = sizeH;
			//Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

			ovrTextureSwapChainDesc desc = {};
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.Format = Format;
			desc.Width = SizeW;
			desc.Height = SizeH;
			desc.MipLevels = 1;
			desc.SampleCount = 1;
			desc.StaticImage = ovrFalse;
			desc.MiscFlags = ovrTextureMisc_DX_Typeless;
			if (isItProtectedContent) desc.MiscFlags |= ovrTextureMisc_ProtectedContent;
			desc.BindFlags = ovrTextureBind_DX_RenderTarget;

			PrintOvrSwapChainDesc(desc);
			ovrResult result = ovr_CreateTextureSwapChainDX(Session, dev, &desc, &TextureChain);
			if (!OVR_SUCCESS(result))
			{
				LVMSG("OculusTexture::Init", "ovr_CreateTextureSwapChainDX failed(%d)", (int)result);
				return false;
			}

			int textureCount = 0;
			ovr_GetTextureSwapChainLength(Session, TextureChain, &textureCount);
			for (int i = 0; i < textureCount; ++i)
			{
				ID3D11Texture2D* tex = nullptr;
				ovr_GetTextureSwapChainBufferDX(Session, TextureChain, i, IID_PPV_ARGS(&tex));
				if (tex == nullptr)
				{
					LVMSG("OculusTexture::Init", "ovr_GetTextureSwapChainBufferDX failed");
				}

				SwapChainBuffer.push_back(tex);
			}

			LVMSG("OculusTexture::Init", "sucess, width(%d), height(%d), dev(%x)", sizeW, sizeH, dev);
			return true;
		}

		~OculusTexture()
		{
			for (int i = 0; i < (int)SwapChainBuffer.size(); ++i)
			{
				if (SwapChainBuffer[i] != nullptr)
				{
					SwapChainBuffer[i]->Release();
					SwapChainBuffer[i] = nullptr;
				}
			}
			if (TextureChain)
			{
				ovr_DestroyTextureSwapChain(Session, TextureChain);
				TextureChain = nullptr;
			}
		}

		ID3D11Texture2D* GetBuffer()
		{
			if (TextureChain)
			{
				int currentIndex = 0;
				ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &currentIndex);
				//LVMSG("GetBuffer", "curr index: %d", currentIndex);
				return SwapChainBuffer[currentIndex];
			}
			return nullptr;
		}

		void Commit()
		{
			ovr_CommitTextureSwapChain(Session, TextureChain);
		}
	};

	//----------------------------------------------------------------------
	struct VRLayer
	{
		ovrSession                  Session;
		ovrEyeRenderDesc            EyeRenderDesc[NUMEYES];        // Description of the VR.
		ovrRecti                    EyeRenderViewport[NUMEYES];    // Useful to remember when varying resolution
		OculusTexture             * pEyeRenderTexture[NUMEYES];    // Where the eye buffers will be rendered
		ovrPosef                    EyeRenderPose[NUMEYES];
		ovrLayerEyeFov              ovrLayer;

		//---------------------------------------------------------------
		VRLayer(ID3D11Device* dev, DXGI_SWAP_CHAIN_DESC desc, ovrSession session, const ovrFovPort * fov = 0, float pixelsPerDisplayPixel = 1.0f, bool isItProtectedContent = false)
		{
			Session = session;
			MakeEyeBuffers(dev, desc, pixelsPerDisplayPixel, isItProtectedContent);
			ConfigureRendering(fov);
		}
		~VRLayer()
		{
			for (int eye = 0; eye < NUMEYES; ++eye)
			{
				delete pEyeRenderTexture[eye];
				pEyeRenderTexture[eye] = nullptr;
			}
		}

		//-----------------------------------------------------------------------
		void MakeEyeBuffers(ID3D11Device* dev, DXGI_SWAP_CHAIN_DESC desc, float pixelsPerDisplayPixel = 1.0f, bool isItProtectedContent = false)
		{
			for (int eye = 0; eye < NUMEYES; ++eye)
			{
				ovrFovPort fp = ovr_GetHmdDesc(Session).DefaultEyeFov[eye];
				ovrSizei idealSize = ovr_GetFovTextureSize(Session, (ovrEyeType)eye, fp, pixelsPerDisplayPixel);
				pEyeRenderTexture[eye] = new OculusTexture(desc);
				LVMSG("MakeEyeBuffers*****************", "w(%d), h(%d), fov(%f, %f, %f, %f), Session(%x), pixelsPerdisplayPixel(%f)", 
					idealSize.w, idealSize.h, fp.DownTan, fp.LeftTan, fp.RightTan,fp.UpTan, Session, pixelsPerDisplayPixel);
				if (!pEyeRenderTexture[eye]->Init(dev, Session, idealSize.w, idealSize.h, isItProtectedContent))
					return;

				float dWidth = 400.f;
				EyeRenderViewport[eye].Pos.x = dWidth * -0.5f;
				EyeRenderViewport[eye].Pos.y = 0;
				idealSize.w = pEyeRenderTexture[0]->SizeW + dWidth;
				idealSize.h = pEyeRenderTexture[0]->SizeH;
				EyeRenderViewport[eye].Size = idealSize;
			}
		}

		//--------------------------------------------------------
		void ConfigureRendering(const ovrFovPort * fov = 0)
		{
			// If any values are passed as NULL, then we use the default basic case
			if (!fov) fov = ovr_GetHmdDesc(Session).DefaultEyeFov;
			for (int eye = 0; eye < NUMEYES; ++eye)
				EyeRenderDesc[eye] = ovr_GetRenderDesc(Session, (ovrEyeType)eye, fov[eye]);
		}

		//------------------------------------------------------------
		ovrTrackingState GetEyePoses(ovrPosef * useEyeRenderPose = 0, float * scaleIPD = 0, float * newIPD = 0)
		{
			// Get both eye poses simultaneously, with IPD offset already included. 
			ovrVector3f useHmdToEyeOffset[NUMEYES];
			for (int eye = 0; eye < NUMEYES; ++eye)
				useHmdToEyeOffset[eye] = EyeRenderDesc[eye].HmdToEyeOffset;

			// If any values are passed as NULL, then we use the default basic case
			if (!useEyeRenderPose) useEyeRenderPose = EyeRenderPose;
			if (scaleIPD)
			{
				useHmdToEyeOffset[0].x *= *scaleIPD;
			}
			if (newIPD)
			{
				useHmdToEyeOffset[0].x = -(*newIPD * 0.5f);
			}

			double ftiming = ovr_GetPredictedDisplayTime(Session, 0);
			ovrTrackingState trackingState = ovr_GetTrackingState(Session, ftiming, ovrTrue);

			ovr_CalcEyePoses(trackingState.HeadPose.ThePose, useHmdToEyeOffset, useEyeRenderPose);
			
			ovrLayer.Header.Type = ovrLayerType_EyeFov;
			ovrLayer.Header.Flags = 0;
			ovrLayer.ColorTexture[0] = pEyeRenderTexture[0]->TextureChain;
			ovrLayer.ColorTexture[1] = pEyeRenderTexture[0]->TextureChain;
			ovrLayer.RenderPose[0] = EyeRenderPose[0];
			ovrLayer.RenderPose[1] = EyeRenderPose[0];
			ovrLayer.Fov[0] = EyeRenderDesc[0].Fov;
			ovrLayer.Fov[1] = EyeRenderDesc[0].Fov;
			ovrLayer.Viewport[0] = EyeRenderViewport[0];
			ovrLayer.Viewport[1] = EyeRenderViewport[0];

			return(trackingState);
		}

		//-----------------------------------------------------------
		void Commit(int eye)
		{
			// Commit rendering to the swap chain so it can be picked up by the SDK
			pEyeRenderTexture[eye]->Commit();
		}

	};

	class OculusVR
	{
		ovrSession                    Session;
		ovrHmdDesc                    HmdDesc;
		std::vector<ovrTrackerDesc>   TrackerDescArray;
		ovrResult                     presentResult;
		bool                          bInitialized;
		VRLayer    *                  Layer[ovrMaxLayerCount];

	public:
		static OculusVR* Get() { static OculusVR Inst; return &Inst; }

		OculusVR() :
			Session(nullptr),
			HmdDesc(),
			TrackerDescArray(),
			presentResult(ovrSuccess),
			bInitialized(false)
		{
			// Set all layers to zero
			for (int i = 0; i < ovrMaxLayerCount; ++i)
				Layer[i] = nullptr;

			// Initializes LibOVR, and the Rift
			ovrResult result = ovr_Initialize(nullptr);
			if (!OVR_SUCCESS(result))
			{
				ovrErrorInfo errorInfo;
				ovr_GetLastErrorInfo(&errorInfo);
				LVMSG("OculusVR Init", "ovr_Initialize failed, error message: %d, %s", errorInfo.Result, errorInfo.ErrorString);
			}
			else
			{
				LVMSG("OculusVR Init", "ovr_Initialize succeeded");
			}

			ovrGraphicsLuid luid;
			result = ovr_Create(&Session, &luid);
			if (!OVR_SUCCESS(result))
			{
				ovrErrorInfo errorInfo;
				ovr_GetLastErrorInfo(&errorInfo);
				LVMSG("OculusVR Init", "ovr_Create failed, error message: %d, %s", errorInfo.Result, errorInfo.ErrorString);
			}
			else
			{
				LVMSG("OculusVR Init", "ovr_Create succeeded");
			}
		}

		~OculusVR()
		{
			Release();
			ovr_Shutdown();
		}

		bool IsDeviceConnected() { return Session != nullptr; }

		void GetSwapChainData(DXGI_FORMAT& outFormat, int& sizeW, int& sizeH)
		{
			ovrSizei idealSize = ovr_GetFovTextureSize(Session, (ovrEyeType)0, ovr_GetHmdDesc(Session).DefaultEyeFov[0], 1.f);
			outFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			sizeW = idealSize.w;
			sizeH = idealSize.h;
			LVMSG("OculusVR::GetSwapChainData ***************************", "session(%x), format(%d), w(%d), h(%d)", Session, outFormat, sizeW, sizeH);
		}

		bool Init(ID3D11Device* Dev, DXGI_SWAP_CHAIN_DESC Desc)
		{
			HmdDesc = ovr_GetHmdDesc(Session);

			unsigned int trackerCount = ovr_GetTrackerCount(Session) < 1 ? 1 : ovr_GetTrackerCount(Session); // Make sure there's always at least one.
			for (unsigned int i = 0; i < trackerCount; ++i)
				TrackerDescArray.push_back(ovr_GetTrackerDesc(Session, i));

			{
				Layer[0] = new VRLayer(Dev, Desc, Session);
			}

			LVMSG("OculusVR Init", "sucess");
			return true;
		}    

		void OnPresent(IDXGISwapChain* SwapChain)
		{
			if (SwapChain == nullptr)
			{
				LVMSG("OculusVR::OnPresent", "null SwapChain");
				return;
			}

			ID3D11Device* Dev = nullptr;
			ID3D11Texture2D* Buf = nullptr;
			ID3D11DeviceContext* Context = nullptr;
			if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D*), (void**)&Buf)) ||
				FAILED(SwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Dev)))
			{
				LVMSG("OculusVR::OnPresent", "failed to get device or buffer, SwapChain(%x).", SwapChain);
				return;
			}

			Dev->GetImmediateContext(&Context);
			DXGI_SWAP_CHAIN_DESC Desc;
			if (FAILED(SwapChain->GetDesc(&Desc)))
			{
				LVMSG("OculusVR::OnPresent", "failed to get SwapChain's desc, SwapChain(%x).", SwapChain);
				return;
			}
			else
			{
				LVMSG("OculusVR::OnPresent", "format(%d), w(%d), h(%d), buff count(%d)", Desc.BufferDesc.Format, Desc.BufferDesc.Width, Desc.BufferDesc.Height, Desc.BufferCount);
			}

			if (!bInitialized)
			{
				if (!(bInitialized = Init(Dev, Desc)))
				{
					LVMSG("OculusVR::OnPresent", "failed to initialize, SwapChain(%x).", SwapChain);
					return;
				}
			}

			if (Context != nullptr)
			{
				Layer[0]->GetEyePoses();
				ID3D11Texture2D* dst = Layer[0]->pEyeRenderTexture[0]->GetBuffer();
				D3D11_TEXTURE2D_DESC SrcDesc, DstDesc;
				dst->GetDesc(&DstDesc);
				Buf->GetDesc(&SrcDesc);
				//LVMSG("OculusVR::OnPresent", "Src(%x): format(%d), w(%d), h(%d), buff count(%d)", Buf, Desc.BufferDesc.Format, Desc.BufferDesc.Width, Desc.BufferDesc.Height, Desc.BufferCount);
				LVMSG("OculusVR::OnPresent", "Src(%x): format(%d), w(%d), h(%d)", Buf, SrcDesc.Format, SrcDesc.Width, SrcDesc.Height);
				LVMSG("OculusVR::OnPresent", "Dst(%x): format(%d), w(%d), h(%d)", dst, DstDesc.Format, DstDesc.Width, DstDesc.Height);
				Context->CopyResource(dst, Buf);
				Layer[0]->Commit(0);
				ovrResult ret = DistortAndPresent(1);
				if (!OVR_SUCCESS(ret))
				{
					LVMSG("OculusVR::OnPresent", "failed, reinitializing...");
					Release();
					if (Init(Dev, Desc))
					{
						LVMSG("OculusVR::OnPresent", "reinitialize sucess");
					}
					else
					{
						LVMSG("OculusVR::OnPresent", "reinitialize fail");
					}
				}
			}
			else
			{
				LVMSG("OculusVR::OnPresent", "failed to get context.", Dev, Buf);
			}

			Context->Release();
		}
		
		int Release()
		{
			bInitialized = false;

			for (int i = 0; i < ovrMaxLayerCount; ++i)
			{
				delete Layer[i];
				Layer[i] = nullptr;
			}

			//if (Session)
			//{
			//	ovr_Destroy(Session);
			//	Session = nullptr;
			//}

			return(0);
		}

		//------------------------------------------------------------
		ovrResult DistortAndPresent(int numLayersToRender, D3D11_BOX * optionalBoxForMirrorWindow = 0, bool mirror = true)
		{
			ovrLayerHeader* layerHeaders[ovrMaxLayerCount];
			for (int i = 0; i < ovrMaxLayerCount; i++)
			{
				if (Layer[i])
				{
					PrintOvrLayerEyeFov(Layer[i]->ovrLayer);
					layerHeaders[i] = &(Layer[i]->ovrLayer.Header);
				}
			}

			presentResult = ovr_SubmitFrame(Session, 0, nullptr, layerHeaders, numLayersToRender);
			if (!OVR_SUCCESS(presentResult))
			{
				LVMSG("ovr_SubmitFrame", "layers(%d), result(%d)", (int)ovrMaxLayerCount, presentResult);
				return(presentResult);
			}

			return(presentResult);
		}
	};
};