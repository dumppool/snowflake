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

namespace LostVR {
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

		unsigned int RecommendWidth;
		unsigned int RecommendHeight;

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
		, RecommendWidth(1280)
		, RecommendHeight(720)
		{}
		
		~OpenVRRenderer()
		{
			if (Sys != nullptr)
			{
				Sys = nullptr;
				pVRShutdownFn();
			}
		}

		bool Init();
		void OnPresent(IDXGISwapChain* SwapChain);
		void SubmitTexture(unsigned int Eye, ID3D11Texture2D* Buf);
		void UpdatePose();
		void GetEyeViewProject(vr::Hmd_Eye Eye, float fNearZ, float fFarZ, vr::EGraphicsAPIConvention eProjType, LVMatrix& EyeView, LVMatrix& Proj) const;
	};
};