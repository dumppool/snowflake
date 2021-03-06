#pragma once

#include "openvr-lib/headers/openvr.h"
#include "projector/Projector.h"
//#pragma comment(lib, "openvr-lib/lib/win64/openvr_api.lib")
#define OPENVR_RUNTIME "C:\\Users\\Administrator\\Documents\\GitHub\\snowflake\\SampleTest\\DetectHMD\\HookCore3\\openvr-lib\\bin\\win64\\openvr_api.dll"

#ifndef OPENVR_USEOVERLAY
#define OPENVR_USEOVERLAY 0
#endif

#define LOAD_INTERFACE_OPENVR(FnType, FnVar, FnName, Module)\
{\
	FnVar = (FnType)::GetProcAddress(Module, FnName);\
	if (FnVar == nullptr)\
	{\
		LVMSG("OpenVR::Init", "Failed to get fn: %s.", FnName);\
		return false;\
	}\
}

namespace lostvr {
	class OpenVRRenderer
	{
		typedef vr::IVRSystem*(VR_CALLTYPE *pVRInit)(vr::HmdError* peError, vr::EVRApplicationType eApplicationType);
		typedef void(VR_CALLTYPE *pVRShutdown)();
		typedef bool(VR_CALLTYPE *pVRIsHmdPresent)();
		typedef const char*(VR_CALLTYPE *pVRGetStringForHmdError)(vr::HmdError error);
		typedef void*(VR_CALLTYPE *pVRGetGenericInterface)(const char* pchInterfaceVersion, vr::HmdError* peError);
		typedef vr::IVRExtendedDisplay *(VR_CALLTYPE *pVRExtendedDisplay)();
		typedef vr::IVRCompositor *(VR_CALLTYPE *pVRCompositor)();
		typedef const char *(VR_CALLTYPE *pVR_GetVRInitErrorAsEnglishDescription)(vr::EVRInitError error);
		typedef vr::IVROverlay *(VR_CALLTYPE *pVROverlay)();

		pVRInit									pVRInitFn;
		pVRShutdown								pVRShutdownFn;
		pVRIsHmdPresent							pVRIsHmdPresentFn;
		pVRGetStringForHmdError					pVRGetStringForHmdErrorFn;
		pVRGetGenericInterface					pVRGetGenericInterfaceFn;
		pVRExtendedDisplay						pVRExtendedDisplayFn;
		pVRCompositor							pVRCompositorFn;
		pVR_GetVRInitErrorAsEnglishDescription	pVR_GetVRInitErrorAsEnglishDescriptionFn;
		pVROverlay								pVROverlayFn;

		vr::IVRSystem *Sys;
		vr::VROverlayHandle_t OverlayHandle;
		vr::VROverlayHandle_t OverlayThumbnailHandle;
		vr::TrackedDevicePose_t TrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

	public:

		static OpenVRRenderer* Get()
		{
			static OpenVRRenderer Inst;
			return &Inst;
		}

		OpenVRRenderer() : Sys(nullptr)
		, pVRInitFn(nullptr)
		, pVRShutdownFn(nullptr)
		, pVRIsHmdPresentFn(nullptr)
		, pVRGetStringForHmdErrorFn(nullptr)
		, pVRGetGenericInterfaceFn(nullptr)
		, pVRExtendedDisplayFn(nullptr)
		, pVRCompositorFn(nullptr)
		, pVR_GetVRInitErrorAsEnglishDescriptionFn(nullptr)
		, pVROverlayFn(nullptr)
		, OverlayHandle(vr::k_ulOverlayHandleInvalid)
		, OverlayThumbnailHandle(vr::k_ulOverlayHandleInvalid)
		{}
		
		~OpenVRRenderer()
		{
			if (Sys != nullptr)
			{
				Sys = nullptr;
				pVRShutdownFn();
			}
		}

