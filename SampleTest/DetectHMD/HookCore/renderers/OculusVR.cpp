#include "HookCorePCH.h"
#include "OculusVR.h"

using namespace lostvr;

OculusVR::OculusVR() :
	Session(nullptr),
	HmdDesc(),
	TrackerDescArray(),
	presentResult(ovrSuccess),
	bInitialized(false),
	Renderer(nullptr)
{
	// Set all layers to zero
	for (int i = 0; i < ovrMaxLayerCount; ++i)
		Layer[i] = nullptr;
}

OculusVR::~OculusVR()
{
	Shutdown();
}

bool OculusVR::Startup()
{
	// Initializes LibOVR, and the Rift
	ovrResult result;
	ovrDetectResult detectResult = ovr_Detect(0);
	LVMSG("Oculus startup", "detect result: IsOculusServiceRunning(%d), IsOculusHMDConnected(%d)", detectResult.IsOculusServiceRunning, detectResult.IsOculusHMDConnected);

	if (detectResult.IsOculusServiceRunning)
	{
		ovrInitParams initParams;
		memset(&initParams, 0, sizeof(initParams));
		initParams.Flags = ovrInit_RequestVersion;
		initParams.RequestedMinorVersion = OVR_MINOR_VERSION;
		initParams.LogCallback = OvrLogCallback;
		result = ovr_Initialize(&initParams);
	}
	else
	{
		return false;
	}

	if (!OVR_SUCCESS(result))
	{
		ovrErrorInfo errorInfo;
		ovr_GetLastErrorInfo(&errorInfo);
		LVMSG("Oculus startup", "ovr_Initialize failed(%d), error message: %d, %s", result, errorInfo.Result, errorInfo.ErrorString);
		return false;
	}
	else
	{
		LVMSG("Oculus startup", "ovr_Initialize succeeded");
		const CHAR* clientid = "vronline";
		ovr_IdentifyClient(clientid);
	}

	ovrGraphicsLuid luid;
	result = ovr_Create(&Session, &luid);
	if (!OVR_SUCCESS(result))
	{
		if (result == ovrError_ServiceConnection || result == ovrError_ServiceError || result == ovrError_NotInitialized)
		{
			Shutdown();

			ovrInitParams initParams;
			memset(&initParams, 0, sizeof(initParams));
			initParams.Flags = ovrInit_RequestVersion;
			initParams.RequestedMinorVersion = OVR_MINOR_VERSION;
			initParams.LogCallback = OvrLogCallback;
			result = ovr_Initialize(&initParams);
			if (OVR_SUCCESS(result))
			{
				const CHAR* clientid = "vronline";
				ovr_IdentifyClient(clientid);
				result = ovr_Create(&Session, &luid);
				if (OVR_SUCCESS(result))
				{
					LVMSG("Oculus startup", "create after reinitialize success");
				}
				else
				{
					LVMSG("Oculus startup", "create after reinitialize failed: %d", result);
					return false;
				}
			}
			else
			{
				LVMSG("Oculus startup", "reinitialize failed: %d", result);
				return false;
			}
		}
		else
		{
			ovrErrorInfo errorInfo;
			ovr_GetLastErrorInfo(&errorInfo);
			LVMSG("Oculus startup", "ovr_Create failed(%d), error message: %d, %s", result, errorInfo.Result, errorInfo.ErrorString);
			return false;
		}
	}
	else
	{
		LVMSG("Oculus startup", "ovr_Create succeeded");
	}

	HmdDesc = ovr_GetHmdDesc(Session);
	return true;
}

void OculusVR::Shutdown()
{
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

	if (bInitialized)
	{
		ovr_Shutdown();
		bInitialized = false;
	}

	memset(&HmdDesc, 0, sizeof(HmdDesc));
}

bool OculusVR::IsConnected()
{
	return (Session != nullptr);
}

