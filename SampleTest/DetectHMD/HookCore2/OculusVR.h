#pragma once

#include "oculus-lib/Include/OVR_CAPI.h"
#include "oculus-lib/Include/OVR_CAPI_D3D.h"
#include "oculus-lib/Include/OVR_Version.h"
#include "oculus-lib/Include/OVR_CAPI_Keys.h"
#include "oculus-lib/Include/OVR_ErrorCode.h"

#define NUMEYES 1

namespace LostVR
{
	//------------------------------------------------------------
	struct OculusTexture
	{
		ovrSession               Session;
		ovrTextureSwapChain      TextureChain;
		std::vector<ID3D11RenderTargetView*> TexRtv;
		std::vector<ID3D11Texture2D*> SwapChainBuffer;
		int                      SizeW, SizeH;

		OculusTexture() :
			Session(nullptr),
			TextureChain(nullptr),
			SizeW(0),
			SizeH(0)
		{
		}

		bool Init(ID3D11Device* dev, ovrSession session, int sizeW, int sizeH, bool isItProtectedContent = false)
		{
			Session = session;
			SizeW = sizeW;
			SizeH = sizeH;

			ovrTextureSwapChainDesc desc = {};
			desc.Type = ovrTexture_2D;
			desc.ArraySize = 1;
			desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
			desc.Width = sizeW;
			desc.Height = sizeH;
			desc.MipLevels = 1;
			desc.SampleCount = 1;
			desc.StaticImage = ovrFalse;
			desc.MiscFlags = ovrTextureMisc_DX_Typeless;
			if (isItProtectedContent) desc.MiscFlags |= ovrTextureMisc_ProtectedContent;
			desc.BindFlags = ovrTextureBind_DX_RenderTarget;

			ovrResult result = ovr_CreateTextureSwapChainDX(Session, dev, &desc, &TextureChain);
			if (!OVR_SUCCESS(result))
			{
				LVMSG("OculusTexture::Init", "ovr_CreateTextureSwapChainDX failed");
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
				//D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
				//rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				//rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				//ID3D11RenderTargetView* rtv;
				//dev->CreateRenderTargetView(tex, &rtvd, &rtv);
				//TexRtv.push_back(rtv);
				//tex->Release();
			}

			LVMSG("OculusTexture::Init", "sucess, width(%d), height(%d)", sizeW, sizeH);
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

		ID3D11RenderTargetView* GetRTV()
		{
			if (TextureChain)
			{
				int currentIndex = 0;
				ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &currentIndex);
				return TexRtv[currentIndex];
			}
			return nullptr;
		}

		ID3D11Texture2D* GetBuffer()
		{
			if (TextureChain)
			{
				int currentIndex = 0;
				ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &currentIndex);
				LVMSG("GetBuffer", "curr index: %d", currentIndex);
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
		VRLayer(ID3D11Device* dev, ovrSession session, const ovrFovPort * fov = 0, float pixelsPerDisplayPixel = 1.0f, bool isItProtectedContent = false)
		{
			Session = session;
			MakeEyeBuffers(dev, pixelsPerDisplayPixel, isItProtectedContent);
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
		void MakeEyeBuffers(ID3D11Device* dev, float pixelsPerDisplayPixel = 1.0f, bool isItProtectedContent = false)
		{
			for (int eye = 0; eye < NUMEYES; ++eye)
			{
				ovrSizei idealSize = ovr_GetFovTextureSize(Session, (ovrEyeType)eye, ovr_GetHmdDesc(Session).DefaultEyeFov[eye], pixelsPerDisplayPixel);
				pEyeRenderTexture[eye] = new OculusTexture();
				if (!pEyeRenderTexture[eye]->Init(dev, Session, idealSize.w, idealSize.h, isItProtectedContent))
					return;
				EyeRenderViewport[eye].Pos.x = 0;
				EyeRenderViewport[eye].Pos.y = 0;
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
				useHmdToEyeOffset[1].x *= *scaleIPD;
			}
			if (newIPD)
			{
				useHmdToEyeOffset[0].x = -(*newIPD * 0.5f);
				useHmdToEyeOffset[1].x = +(*newIPD * 0.5f);
			}

			double ftiming = ovr_GetPredictedDisplayTime(Session, 0);
			ovrTrackingState trackingState = ovr_GetTrackingState(Session, ftiming, ovrTrue);

			ovr_CalcEyePoses(trackingState.HeadPose.ThePose, useHmdToEyeOffset, useEyeRenderPose);

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
		bool                          initialized;
		VRLayer    *                  Layer[ovrMaxLayerCount];

	public:
		static OculusVR* Get() { static OculusVR Inst; return &Inst; }

		OculusVR() :
			Session(nullptr),
			HmdDesc(),
			TrackerDescArray(),
			presentResult(ovrSuccess),
			initialized(false)
		{
			// Set all layers to zero
			for (int i = 0; i < ovrMaxLayerCount; ++i)
				Layer[i] = nullptr;

			// Initializes LibOVR, and the Rift
			ovrResult result = ovr_Initialize(nullptr);
		}

		~OculusVR()
		{
			Release();
			ovr_Shutdown();
		}

		bool Init(ID3D11Device* dev)
		{
			ovrGraphicsLuid luid;
			ovrResult result = ovr_Create(&Session, &luid);
			if (!OVR_SUCCESS(result))
			{
				LVMSG("OculusVR Init", "fail");
				return false;
			}

			HmdDesc = ovr_GetHmdDesc(Session);

			unsigned int trackerCount = ovr_GetTrackerCount(Session) < 1 ? 1 : ovr_GetTrackerCount(Session); // Make sure there's always at least one.
			for (unsigned int i = 0; i < trackerCount; ++i)
				TrackerDescArray.push_back(ovr_GetTrackerDesc(Session, i));

			{
				Layer[0] = new VRLayer(dev, Session);
			}

			LVMSG("OculusVR Init", "sucess");
			return true;
		}    

		void Update(ID3D11Device* dev, ID3D11Texture2D* bb)
		{
			if (bb == nullptr || dev == nullptr)
			{
				LVMSG("OculusVR::Update", "failed, dev(%x), buffer(%x).", dev, bb);
				return;
			}

			ID3D11DeviceContext* Context = nullptr;
			dev->GetImmediateContext(&Context);
			if (Context != nullptr)
			{
				Context->CopyResource(Layer[0]->pEyeRenderTexture[0]->GetBuffer(), bb);
				Layer[0]->Commit(0);
				ovrResult ret = DistortAndPresent(0);
				if (!OVR_SUCCESS(ret))
				{
					Release();
					Init(dev);
				}
			}
			else
			{
				LVMSG("OculusVR::Update", "failed to get context.", dev, bb);
			}
		}
		
		int Release()
		{
			initialized = false;

			for (int i = 0; i < ovrMaxLayerCount; ++i)
			{
				delete Layer[i];
				Layer[i] = nullptr;
			}

			if (Session)
			{
				ovr_Destroy(Session);
				Session = nullptr;
			}

			return(0);
		}

		//------------------------------------------------------------
		ovrResult DistortAndPresent(int numLayersToRender, D3D11_BOX * optionalBoxForMirrorWindow = 0, bool mirror = true)
		{
			ovrLayerHeader* layerHeaders[ovrMaxLayerCount];
			for (int i = 0; i < ovrMaxLayerCount; i++)
			{
				if (Layer[i])
					layerHeaders[i] = &Layer[i]->ovrLayer.Header;
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