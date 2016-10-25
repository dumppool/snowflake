#pragma once

#include "oculus-lib/Include/OVR_CAPI.h"
#include "oculus-lib/Include/OVR_CAPI_D3D.h"
#include "oculus-lib/Include/OVR_Version.h"
#include "oculus-lib/Include/OVR_CAPI_Keys.h"
#include "oculus-lib/Include/OVR_ErrorCode.h"

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
				//D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
				//rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				//rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
				//ID3D11RenderTargetView* rtv;
				//dev->CreateRenderTargetView(tex, &rtvd, &rtv);
				//TexRtv.push_back(rtv);
				//tex->Release();
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
				ovrFovPort fp = ovr_GetHmdDesc(Session).DefaultEyeFov[eye];
				ovrSizei idealSize = ovr_GetFovTextureSize(Session, (ovrEyeType)eye, fp, pixelsPerDisplayPixel);
				pEyeRenderTexture[eye] = new OculusTexture();
				LVMSG("MakeEyeBuffers*****************", "w(%d), h(%d), fov(%f, %f, %f, %f), Session(%x), pixelsPerdisplayPixel(%f)", 
					idealSize.w, idealSize.h, fp.DownTan, fp.LeftTan, fp.RightTan,fp.UpTan, Session, pixelsPerDisplayPixel);
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
			if (!OVR_SUCCESS(result))
			{
				LVMSG("OculusVR Init", "ovr_Initialize failed");
			}
			else
			{
				LVMSG("OculusVR Init", "ovr_Initialize succeeded");
			}

			ovrGraphicsLuid luid;
			result = ovr_Create(&Session, &luid);
			if (!OVR_SUCCESS(result))
			{
				LVMSG("OculusVR Init", "ovr_Create failed");
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

		void GetSwapChainData(DXGI_FORMAT& outFormat, int& sizeW, int& sizeH)
		{
			ovrSizei idealSize = ovr_GetFovTextureSize(Session, (ovrEyeType)0, ovr_GetHmdDesc(Session).DefaultEyeFov[0], 1.f);
			outFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			sizeW = idealSize.w;
			sizeH = idealSize.h;
			LVMSG("OculusVR::GetSwapChainData ***************************", "session(%x), format(%d), w(%d), h(%d)", Session, outFormat, sizeW, sizeH);
		}

		bool Init(ID3D11Device* dev)
		{
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
			LVMSG("OculusVR::Update", "dev(%x), buffer(%x).", dev, bb);
			if (bb == nullptr || dev == nullptr)
			{
				LVMSG("OculusVR::Update", "failed, dev(%x), buffer(%x).", dev, bb);
				return;
			}

			ID3D11DeviceContext* Context = nullptr;
			dev->GetImmediateContext(&Context);
			if (Context != nullptr)
			{
				Layer[0]->GetEyePoses();
				ID3D11Texture2D* dst = Layer[0]->pEyeRenderTexture[0]->GetBuffer();
				LVMSG("OculusVR::Update", "ready to copy from src(%x) to dst(%x)", bb, dst);
				Context->CopyResource(dst, bb);
				Layer[0]->Commit(0);
				ovrResult ret = DistortAndPresent(1);
				if (!OVR_SUCCESS(ret))
				{
					LVMSG("OculusVR::Update", "failed, reinitializing...");
					Release();
					if (Init(dev))
					{
						LVMSG("OculusVR::Update", "reinitialize sucess");
					}
					else
					{
						LVMSG("OculusVR::Update", "reinitialize fail");
					}
				}
			}
			else
			{
				LVMSG("OculusVR::Update", "failed to get context.", dev, bb);
			}

			Context->Release();
		}
		
		int Release()
		{
			initialized = false;

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