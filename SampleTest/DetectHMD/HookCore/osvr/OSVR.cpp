/*
 * file FOSVR.cpp
 *
 * author luoxw
 * date 2017/01/09
 *
 * 
 */
#include "HookCorePCH.h"
#include "OSVR.h"
#include "renderers/ConcreteProjectors.h"

#include <osvr/ClientKit/ServerAutoStartC.h>

#pragma comment(lib, "osvr-lib/lib/win64/osvrClientKit.lib")
#pragma comment(lib, "osvr-lib/lib/win64/osvrRenderManager.lib")

#define GETPROPERTYSTRING_OPENVR(propVar, indent, propEnum)\
char _##propVar[128] = {0};\
vr::TrackedPropertyError propVar##err;\
Sys->GetStringTrackedDeviceProperty(vr::k_unTrackedDeviceIndex_Hmd, propEnum, _##propVar, sizeof(_##propVar), &propVar##err);\
char propVar[256] = {0};\
snprintf(&propVar[0], sizeof(propVar), "%s:%s%s\n", #propVar, indent, _##propVar);

using namespace lostvr;

FOSVR::FOSVR(const std::string& key) : IVRDevice(key)
, ClientCxt(nullptr)
, DisplayConf(nullptr)
, RenderManagerD3D11(nullptr)
, RenderManager(nullptr)
, Projector(nullptr)
, PoseOrientation(nullptr)
, bIsCounting(false)
{
	Modules.clear();
	RenderBuffers.clear();
	RenderInfos.clear();
	RenderViewports.clear();
}

FOSVR::~FOSVR()
{
	Shutdown();
}

bool FOSVR::Startup()
{
	const CHAR* head = "FOSVR::Startup";

	Shutdown();

	LoadLibraryA("dep_x64/d3dcompiler_47.dll");
	LoadLibraryA("dep_x64/glew32.dll");
	LoadLibraryA("dep_x64/osvrClient.dll");
	LoadLibraryA("dep_x64/osvrClientKit.dll");
	LoadLibraryA("dep_x64/osvrCommon.dll");
	LoadLibraryA("dep_x64/osvrRenderManager.dll");
	LoadLibraryA("dep_x64/osvrUtil.dll");
	LoadLibraryA("dep_x64/SDL2.dll");
	// load dlls
	//Modules.push_back()

	osvrClientAttemptServerAutoStart();

	ClientCxt = osvrClientInit("lostvr.osvr");

	{
		bool bClientContextOK = false;
		int cd = 10;
		while (cd-- > 0 && !bClientContextOK)
		{
			bClientContextOK = osvrClientCheckStatus(ClientCxt) == OSVR_RETURN_SUCCESS;
			if (!bClientContextOK)
			{
				if (osvrClientUpdate(ClientCxt) == OSVR_RETURN_FAILURE)
				{
					break;
				}

				Sleep(200);
			}
		}

		if (!bClientContextOK)
		{
			LVERROR(head, "OSVR client context could not connect. Most likely the server isn't running. Treating this as if the HMD is not connected.");
			return false;
		}
	}

	bool bDisplayConfigOK = false;
	{
		bool bFailure = false;

		auto rc = osvrClientGetDisplay(ClientCxt, &DisplayConf);
		if (rc == OSVR_RETURN_FAILURE)
		{
			LVERROR(head, "Could not create DisplayConfig. Treating this as if the HMD is not connected.");
		}
		else
		{
			int cd = 10;
			while (!bDisplayConfigOK && cd-- > 0)
			{
				bDisplayConfigOK = osvrClientCheckDisplayStartup(DisplayConf) == OSVR_RETURN_SUCCESS;
				if (!bDisplayConfigOK)
				{
					if (osvrClientUpdate(ClientCxt) == OSVR_RETURN_FAILURE)
					{
						break;
					}
				}

				Sleep(200);
			}

			if (!bDisplayConfigOK)
			{
				LVERROR(head, "DisplayConfig failed to startup. This could mean that there is nothing mapped to /me/head. Treating this as if the HMD is not connected.");
				return false;
			}
		}
	}

	{
		bool bSuccess = Description.Init(ClientCxt, DisplayConf);
		if (!bSuccess)
		{
			LVERROR(head, "The OSVR display config does not match the expectations. Possibly incompatible HMD configuration.");
			return false;
		}
	}

	LVMSG(head, "Succeeded to init osvr runtime");
	return true;

}

void FOSVR::Shutdown()
{
	if (ClientCxt)
	{
		osvrClientShutdown(ClientCxt);
	}

	osvrClientReleaseAutoStartedServer();
	ClientCxt = nullptr;
}

bool FOSVR::IsConnected()
{
	return ClientCxt && osvrClientCheckStatus(ClientCxt) == OSVR_RETURN_SUCCESS;
}

bool FOSVR::OnPresent_Direct3D11(IDXGISwapChain* swapChain)
{
	const CHAR* head = "FOSVR::OnPresent_Direct3D11";
	OSVR_ReturnCode rc;

	if (Projector == nullptr)
	{
		Projector = new TextureProjector1;
	}

	if (!Projector->IsInitialized(swapChain))
	{
		//ID3D11Device* device = nullptr;
		//ID3D11DeviceContext* context = nullptr;
		//swapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
		//device->GetImmediateContext(&context);
		//if (UpdateRenderInfos(nullptr, nullptr))
		{
			LVMatrix leftV = Description.GetViewMatrix(SEnumEyeL);
			LVMatrix leftP = Description.GetProjectionMatrix(SEnumEyeL, DisplayConf, 0.1f, 1000.f);
			LVMatrix rightV = Description.GetViewMatrix(SEnumEyeR);
			LVMatrix rightP = Description.GetProjectionMatrix(SEnumEyeR, DisplayConf, 0.1f, 1000.f);
			if (!Projector->InitializeProjector(swapChain, 1440, 1620, leftV, leftP, rightV, rightP))
			{
				LVMSG(head, "initialize projector failed");
				return false;
			}

			UpdateRenderInfos(Projector->GetRenderer()->GetDevice(), Projector->GetRenderer()->GetContext());
			UpdateRenderBuffers();
			UpdateRenderViewports();
		}
	}

	if (OSVR_RETURN_SUCCESS != osvrClientUpdate(ClientCxt))
	{
		LVERROR(head, "client update failed");
		return false;
	}

	//OSVR_Pose3 pose;
	//rc = osvrClientGetViewerPose(DisplayConf, 0, &pose);

	if (!Projector->UpdateTexture())
	{
		LVMSG(head, "projector update failed");
		return false;
	}

	// all of the render manager samples keep the flipY at the default false,
	// for both OpenGL and DirectX. Is this even needed anymore?
	OSVR_RenderManagerPresentState presentState;
	rc = osvrRenderManagerStartPresentRenderBuffers(&presentState);
	if (rc != OSVR_RETURN_SUCCESS)
	{
		LVERROR(head, "osvrRenderManagerStartPresentRenderBuffers failed");
		return false;
	}

	if (RenderBuffers.size() != RenderInfos.size() || RenderBuffers.size() != RenderViewports.size())
	{
		LVERROR(head, "render buffer size not match");
		return false;
	}

	for (int32 i = 0; i < RenderBuffers.size(); i++)
	{
		rc = osvrRenderManagerPresentRenderBufferD3D11(presentState, RenderBuffers[i], RenderInfos[i], RenderViewports[i]);
		if (rc != OSVR_RETURN_SUCCESS)
		{
			LVERROR(head, "osvrRenderManagerPresentRenderBufferD3D11 failed");
			return false;
		}
	}

	rc = osvrRenderManagerFinishPresentRenderBuffers(RenderManager, presentState, RenderParams, true ? OSVR_TRUE : OSVR_FALSE);
	if (rc != OSVR_RETURN_SUCCESS)
	{
		LVERROR(head, "osvrRenderManagerFinishPresentRenderBuffers failed");
		return false;
	}

	return true;
}

bool FOSVR::OnPresent_Direct3D9(IDirect3DDevice9 * device)
{
	const CHAR* head = "FOSVR::OnPresent_Direct3D9";
	
	return true;
}

const std::string FOSVR::GetDeviceString() const
{
	CHAR strDesc[1024];
	//if (Sys == nullptr)
	{
		return "[unknown]";
	}

	return std::string(strDesc);
}

void FOSVR::AddMovement(EMovement movement)
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

void FOSVR::ProcessPose()
{
	ScopedHighFrequencyCounter counter("ProcessPose");

}

bool FOSVR::UpdateRenderInfos(ID3D11Device* device, ID3D11DeviceContext* context)
{
	const CHAR* head = "FOSVR::UpdateRenderInfos";

	//if (Projector == nullptr && Projector9 == nullptr)
	//{
	//	LVERROR(head, "both projector and projector9 are invalid");
	//	return false;
	//}

	//Direct3D11Helper* renderer = Projector != nullptr ? Projector->GetRenderer() : Projector9->GetRenderer();
	//if (renderer == nullptr)
	//{
	//	LVERROR(head, "null renderer");
	//	return false;
	//}

	//RenderParams;

	OSVR_GraphicsLibraryD3D11 glib;
	glib.device = device;
	glib.context = context;

	if (device != nullptr)
	{
		device->AddRef();
	}

	if (context != nullptr)
	{
		context->AddRef();
	}

	OSVR_ReturnCode rc;

	if (!ClientCxt)
	{
		LVERROR(head, "Can't initialize FOSVRCustomPresent without a valid client context");
		return false;
	}

	rc = osvrCreateRenderManagerD3D11(ClientCxt, "Direct3D11", glib, &RenderManager, &RenderManagerD3D11);
	if (rc == OSVR_RETURN_FAILURE || !RenderManager || !RenderManagerD3D11)
	{
		LVERROR(head, "osvrCreateRenderManagerD3D11 call failed, or returned numm renderManager/renderManagerD3D11 instances");
		return false;
	}

	rc = osvrRenderManagerGetDoingOkay(RenderManager);
	if (rc == OSVR_RETURN_FAILURE)
	{
		LVERROR(head, "osvrRenderManagerGetDoingOkay call failed. Perhaps there was an error during initialization?");
		return false;
	}

	OSVR_OpenResultsD3D11 results;
	rc = osvrRenderManagerOpenDisplayD3D11(RenderManagerD3D11, &results);
	if (rc == OSVR_RETURN_FAILURE || results.status == OSVR_OPEN_STATUS_FAILURE)
	{
		LVERROR(head, "osvrRenderManagerOpenDisplayD3D11 call failed, or the result status was OSVR_OPEN_STATUS_FAILURE."\
			"Potential causes could be that the display is already open in direct mode with another app, or the display "\
			"does not support direct mode");
		return false;
	}

	// Should we create a RenderParams?

	rc = osvrRenderManagerGetDefaultRenderParams(&RenderParams);
	if (rc != OSVR_RETURN_SUCCESS)
	{
		LVERROR(head, "osvrRenderManagerGetDefaultRenderParams failed");
		return false;
	}

	OSVR_RenderInfoCount numRenderInfo;
	rc = osvrRenderManagerGetNumRenderInfo(RenderManager, RenderParams, &numRenderInfo);
	if (rc != OSVR_RETURN_SUCCESS)
	{
		LVERROR(head, "osvrRenderManagerGetNumRenderInfo failed");
		return false;
	}

	RenderInfos.clear();
	for (int i = 0; i < numRenderInfo; i++)
	{
		OSVR_RenderInfoD3D11 renderInfo;
		rc = osvrRenderManagerGetRenderInfoD3D11(RenderManagerD3D11, i, RenderParams, &renderInfo);
		if (rc != OSVR_RETURN_SUCCESS)
		{
			LVERROR(head, "osvrRenderManagerGetRenderInfoD3D11 failed");
			return false;
		}

		RenderInfos.push_back(renderInfo);
	}

	// check some assumptions. Should all be the same height.
	if (RenderInfos.size() != 2)
	{
		LVERROR(head, "RenderInfos.size() is %d", RenderInfos.size());
		return false;
	}

	if (RenderInfos[0].viewport.height != RenderInfos[1].viewport.height)
	{
		LVERROR(head, "viewports' size not match(%f, %f)", (float)RenderInfos[0].viewport.height, (float)RenderInfos[1].viewport.height);
		return false;
	}

	RenderInfos[0].viewport.width = int(float(RenderInfos[0].viewport.width));
	RenderInfos[0].viewport.height = int(float(RenderInfos[0].viewport.height));
	RenderInfos[1].viewport.width = RenderInfos[0].viewport.width;
	RenderInfos[1].viewport.height = RenderInfos[0].viewport.height;
	//RenderInfos[1].viewport.left = RenderInfos[0].viewport.width;

	return true;
}

bool FOSVR::UpdateRenderBuffers()
{
	const CHAR* head = "FOSVR::UpdateRenderBuffers";

	//D3D11_TEXTURE2D_DESC texDesc;
	//renderer->GetDescriptionTemplate_DefaultTexture2D(texDesc);
	//texDesc.Width = RenderInfos[0].viewport.width;
	//texDesc.Height = RenderInfos[0].viewport.height;
	//texDesc.Usage = D3D11_USAGE_DEFAULT;
	//texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	//ID3D11Texture2D* tex = nullptr;
	//ID3D11ShaderResourceView* srv = nullptr;
	//ID3D11RenderTargetView* rtv = nullptr;
	//if (!renderer->CreateTexture2D(&tex, &srv, &texDesc) || !renderer->CreateRenderTargetView(tex, &rtv))
	//{
	//	LVERROR(head, "failed to create texture");
	//	return false;
	//}

	//ID3D11Texture2D* tex2 = nullptr;
	//ID3D11ShaderResourceView* srv2 = nullptr;
	//ID3D11RenderTargetView* rtv2 = nullptr;
	//if (!renderer->CreateTexture2D(&tex2, &srv2, &texDesc) || !renderer->CreateRenderTargetView(tex2, &rtv2))
	//{
	//	LVERROR(head, "failed to create texture2");
	//	return false;
	//}

	BaseTextureProjector* proj = Projector != nullptr ? Projector : Projector9;
	if (proj == nullptr)
	{
		LVERROR(head, "both Projector and Projector9 are invalid");
		return false;
	}

	RenderBuffers.clear();
	OSVR_RenderBufferD3D11 buffer;
	buffer.colorBuffer = proj->GetFinalBuffer(SEnumEyeL);
	buffer.colorBufferView = proj->GetFinalBuffer_RTV(SEnumEyeL);
	RenderBuffers.push_back(buffer);

	buffer.colorBuffer = proj->GetFinalBuffer(SEnumEyeR);
	buffer.colorBufferView = proj->GetFinalBuffer_RTV(SEnumEyeR);
	RenderBuffers.push_back(buffer);

	OSVR_RenderManagerRegisterBufferState state;
	HRESULT hr = osvrRenderManagerStartRegisterRenderBuffers(&state);
	if (hr != S_OK)
	{
		LVERROR(head, "osvrRenderManagerStartRegisterRenderBuffers failed");
		return false;
	}

	for (int i = 0; i < RenderBuffers.size(); ++i)
	{
		hr = osvrRenderManagerRegisterRenderBufferD3D11(state, RenderBuffers[i]);
		if (hr != S_OK)
		{
			LVERROR(head, "osvrRenderManagerRegisterRenderBufferD3D11 failed");
			return false;
		}
	}

	hr = osvrRenderManagerFinishRegisterRenderBuffers(RenderManager, state, false);
	if (hr != S_OK)
	{
		LVERROR(head, "osvrRenderManagerFinishRegisterRenderBuffers failed");
		return false;
	}

	return true;
}

bool FOSVR::UpdateRenderViewports()
{
	RenderViewports.clear();

	OSVR_ViewportDescription leftEye, rightEye;

	leftEye.left = 0;
	leftEye.lower = 0;
	leftEye.width = 1.0;
	leftEye.height = 1.0;
	RenderViewports.push_back(leftEye);

	rightEye.left = 0;
	rightEye.lower = 0;
	rightEye.width = 1.0;
	rightEye.height = 1.0;
	RenderViewports.push_back(rightEye);
	return true;
}

bool FOSVRDescription::Init(OSVR_ClientContext OSVRClientContext, OSVR_DisplayConfig displayConfig)
{
	const CHAR* head = "FOSVRDescription::Init";
	bValid = false;

	// if the OSVR viewer doesn't fit nicely with the Unreal HMD model, don't
	// bother trying to fill everything else out.
	if (!OSVRViewerFits(displayConfig))
	{
		LVERROR(head, "OSVRViewerFits failed");
		return false;
	}

	if (!InitIPD(displayConfig))
	{
		LVERROR(head, "InitIPD failed");
		return false;
	}
	if (!InitDisplaySize(displayConfig))
	{
		LVERROR(head, "InitDisplaySize failed");
		return false;
	}
	if (!InitFOV(displayConfig))
	{
		LVERROR(head, "InitFOV failed");
		return false;
	}

	bValid = true;
	return bValid;
}

bool FOSVRDescription::IsValid() const
{
	return bValid;
}

LVVec2 FOSVRDescription::GetDisplaySize(EnumEyeID eye) const
{
	const CHAR* head = "FOSVRDescription::GetDisplaySize";

	if (eye == SEnumEyeL)
	{
		return Data->DisplaySize[0];
	}
	else if (eye == SEnumEyeR)
	{
		return Data->DisplaySize[1];
	}

	LVERROR(head, "invalid eye: %d", eye);
	return LVVec2(0.0f, 0.0f);
}

LVVec2 FOSVRDescription::GetFov(EnumEyeID eye) const
{
	const CHAR* head = "FOSVRDescription::GetFov";

	if (eye == SEnumEyeL)
	{
		return Data->Fov[0];
	}
	else if (eye == SEnumEyeR)
	{
		return Data->Fov[1];
	}

	LVERROR(head, "invalid eye: %d", eye);
	return LVVec2(0.0f, 0.0f);
}

LVMatrix FOSVRDescription::GetProjectionMatrix(EnumEyeID eye, OSVR_DisplayConfig displayConfig, float nearClip, float farClip) const
{
	const CHAR* head = "FOSVRDescription::GetProjectionMatrix";
	OSVR_EyeCount osvrEye = (eye == SEnumEyeL ? 0 : 1);
	double left, right, bottom, top;
	OSVR_ReturnCode rc;
	rc = osvrClientGetViewerEyeSurfaceProjectionClippingPlanes(displayConfig, 0, osvrEye, 0, &left, &right, &bottom, &top);
	if (rc != OSVR_RETURN_SUCCESS)
	{
		LVERROR(head, "osvrClientGetViewerEyeSurfaceProjectionClippingPlanes failed");
		return LVMatrix();
	}

	// The steam plugin inverts the clipping planes here, but that doesn't appear to
	// be necessary for the OSVR calculated planes.

	return GetProjectionMatrix(
		static_cast<float>(left),
		static_cast<float>(right),
		static_cast<float>(bottom),
		static_cast<float>(top),
		nearClip, farClip);
}

LVMatrix FOSVRDescription::GetViewMatrix(EnumEyeID eye) const
{
	LVMatrix mat = DirectX::XMMatrixIdentity();
	mat.r[0].m128_f32[3] = m_ipd * 0.5f * (eye == SEnumEyeL ? -1 : 1);
	mat = DirectX::XMMatrixTranspose(mat);
	mat = DirectX::XMMatrixInverse(nullptr, mat);
	return mat;
}

LVMatrix FOSVRDescription::GetProjectionMatrix(float left, float right, float bottom, float top, float nearClip, float farClip) const
{
	const CHAR* head = "FOSVRDescription::GetDisplaySize";
	OSVR_ProjectionMatrix projection;
	projection.left = static_cast<double>(left);
	projection.right = static_cast<double>(right);
	projection.top = static_cast<double>(top);
	projection.bottom = static_cast<double>(bottom);
	projection.nearClip = static_cast<double>(nearClip);
	// @todo Since farClip may be FLT_MAX, round-trip casting to double
	// and back (via the OSVR_Projection_to_Unreal call) it should
	// be checked for conversion issues.
	projection.farClip = static_cast<double>(farClip);
	float p[16];
	OSVR_Projection_to_D3D(p, projection);
	//OSVR_Projection_to_Unreal(p, projection);
	LVMatrix ret;
	memcpy(&ret.r[0], p, sizeof(p));
	return ret;
}

bool FOSVRDescription::OSVRViewerFits(OSVR_DisplayConfig displayConfig)
{
	const CHAR* head = "FOSVRDescription::OSVRViewerFits";
	// if the display config hasn't started up, we can't tell yet
	if (osvrClientCheckDisplayStartup(displayConfig) == OSVR_RETURN_FAILURE)
	{
		LVERROR(head, "osvrClientCheckDisplayStartup call failed. Perhaps the HMD isn't connected?");
		return false;
	}

	OSVR_ReturnCode returnCode;

	// must be only one display input
	OSVR_DisplayInputCount numDisplayInputs;
	returnCode = osvrClientGetNumDisplayInputs(displayConfig, &numDisplayInputs);
	if (returnCode == OSVR_RETURN_FAILURE || numDisplayInputs != 1)
	{
		LVERROR(head, "osvrClientGetNumDisplayInputs call failed or number of display inputs not equal to 1");
		return false;
	}

	// must be only one viewer
	OSVR_ViewerCount numViewers;
	returnCode = osvrClientGetNumViewers(displayConfig, &numViewers);
	if (returnCode == OSVR_RETURN_FAILURE || numViewers != 1)
	{
		LVERROR(head, "osvrClientGetNumViewers call failed or number of viewers not equal to 1");
		return false;
	}

	// the one viewer must have two eyes
	OSVR_EyeCount numEyes;
	returnCode = osvrClientGetNumEyesForViewer(displayConfig, 0, &numEyes);
	if (returnCode == OSVR_RETURN_FAILURE || numEyes != 2)
	{
		LVERROR(head, "osvrClientGetNumEyesForViewer call failed or number of eyes not equal to 2");
		return false;
	}

	// each eye must have only one surface

	// left eye
	OSVR_ViewerCount numLeftEyeSurfaces, numRightEyeSurfaces;
	returnCode = osvrClientGetNumSurfacesForViewerEye(displayConfig, 0, 0, &numLeftEyeSurfaces);
	if (returnCode == OSVR_RETURN_FAILURE || numLeftEyeSurfaces != 1)
	{
		LVERROR(head, "osvrClientGetNumSurfacesForViewerEye call failed for the left eye, or number of surfaces not equal to 1");
		return false;
	}

	// right eye
	returnCode = osvrClientGetNumSurfacesForViewerEye(displayConfig, 0, 1, &numRightEyeSurfaces);
	if (returnCode == OSVR_RETURN_FAILURE || numRightEyeSurfaces != 1)
	{
		LVERROR(head, "osvrClientGetNumSurfacesForViewerEye call failed for the right eye, or number of surfaces not equal to 1");
		return false;
	}

	return true;
}

float FOSVRDescription::GetInterpupillaryDistance() const
{
	return m_ipd;
}

FOSVRDescription::FOSVRDescription(FOSVRDescription&)
{

}

FOSVRDescription::~FOSVRDescription()
{
	bValid = false;
	SAFE_DELETE(Data);
}

FOSVRDescription& FOSVRDescription::operator=(FOSVRDescription&)
{
	return *this;
}

FOSVRDescription::FOSVRDescription() : bValid(false), Data(new FDescriptionData)
{

}

bool FOSVRDescription::InitIPD(OSVR_DisplayConfig displayConfig)
{
	const CHAR* head = "FOSVRDescription::InitIPD";
	OSVR_Pose3 leftEye, rightEye;
	OSVR_ReturnCode returnCode;

	returnCode = osvrClientGetViewerEyePose(displayConfig, 0, 0, &leftEye);
	if (returnCode == OSVR_RETURN_FAILURE)
	{
		LVERROR(head, "osvrClientGetViewerEyePose call failed for left eye");
		return false;
	}

	returnCode = osvrClientGetViewerEyePose(displayConfig, 0, 1, &rightEye);
	if (returnCode == OSVR_RETURN_FAILURE)
	{
		LVERROR(head, "osvrClientGetViewerEyePose call failed for right eye");
		return false;
	}

	double dx = leftEye.translation.data[0] - rightEye.translation.data[0];
	double dy = leftEye.translation.data[1] - rightEye.translation.data[1];
	double dz = leftEye.translation.data[2] - rightEye.translation.data[2];

	m_ipd = (float)std::sqrt(dx * dx + dy * dy + dz * dz);
	return true;
}

bool FOSVRDescription::InitDisplaySize(OSVR_DisplayConfig displayConfig)
{
	const CHAR* head = "FOSVRDescription::InitDisplaySize";
	OSVR_ReturnCode returnCode;

	// left eye surface (only one surface per eye supported)
	OSVR_ViewportDimension leftViewportLeft, leftViewportBottom, leftViewportWidth, leftViewportHeight;
	returnCode = osvrClientGetRelativeViewportForViewerEyeSurface(displayConfig, 0, 0, 0,
		&leftViewportLeft, &leftViewportBottom, &leftViewportWidth, &leftViewportHeight);
	if (returnCode == OSVR_RETURN_FAILURE)
	{
		LVERROR(head, "osvrClientGetRelativeViewportForViewerEyeSurface call failed for left eye surface");
		return false;
	}

	// right eye surface (only one surface per eye supported)
	OSVR_ViewportDimension rightViewportLeft, rightViewportBottom, rightViewportWidth, rightViewportHeight;
	returnCode = osvrClientGetRelativeViewportForViewerEyeSurface(displayConfig, 0, 1, 0,
		&rightViewportLeft, &rightViewportBottom, &rightViewportWidth, &rightViewportHeight);
	if (returnCode == OSVR_RETURN_FAILURE)
	{
		LVERROR(head, "osvrClientGetRelativeViewportForViewerEyeSurface call failed for right eye surface");
		return false;
	}

	Data->DisplaySize[0].x = (float)leftViewportWidth;
	Data->DisplaySize[0].y = (float)leftViewportHeight;
	Data->DisplaySize[1].x = (float)rightViewportWidth;
	Data->DisplaySize[1].y = (float)rightViewportHeight;
	return true;
}

bool FOSVRDescription::InitFOV(OSVR_DisplayConfig displayConfig)
{
	const CHAR* head = "FOSVRDescription::InitFOV";
	OSVR_ReturnCode returnCode;
	for (OSVR_EyeCount eye = 0; eye < 2; eye++)
	{
		double left, right, top, bottom;
		returnCode = osvrClientGetViewerEyeSurfaceProjectionClippingPlanes(displayConfig, 0, 0, eye, &left, &right, &bottom, &top);
		if (returnCode == OSVR_RETURN_FAILURE)
		{
			LVERROR(head, "osvrClientGetViewerEyeSurfaceProjectionClippingPlanes call failed");
			return false;
		}

		double horizontalFOV = 180.0 / 3.141592654 * (std::atan(std::abs(left)) + std::atan(std::abs(right)));
		double verticalFOV = 180.0 / 3.141592654 * (std::atan(std::abs(top)) + std::atan(std::abs(bottom)));
		Data->Fov[eye].x = (float)horizontalFOV;
		Data->Fov[eye].y = (float)verticalFOV;
	}

	return true;
}

static FOSVR* SInst = new FOSVR("osvr");