bool OculusVR::OnPresent_Direct3D11(IDXGISwapChain* swapChain)
{
	const CHAR* head = "OculusVR::OnPresent_Direct3D11";
	if (Renderer == nullptr)
	{
		Renderer = new Direct3D11Helper(EDirect3D::DeviceRef);
		if (Renderer == nullptr)
		{
			LVMSG(head, "alloc renderer object failed");
			return false;
		}
	}

	if (!Renderer->UpdateRHIWithSwapChain(swapChain) || !EnsureInitialized(swapChain))
	{
		return false;
	}

	Layer[0]->GetEyePoses();
	ID3D11Texture2D* dst = Layer[0]->pEyeRenderTexture[0]->GetBuffer();
	Renderer->OutputBuffer_Texture2D(dst);
	Layer[0]->Commit(0);
	ovrResult ret = DistortAndPresent(1);
	if (!OVR_SUCCESS(ret))
	{
		LVMSG(head, "failed(%d), ...", ret);
		return false;
	}

	return true;
}

bool OculusVR::OnPresent_Direct3D9(IDirect3DDevice9* device)
{
	const CHAR* head = "OculusVR::OnPresent_Direct3D9";

	if (Renderer == nullptr)
	{
		Renderer = new Direct3D11Helper(EDirect3D::DXGI0, device);
		if (Renderer == nullptr)
		{
			LVMSG(head, "alloc renderer object failed");
			return false;
		}
	}

	if (!EnsureInitialized(Renderer->GetSwapChain()))
	{
		return false;
	}

	if (!Renderer->UpdateBuffer_Direct3D9(device))
	{
		LVMSG(head, "update buffer with direct9 device failed");
		return false;
	}

	Layer[0]->GetEyePoses();
	ID3D11Texture2D* dst = Layer[0]->pEyeRenderTexture[0]->GetBuffer();
	Renderer->OutputBuffer_Texture2D(dst);
	Layer[0]->Commit(0);
	ovrResult ret = DistortAndPresent(1);
	if (!OVR_SUCCESS(ret))
	{
		LVMSG(head, "failed(%d), ...", ret);
		return false;
	}

	return true;


	return false;
}

const std::string OculusVR::GetDeviceString() const
{
	char strDesc[1024];
	snprintf(strDesc, 1023, "[ProductName: %s, Manufacturer: %s, SerialNumber: %s]",
		HmdDesc.ProductName, HmdDesc.Manufacturer, HmdDesc.SerialNumber);
	return std::string(strDesc);
}

//std::string OculusVR::GetKeyString() const
//{
//	return std::string("[OculusVR]");
//}

bool OculusVR::Init(ID3D11Device* Dev, DXGI_SWAP_CHAIN_DESC Desc)
{
	unsigned int trackerCount = ovr_GetTrackerCount(Session) < 1 ? 1 : ovr_GetTrackerCount(Session); // Make sure there's always at least one.
	for (unsigned int i = 0; i < trackerCount; ++i)
		TrackerDescArray.push_back(ovr_GetTrackerDesc(Session, i));

	{
		Layer[0] = new VRLayer(Dev, Desc, Session);
	}

	LVMSG("OculusVR Init", "sucess");
	bInitialized = true;
	return true;
}

bool OculusVR::EnsureInitialized(IDXGISwapChain * swapChain)
{
	const CHAR* head = "OculusVR::EnsureInitialized";

	if (!bInitialized)
	{
		if (swapChain == nullptr)
		{
			LVMSG(head, "null swap chain");
			return false;
		}

		DXGI_SWAP_CHAIN_DESC Desc;
		if (FAILED(swapChain->GetDesc(&Desc)))
		{
			LVMSG(head, "failed to get SwapChain's desc, SwapChain(%x).", swapChain);
			return false;
		}

		if (!(bInitialized = Init(Renderer->GetDevice(), Desc)))
		{
			LVMSG(head, "failed to initialize, SwapChain(%x).", swapChain);
			return false;
		}
	}

	return true;
}

ovrResult OculusVR::DistortAndPresent(int numLayersToRender, D3D11_BOX * optionalBoxForMirrorWindow/* = 0*/, bool mirror/* = true*/)
{
	ovrLayerHeader* layerHeaders[ovrMaxLayerCount];
	for (int i = 0; i < ovrMaxLayerCount; i++)
	{
		if (Layer[i])
		{
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

static OculusVR* SInst = new OculusVR;