/*
 * file OpenVR.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"
#include "OpenVR.h"
#include "renderers/ConcreteProjectors.h"

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

OpenVR::OpenVR(const std::string& key) : IVRDevice(key)
, Sys(nullptr)
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
, PoseOrientation(nullptr)
, bIsCounting(false)
, Count(0)
, CountMax(100)
, Counter()
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

	double elapsed = 0.0;
	if (!Counter.Started())
	{
		Counter.Start();
	}
	else
	{
		elapsed = Counter.Count();
	}

	//if (bIsCounting)
	//{
	//	Count += elapsed;
	//	if (Count >= CountMax)
	//	{
	//		FakeMouseRelease(false);
	//		bIsCounting = false;
	//	}
	//}

	if (Projector == nullptr)
	{
		Projector = new TextureProjector0;
	}

#if !OPENVR_USEOVERLAY
	vr::IVRCompositor* pCompositor = pVRCompositorFn();
	if (pCompositor == nullptr)
	{
		LVMSG(head, "null compositor:");
		return false;
	}

	if (!Projector->IsInitialized(swapChain))
	{
		LVMatrix leftV, leftP, rightV, rightP;
		GetEyeViewProject(vr::Hmd_Eye::Eye_Left, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, leftV, leftP);
		GetEyeViewProject(vr::Hmd_Eye::Eye_Right, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, rightV, rightP);
		uint32 w, h;
		Sys->GetRecommendedRenderTargetSize(&w, &h);
		if (!Projector->InitializeProjector(swapChain, w, h, leftV, leftP, rightV, rightP))
		{
			LVMSG(head, "initialize projector failed");
			return false;
		}
	}

	pCompositor->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	if (SGlobalSharedDataInst.GetBFakeMouseMove())
	{
		ProcessPose();
	}

	if (!Projector->UpdateTexture())
	{
		LVMSG(head, "projector update failed");
		return false;
	}

	vr::Texture_t Texture;
	Texture.handle = Projector->GetFinalBuffer(0);
	Texture.eType = vr::API_DirectX;
	Texture.eColorSpace = vr::ColorSpace_Auto;
	vr::EVRCompositorError lError = pVRCompositorFn()->Submit(vr::Eye_Left, &Texture, 0, vr::Submit_Default);

	D3D11_TEXTURE2D_DESC LDesc;
	(Projector->GetFinalBuffer(0))->GetDesc(&LDesc);
	if (vr::VRCompositorError_None != lError)
	{
		LVMSG("OpenVR::OnPresent", "Submit left  with result: %d, texture: 0x%x, width(%d), height(%d), format(%d)", lError,
			Texture.handle, LDesc.Width, LDesc.Height, LDesc.Format);
	}

	Texture.handle = Projector->GetFinalBuffer(1);
	vr::EVRCompositorError rError = pVRCompositorFn()->Submit(vr::Eye_Right, &Texture, 0, vr::Submit_Default);

	D3D11_TEXTURE2D_DESC RDesc;
	(Projector->GetFinalBuffer(1))->GetDesc(&RDesc);
	if (vr::VRCompositorError_None != rError)
	{
		LVMSG("OpenVR::OnPresent", "Submit right with result: %d, texture: 0x%x, width(%d), height(%d), format(%d)", rError,
			Texture.handle, RDesc.Width, RDesc.Height, RDesc.Format);
	}

	if ((int)lError == 106 || (int)rError == 106)
	{
		SAFE_DELETE(Projector);
		Projector = new TextureProjector1;
		LVMatrix leftV, leftP, rightV, rightP;
		GetEyeViewProject(vr::Hmd_Eye::Eye_Left, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, leftV, leftP);
		GetEyeViewProject(vr::Hmd_Eye::Eye_Right, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, rightV, rightP);
		uint32 w, h;
		Sys->GetRecommendedRenderTargetSize(&w, &h);
		if (!Projector->InitializeProjector(swapChain, w, h, leftV, leftP, rightV, rightP))
		{
			LVMSG(head, "after submit failed, initialize projector failed");
		}

		return false;
	}

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
	const CHAR* head = "OpenVR::OnPresent_Direct3D9";
	
	if (Projector9 == nullptr)
	{
		Projector9 = new TextureProjector9;
	}

	vr::IVRCompositor* pCompositor = pVRCompositorFn();
	if (pCompositor == nullptr)
	{
		LVMSG(head, "null compositor:");
		return false;
	}

	if (!Projector9->IsInitialized_Direct3D9(device))
	{
		LVMatrix leftV, leftP, rightV, rightP;
		GetEyeViewProject(vr::Hmd_Eye::Eye_Left, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, leftV, leftP);
		GetEyeViewProject(vr::Hmd_Eye::Eye_Right, 0.1f, 10000.f, vr::EGraphicsAPIConvention::API_DirectX, rightV, rightP);
		uint32 w, h;
		Sys->GetRecommendedRenderTargetSize(&w, &h);
		if (!Projector9->InitializeProjector_Direct3D9(device, w, h, leftV, leftP, rightV, rightP))
		{
			LVMSG(head, "initialize projector failed");
			return false;
		}
	}

	pCompositor->WaitGetPoses(TrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	if (!Projector9->UpdateTexture())
	{
		LVMSG(head, "projector update failed");
		return false;
	}

	vr::VRCompositorError vrErr;
	vr::Texture_t vrTex;
	vrTex.handle = Projector9->GetFinalBuffer(vr::Eye_Left);
	vrTex.eType = vr::API_DirectX;
	vrTex.eColorSpace = vr::ColorSpace_Auto;
	vrErr = pCompositor->Submit(vr::Eye_Left, &vrTex);
	if (vrErr != vr::VRCompositorError_None)
	{
		LVMSG(head, "submit failed: %d, eye: %d", vrErr, vr::Eye_Left);
	}

	vrTex.handle = Projector9->GetFinalBuffer(vr::Eye_Right);
	vrErr = pCompositor->Submit(vr::Eye_Right, &vrTex);
	if (vrErr != vr::VRCompositorError_None)
	{
		LVMSG(head, "submit failed: %d, eye: %d", vrErr, vr::Eye_Right);
	}

	return true;
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

void OpenVR::AddMovement(EMovement movement)
{
	float rate = 1.f;
	switch (movement)
	{
	case lostvr::EMovement::UnDefined:
		break;
	case lostvr::EMovement::CameraFront:
		if (Projector != nullptr)
		{
			Projector->Translation.z += -rate;
		}
		if (Projector9 != nullptr)
		{
			Projector9->Translation.z += -rate;
		}
		break;
	case lostvr::EMovement::CameraBack:
		if (Projector != nullptr)
		{
			Projector->Translation.z += rate;
		}
		if (Projector9 != nullptr)
		{
			Projector9->Translation.z += rate;
		}
		break;
	default:
		break;
	}
}

void OpenVR::ProcessPose()
{
	ScopedHighFrequencyCounter counter("ProcessPose");

	const float scale = 1000.0f;
	for (int32 i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
	{
		if (TrackedDevicePose[i].bPoseIsValid &&
			TrackedDevicePose[i].eTrackingResult == vr::ETrackingResult::TrackingResult_Running_OK &&
			Sys->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_HMD)
		{
			if (abs(TrackedDevicePose[i].vAngularVelocity.v[0]) > 0.01f ||
				abs(TrackedDevicePose[i].vAngularVelocity.v[1]) > 0.01f ||
				abs(TrackedDevicePose[i].vAngularVelocity.v[2]) > 0.01f)
			{
				vr::HmdMatrix34_t mat = TrackedDevicePose[0].mDeviceToAbsoluteTracking;
				LVMatrix pose = LVMatrix(
					mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f,
					mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f,
					mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f,
					mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f);

				//LVQuat dst(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionRotationMatrix(pose)));
				LVQuat dst;
				dst = { 0.0f, 0.0f, 1.0f, 1.0f };
				dst = DirectX::XMVector4Transform(dst, pose);

				if (PoseOrientation == nullptr)
				{
					PoseOrientation = new LVQuat(dst);
				}
				else
				{
					pose = DirectX::XMMatrixInverse(nullptr, pose);
					LVQuat vdst = DirectX::XMVector4Transform(*PoseOrientation, pose);
					LVQuat dDir = {
						dst.m128_f32[0] - PoseOrientation->m128_f32[0],
						dst.m128_f32[1] - PoseOrientation->m128_f32[1],
						dst.m128_f32[2] - PoseOrientation->m128_f32[2],
						dst.m128_f32[3] - PoseOrientation->m128_f32[3] };

					float pitch, yaw;
					//LVQuat dQuat = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(*PoseOrientation), dst);
					//dQuat = DirectX::XMQuaternionNormalize(dQuat);
					//const float test = dQuat.m128_f32[2] * dQuat.m128_f32[0] - dQuat.m128_f32[3] * dQuat.m128_f32[1];
					//const float yawy = -2.f*(dQuat.m128_f32[3] * dQuat.m128_f32[1] + dQuat.m128_f32[2] * dQuat.m128_f32[0]);
					//const float yawx = (1.f - 2.f*(dQuat.m128_f32[1] * dQuat.m128_f32[1] + dQuat.m128_f32[2] * dQuat.m128_f32[2]));
					//const float pitchy = -2.f*(dQuat.m128_f32[3] * dQuat.m128_f32[0] + dQuat.m128_f32[1] * dQuat.m128_f32[2]);
					//const float pitchx = (1.f - 2.f*(dQuat.m128_f32[1] * dQuat.m128_f32[1] + dQuat.m128_f32[0] * dQuat.m128_f32[0]));

					//const float SINGULARITY_THRESHOLD = 0.4999995f;
					//const float RAD_TO_DEG = (180.f) / 3.1415926535897932f;

					//yaw = asin(yawy);
					//pitch = atan2(pitchy, pitchx);	

					yaw = scale * vdst.m128_f32[0];
					pitch = scale * -vdst.m128_f32[1];

					*PoseOrientation = dst;

					if (SGlobalSharedDataInst.GetBHoldWhenMoving() && !bIsCounting)
					{
						FakeMousePress(false);
					}

					FakeMouseMove(yaw, pitch);

					Count = 0;
					bIsCounting = true;
				}
			}
			else
			{
			}

			break;
		}
	}
} 

static OpenVR* SInst = new OpenVR("openvr");