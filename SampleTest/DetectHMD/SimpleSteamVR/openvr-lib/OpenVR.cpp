#include "stdafx.h"
#include "openvr-lib/OpenVR.h"

using namespace LostVR;

bool OpenVRRenderer::Init()
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

	Sys->GetRecommendedRenderTargetSize(&RecommendWidth, &RecommendHeight);
	LVMSG("OpenVR::Init", "recommended width: %d, height: %d.", RecommendWidth, RecommendHeight);
	LVMSG("OpenVR::Init", "Succeeded to init VR runtime");
	return true;
}

void OpenVRRenderer::OnPresent(IDXGISwapChain* SwapChain)
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

	if (LostVR::TextureProjector::Get()->SetSourceRef(Buf))
	{
		LostVR::TextureProjector::Get()->OnPresent(nullptr);
	}

	ReleaseComObjectRef(Buf);

	vr::VRTextureBounds_t LeftBounds;
	LeftBounds.uMin = 0.0f;
	LeftBounds.uMax = 1.f;
	LeftBounds.vMin = 0.0f;
	LeftBounds.vMax = 1.0f;

	vr::Texture_t Texture;
	Texture.handle = LostVR::TextureProjector::Get()->GetFinalBuffer(0);
	Texture.eType = vr::API_DirectX;
	Texture.eColorSpace = vr::ColorSpace_Auto;
	vr::EVRCompositorError Error = pCompositor->Submit(vr::Eye_Left, &Texture, 0, vr::Submit_Default);

	if (vr::VRCompositorError_None != Error)
		LVMSG("OpenVR::OnPresent", "Submit left  with result: %d", Error);

	vr::VRTextureBounds_t RightBounds;
	RightBounds.uMin = 0.f;
	RightBounds.uMax = 1.0f;
	RightBounds.vMin = 0.0f;
	RightBounds.vMax = 1.0f;

	Texture.handle = LostVR::TextureProjector::Get()->GetFinalBuffer(1);
	Error = pCompositor->Submit(vr::Eye_Right, &Texture, 0, vr::Submit_Default);

	if (vr::VRCompositorError_None != Error)
		LVMSG("OpenVR::OnPresent", "Submit right with result: %d", Error);

	//LVMSG("OpenVR::OnPresent", "tracking space: %d.", pCompositor->GetTrackingSpace());

#else /*if OPENVR_USEOVERLAY*/
	vr::IVROverlay* pOverlay = pVROverlayFn();
	if (pOverlay == nullptr)
	{
		LostVR::ReleaseComObjectRef(Buf);
		LVMSG("OpenVR::OnPresent", "null overlay");
		return;
	}

	if (!pOverlay->IsOverlayVisible(OverlayHandle) && !pOverlay->IsOverlayVisible(OverlayThumbnailHandle))
	{
		LostVR::ReleaseComObjectRef(Buf);
		return;
	}

	vr::Texture_t texture = { (void*)Buf, vr::API_DirectX, vr::ColorSpace_Auto };
	pOverlay->SetOverlayTexture(OverlayHandle, &texture);
#endif

	LostVR::ReleaseComObjectRef(Buf);
}

void OpenVRRenderer::SubmitTexture(unsigned int Eye, ID3D11Texture2D* Buf)
{
	vr::IVRCompositor* pCompositor = pVRCompositorFn();
	if (pCompositor == nullptr)
	{
		LVMSG("OpenVRRenderer::SubmitTexture", "null compositor");
		return;
	}

	vr::Texture_t Texture;
	Texture.handle = Buf;
	Texture.eType = vr::API_DirectX;
	Texture.eColorSpace = vr::ColorSpace_Auto;
	vr::EVRCompositorError Error = pCompositor->Submit(Eye==0?vr::EVREye::Eye_Left:vr::EVREye::Eye_Right, &Texture);
}

void OpenVRRenderer::GetEyeViewProject(vr::Hmd_Eye Eye, float fNearZ, float fFarZ, vr::EGraphicsAPIConvention eProjType, LVMatrix& EyeView, LVMatrix& Proj) const
{
	bool bCustom = false;

	if (bCustom)
	{
		float L, R, B, T;
		Sys->GetProjectionRaw(Eye, &L, &R, &T, &B);
		L *= 1.f;
		R *= 1.f;
		B *= -1.f;
		T *= -1.f;

		Proj.r[0].m128_f32[0] = 2.f / (R - L);
		Proj.r[0].m128_f32[1] = 0.f;
		Proj.r[0].m128_f32[2] = 0.f;
		Proj.r[0].m128_f32[3] = 0.f;

		Proj.r[1].m128_f32[0] = 0.f;
		Proj.r[1].m128_f32[1] = 2.f / (T - B);
		Proj.r[1].m128_f32[2] = 0.f;
		Proj.r[1].m128_f32[3] = 0.f;

		Proj.r[2].m128_f32[0] = (R + L) / (R - L);
		Proj.r[2].m128_f32[1] = (T + B) / (T - B);
		Proj.r[2].m128_f32[2] = 0.f;
		Proj.r[2].m128_f32[3] = 1.f;

		Proj.r[3].m128_f32[0] = 0.f;
		Proj.r[3].m128_f32[1] = 0.f;
		Proj.r[3].m128_f32[2] = 0.1f;
		Proj.r[3].m128_f32[3] = 0.f;
	}
	else
	{
		vr::HmdMatrix44_t HmdProj = Sys->GetProjectionMatrix(Eye, fNearZ, fFarZ, eProjType);
		memcpy(&Proj, &HmdProj, sizeof(HmdProj));
		Proj = DirectX::XMMatrixTranspose(Proj);
	}

	vr::HmdMatrix34_t HmdView = Sys->GetEyeToHeadTransform(Eye);

	if (bCustom)
	{
		EyeView = DirectX::XMMatrixTranslation(HmdView.m[0][3], HmdView.m[1][3], HmdView.m[2][3]);
		EyeView = DirectX::XMMatrixInverse(nullptr, EyeView);
	}
	else
	{
		memcpy(&EyeView, &HmdView, sizeof(HmdView));
		EyeView.r[3].m128_f32[0] = 0.f;
		EyeView.r[3].m128_f32[1] = 0.f;
		EyeView.r[3].m128_f32[2] = 0.f;
		EyeView.r[3].m128_f32[3] = 1.f;
		EyeView = DirectX::XMMatrixTranspose(EyeView);
		EyeView = DirectX::XMMatrixInverse(nullptr, EyeView);
	}
}

void OpenVRRenderer::UpdatePose()
{
	vr::IVRCompositor* pCompositor = pVRCompositorFn();
	if (pCompositor == nullptr)
	{
		LVMSG("OpenVR::UpdatePose", "null compositor:");
		return;
	}

	pCompositor->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
}