		bool Init()
		{
			HMODULE hModule = ::LoadLibrary(TEXT(OPENVR_RUNTIME));
			if (!hModule)
			{
				LVMSG("OpenVR::Init", "failed to load openvr_api.dll");
				return false;
			}

			LOAD_INTERFACE_OPENVR(pVRInit, pVRInitFn, "VR_Init", hModule);
			LOAD_INTERFACE_OPENVR(pVRShutdown, pVRShutdownFn, "VR_Shutdown", hModule);
			LOAD_INTERFACE_OPENVR(pVRIsHmdPresent, pVRIsHmdPresentFn, "VR_IsHmdPresent", hModule);
			LOAD_INTERFACE_OPENVR(pVRGetStringForHmdError, pVRGetStringForHmdErrorFn, "VR_GetStringForHmdError", hModule);
			LOAD_INTERFACE_OPENVR(pVRGetGenericInterface, pVRGetGenericInterfaceFn, "VR_GetGenericInterface", hModule);
			LOAD_INTERFACE_OPENVR(pVRExtendedDisplay, pVRExtendedDisplayFn, "VRExtendedDisplay", hModule);
			LOAD_INTERFACE_OPENVR(pVRCompositor, pVRCompositorFn, "VRCompositor", hModule);
			LOAD_INTERFACE_OPENVR(pVR_GetVRInitErrorAsEnglishDescription, pVR_GetVRInitErrorAsEnglishDescriptionFn, "VR_GetVRInitErrorAsEnglishDescription", hModule);
			LOAD_INTERFACE_OPENVR(pVROverlay, pVROverlayFn, "VROverlay", hModule);

			// Loading the SteamVR Runtime
			vr::EVRInitError eError = vr::VRInitError_None;
#if OPENVR_USEOVERLAY
			Sys = (pVRInitFn)(&eError, vr::VRApplication_Overlay);
			vr::VROverlayError overlayError = pVROverlayFn()->CreateDashboardOverlay("LostVR_Key", "LostVR_Overlay", &OverlayHandle, &OverlayThumbnailHandle);
			if (overlayError != vr::VROverlayError_None)
			{
				LVMSG("OpenVR::Init", "unable to create dashboard overlay: %d.", overlayError);
				return false;
			}

			vr::HmdVector2_t vecWindowSize =
			{
				1280.f, 720.f
			};

			pVROverlayFn()->SetOverlayWidthInMeters(OverlayHandle, 5.f);
			pVROverlayFn()->SetOverlayInputMethod(OverlayHandle, vr::VROverlayInputMethod_Mouse);
			pVROverlayFn()->SetOverlayMouseScale(OverlayHandle, &vecWindowSize);
			pVROverlayFn()->ShowOverlay(OverlayHandle);
#else
			Sys = (pVRInitFn)(&eError, vr::VRApplication_Scene);
#endif

			if (eError != vr::VRInitError_None)
			{
				Sys = nullptr;
				LVMSG("OpenVR::Init", "Unable to init VR runtime: %s", pVR_GetVRInitErrorAsEnglishDescriptionFn(eError));
				return false;
			}

			if (pVRCompositorFn() == nullptr)
			{
				Sys = nullptr;
				LVMSG("OpenVR::Init", "Compositor init failed.");
				return false;
			}

			LVMSG("OpenVR::Init", "Succeeded to init VR runtime");
			return true;
		}

		void OnPresent(IDXGISwapChain* SwapChain)
		{
			if (Sys == nullptr)
			{
				if (!Init())
				{
					LVMSG("OpenVR::OnPresent", "init failed.");
					return;
				}
			}

			ID3D11Texture2D* Buf = nullptr;
			if (FAILED(SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D*), (void**)&Buf)))
			{
				LVMSG("OpenVR::OnPresent", "failed to get buffer, SwapChain(%x).", SwapChain);
				return;
			}

#if !OPENVR_USEOVERLAY
			vr::IVRCompositor* pCompositor = pVRCompositorFn();
			//vr::EVRInitError VRInitErr = vr::VRInitError_None;
			//pCompositor = (vr::IVRCompositor*)(*pVRGetGenericInterfaceFn)(vr::IVRCompositor_Version, &VRInitErr);
			if (pCompositor == nullptr)
			{
				LVMSG("OpenVR::OnPresent", "null compositor:");
				return;
			}

