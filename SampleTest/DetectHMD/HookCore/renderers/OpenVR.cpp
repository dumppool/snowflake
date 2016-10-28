#include "HookCorePCH.h"
#include "OpenVR.h"

#define GETPROPERTYSTRING_OPENVR(propVar, indent, propEnum)\
char _##propVar[128] = {0};\
vr::TrackedPropertyError propVar##err;\
Sys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, propEnum, _##propVar, sizeof(_##propVar), &propVar##err);\
char propVar[256] = {0};\
snprintf(&propVar[0], sizeof(propVar), "%s:%s%s\n", #propVar, indent, _##propVar);

using namespace lostvr;

void OpenVR::GetEyeViewProject(vr::Hmd_Eye Eye, float fNearZ, float fFarZ, vr::EGraphicsAPIConvention eProjType, LVMatrix& EyeView, LVMatrix& Proj) const
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

OpenVR::OpenVR() : Sys(nullptr)
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
, Projector(nullptr)
{

}

OpenVR::~OpenVR()
{
	Shutdown();
}

bool OpenVR::Startup()
{
	Shutdown();

	const CHAR* head = "OpenVR::Startup";
	HMODULE hModule = ::LoadLibrary(SGlobalSharedDataInst.GetOpenVRDllPath());
	if (!hModule)
	{
		LVMSG(head, "failed to load %ls", SGlobalSharedDataInst.GetOpenVRDllName());
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
		LVMSG(head, "unable to create dashboard overlay: %d.", overlayError);
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
		LVMSG(head, "Unable to init VR runtime: %s", pVR_GetVRInitErrorAsEnglishDescriptionFn(eError));
		return false;
	}

	if (pVRCompositorFn() == nullptr)
	{
		Sys = nullptr;
		LVMSG(head, "Compositor init failed: %s", pVR_GetVRInitErrorAsEnglishDescriptionFn(eError));
		return false;
	}

	LVMSG(head, "Succeeded to init VR runtime");
	return true;

}

void OpenVR::Shutdown()
{
	if (Sys != nullptr)
	{
		pVRInitFn = nullptr;
		pVRShutdownFn = nullptr;
		pVRIsHmdPresentFn = nullptr;
		pVRGetStringForHmdErrorFn = nullptr;
		pVRGetGenericInterfaceFn = nullptr;
		pVRExtendedDisplayFn = nullptr;
		pVRCompositorFn = nullptr;
		pVR_GetVRInitErrorAsEnglishDescriptionFn = nullptr;
		pVROverlayFn = nullptr;

		Sys = nullptr;
		pVRShutdownFn();

		SAFE_DELETE(Projector);
	}
}

bool OpenVR::IsConnected()
{
	return Sys != nullptr;
}

bool OpenVR::OnPresent_Direct3D11(IDXGISwapChain* swapChain)
{
	const CHAR* head = "OpenVR::OnPresent_Direct3D11";

	if (Projector == nullptr)
	{
		Projector = new TextureProjector;
	}

#if !OPENVR_USEOVERLAY
	vr::IVRCompositor* pCompositor = pVRCompositorFn();
	if (pCompositor == nullptr)
	{
		LVMSG("OpenVR::OnPresent", "null compositor:");
		return false;
	}

	if (!Projector->IsInitialized(swapChain))
	{
		LVMatrix leftV, leftP, rightV, rightP;
		GetEyeViewProject(vr::Hmd_Eye::Eye_Left, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, leftV, leftP);
		GetEyeViewProject(vr::Hmd_Eye::Eye_Right, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, rightV, rightP);
		uint32 w, h;
		Sys->GetRecommendedRenderTargetSize(&w, &h);
		if (!Projector->InitializeProjector(swapChain, w, h, &leftV, &leftP, &rightV, &rightP))
		{
			LVMSG(head, "initialize projectpr failed");
			return false;
		}
	}

	pCompositor->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	if (!Projector->UpdateTexture())
	{
		LVMSG(head, "projector update failed");
		return false;
	}

	vr::VRTextureBounds_t LeftBounds;
	LeftBounds.uMin = 0.0f;
	LeftBounds.uMax = 1.f;
	LeftBounds.vMin = 0.0f;
	LeftBounds.vMax = 1.0f;

	vr::Texture_t Texture;
	Texture.handle = Projector->GetFinalBuffer(0);
	Texture.eType = vr::API_DirectX;
	Texture.eColorSpace = vr::ColorSpace_Auto;
	vr::EVRCompositorError Error = pVRCompositorFn()->Submit(vr::Eye_Left, &Texture, 0, vr::Submit_Default);

	D3D11_TEXTURE2D_DESC LDesc;
	(Projector->GetFinalBuffer(0))->GetDesc(&LDesc);
	if (vr::VRCompositorError_None != Error)
		LVMSG("OpenVR::OnPresent", "Submit left  with result: %d, texture: 0x%x, width(%d), height(%d), format(%d)", Error,
			Texture.handle, LDesc.Width, LDesc.Height, LDesc.Format);

	vr::VRTextureBounds_t RightBounds;
	RightBounds.uMin = 0.f;
	RightBounds.uMax = 1.0f;
	RightBounds.vMin = 0.0f;
	RightBounds.vMax = 1.0f;

	Texture.handle = Projector->GetFinalBuffer(1);
	Error = pVRCompositorFn()->Submit(vr::Eye_Right, &Texture, 0, vr::Submit_Default);

	D3D11_TEXTURE2D_DESC RDesc;
	(Projector->GetFinalBuffer(1))->GetDesc(&RDesc);
	if (vr::VRCompositorError_None != Error)
		LVMSG("OpenVR::OnPresent", "Submit right with result: %d, texture: 0x%x, width(%d), height(%d), format(%d)", Error,
			Texture.handle, LDesc.Width, LDesc.Height, LDesc.Format);

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

	return true;
}

bool OpenVR::OnPresent_Direct3D9(IDirect3DDevice9 * device)
{
	return false;
}

const std::string OpenVR::GetDeviceString() const
{
	CHAR strDesc[1024];
	if (Sys == nullptr)
	{
		return "[unknown]";
	}

	GETPROPERTYSTRING_OPENVR(TrackingSystemName, "\t\t", vr::Prop_TrackingSystemName_String);
	GETPROPERTYSTRING_OPENVR(ModelNumber, "\t\t\t", vr::Prop_ModelNumber_String);
	GETPROPERTYSTRING_OPENVR(SerialNumber, "\t\t\t", vr::Prop_SerialNumber_String);
	GETPROPERTYSTRING_OPENVR(RenderModelNumber, "\t\t", vr::Prop_RenderModelName_String);
	GETPROPERTYSTRING_OPENVR(ManufacturerName, "\t\t", vr::Prop_ManufacturerName_String);
	GETPROPERTYSTRING_OPENVR(TrackingFirmwareVersion, "\t", vr::Prop_TrackingFirmwareVersion_String);
	GETPROPERTYSTRING_OPENVR(HardwareRevision, "\t\t", vr::Prop_HardwareRevision_String);
	GETPROPERTYSTRING_OPENVR(AllWirelessDongleDescriptions, "\t", vr::Prop_AllWirelessDongleDescriptions_String);
	GETPROPERTYSTRING_OPENVR(ConnectedWirelessDongle, "\t", vr::Prop_ConnectedWirelessDongle_String);
	GETPROPERTYSTRING_OPENVR(FirmwareManualUpdateURL, "\t", vr::Prop_Firmware_ManualUpdateURL_String);
	GETPROPERTYSTRING_OPENVR(AttachedDeviceId, "\t\t", vr::Prop_AttachedDeviceId_String);

	snprintf(strDesc, sizeof(strDesc), "\n%s%s%s%s%s%s%s%s%s%s%s", TrackingSystemName, ModelNumber, SerialNumber, RenderModelNumber, ManufacturerName,
		TrackingFirmwareVersion, HardwareRevision, AllWirelessDongleDescriptions, ConnectedWirelessDongle, FirmwareManualUpdateURL, AttachedDeviceId);

	return std::string(strDesc);
}

static OpenVR* SInst = new OpenVR;