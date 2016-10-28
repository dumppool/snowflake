#pragma once

#include "openvr-lib/headers/openvr.h"
#include "renderers/Projector.h"
#include "renderers/IRenderer.h"

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
	class OpenVR : public IVRDevice
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

		TextureProjector* Projector;

	public:

		OpenVR();
		virtual ~OpenVR();

		virtual bool Startup() override;
		virtual void Shutdown() override;
		virtual bool IsConnected() override;

		virtual bool OnPresent_Direct3D11(IDXGISwapChain* swapChain) override;
		virtual bool OnPresent_Direct3D9(IDirect3DDevice9* device) override;

		virtual const std::string GetDeviceString() const;
		virtual std::string GetKeyString() const { return "[openvr]"; }

		void GetEyeViewProject(vr::Hmd_Eye Eye, float fNearZ, float fFarZ, vr::EGraphicsAPIConvention eProjType, LVMatrix& EyeView, LVMatrix& Proj) const;
	};
};