			//pCompositor->SetTrackingSpace(vr::TrackingUniverseRawAndUncalibrated);
			pCompositor->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

			if (!lostvr::TextureProjector::Get()->OnPresent(Buf))
			{
				lostvr::TextureProjector::Get()->Fini();
				LVMatrix LeftV, LeftP, RightV, RightP;
				GetEyeViewProject(vr::Hmd_Eye::Eye_Left, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, LeftV, LeftP);
				GetEyeViewProject(vr::Hmd_Eye::Eye_Right, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, RightV, RightP);
				uint32 w, h;
				Sys->GetRecommendedRenderTargetSize(&w, &h);
				if (!lostvr::TextureProjector::Get()->Init(Buf, w, h, &LeftV, &LeftP, &RightV, &RightP))
				{
					lostvr::TextureProjector::Get()->Fini();
					LVMSG("OpenVR::OnPresent", "failed to init TextureProjector");
				}
				else
				{
					LVMSG("OpenVR::OnPresent", "inited TextureProjector successfully");
				}

				lostvr::ReleaseComObjectRef(Buf);
				return;
			}

			vr::VRTextureBounds_t LeftBounds;
			LeftBounds.uMin = 0.0f;
			LeftBounds.uMax = 1.f;
			LeftBounds.vMin = 0.0f;
			LeftBounds.vMax = 1.0f;

			vr::Texture_t Texture;
			Texture.handle = lostvr::TextureProjector::Get()->GetFinalBuffer(0);
			Texture.eType = vr::API_DirectX;
			Texture.eColorSpace = vr::ColorSpace_Auto;
			vr::EVRCompositorError Error = pVRCompositorFn()->Submit(vr::Eye_Left, &Texture, 0, vr::Submit_Default);

			D3D11_TEXTURE2D_DESC LDesc;
			(lostvr::TextureProjector::Get()->GetFinalBuffer(0))->GetDesc(&LDesc);
			if (vr::VRCompositorError_None != Error)
				LVMSG("OpenVR::OnPresent", "Submit left  with result: %d, width(%d), height(%d), format(%d)", Error, LDesc.Width, LDesc.Height, LDesc.Format);

			vr::VRTextureBounds_t RightBounds;
			RightBounds.uMin = 0.f;
			RightBounds.uMax = 1.0f;
			RightBounds.vMin = 0.0f;
			RightBounds.vMax = 1.0f;

			Texture.handle = lostvr::TextureProjector::Get()->GetFinalBuffer(1);
			Error = pVRCompositorFn()->Submit(vr::Eye_Right, &Texture, 0, vr::Submit_Default);

			D3D11_TEXTURE2D_DESC RDesc;
			(lostvr::TextureProjector::Get()->GetFinalBuffer(1))->GetDesc(&RDesc);
			if (vr::VRCompositorError_None != Error)
				LVMSG("OpenVR::OnPresent", "Submit right with result: %d, width(%d), height(%d), format(%d)", Error, RDesc.Width, RDesc.Height, RDesc.Format);

			//LVMSG("OpenVR::OnPresent", "tracking space: %d.", pCompositor->GetTrackingSpace());

#else /*if OPENVR_USEOVERLAY*/
			vr::IVROverlay* pOverlay = pVROverlayFn();
			if (pOverlay == nullptr)
			{
				lostvr::ReleaseComObjectRef(Buf);
				LVMSG("OpenVR::OnPresent", "null overlay");
				return;
			}

			if (!pOverlay->IsOverlayVisible(OverlayHandle) && !pOverlay->IsOverlayVisible(OverlayThumbnailHandle))
			{
				lostvr::ReleaseComObjectRef(Buf);
				return;
			}

			vr::Texture_t texture = { (void*)Buf, vr::API_DirectX, vr::ColorSpace_Auto };
			pOverlay->SetOverlayTexture(OverlayHandle, &texture);
#endif

			lostvr::ReleaseComObjectRef(Buf);
		}

		void GetEyeViewProject(vr::Hmd_Eye Eye, float fNearZ, float fFarZ, vr::EGraphicsAPIConvention eProjType, LVMatrix& EyeView, LVMatrix& Proj) const;
	};
};