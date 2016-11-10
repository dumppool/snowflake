/*
 * file IRenderer.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"
#include "IRenderer.h"
#include "input/InputDevice.h"

using namespace lostvr;

const CHAR* SLogPrefix = "LostVR";

LostVR::LostVR() : bAllowConnect(true)
{
	VRDevices.clear();
}

LostVR::~LostVR()
{
	for (auto dev : VRDevices)
	{
		if (dev.second != nullptr)
		{
			delete dev.second;
			dev.second = nullptr;
		}
	}

	VRDevices.clear();
}

void LostVR::RegisterVRDevice(const std::string& InKey, IVRDevice* InInst)
{
	auto it = VRDevices.find(InKey);
	if (VRDevices.end() != it)
	{
		LVMSG("LostVR::RegisterVRDevice", "unexpected registration on duplicate key: %s", InKey.c_str());
	}

	VRDevices[InKey] = InInst;
}

IVRDevice* LostVR::GetDevice()
{
	for (auto dev : VRDevices)
	{
		if (dev.second->IsConnected())
		{
			return dev.second;
		}

		if (bAllowConnect && dev.second->Startup())
		{
			bAllowConnect = false;
			LVMSG("LostVR::GetDevice", "connected to vr device: %s", dev.second->GetDeviceString().c_str());
			return dev.second;
		}
	}

	return nullptr;
}

IVRDevice* LostVR::GetDevice(const std::string& InKey) const
{
	auto it = VRDevices.find(InKey);
	if (it == VRDevices.end())
	{
		return nullptr;
	}

	return it->second;
}

void LostVR::OnPresent_Direct3D11(IDXGISwapChain* swapChain)
{
	ScopedHighFrequencyCounter Count("LostVR::OnPresent_Direct3D11");

	auto dev = GetDevice();
	if (dev == nullptr)
	{
		bAllowConnect = true;
		return;
	}

	dev->OnPresent_Direct3D11(swapChain);

	SetupInput();
	PollInputDevice(nullptr);
}

void LostVR::OnPresent_Direct3D9(IDirect3DDevice9* device)
{
	ScopedHighFrequencyCounter Count("LostVR::OnPresent_Direct3D9");

	auto dev = GetDevice();
	if (dev == nullptr)
	{
		bAllowConnect = true;
		return;
	}

	dev->OnPresent_Direct3D9(device);

	SetupInput();
	PollInputDevice(nullptr);
}

class KeyboardEventDetector_SWShift
{
public:
	static bool SPressed_Shift;
	static bool SReleased_S;
	static bool SReleased_W;

	static void Update()
	{
		if (SPressed_Shift && SReleased_S)
		{
			auto dev = LostVR::Get()->GetDevice();
			if (dev != nullptr)
			{
				dev->AddMovement(EMovement::CameraBack);
			}
		}
		else if (SPressed_Shift && SReleased_W)
		{
			auto dev = LostVR::Get()->GetDevice();
			if (dev != nullptr)
			{
				dev->AddMovement(EMovement::CameraFront);
			}
		}

		SReleased_S = false;
		SReleased_W = false;
	}

	static void _cdecl OnKeyUpdate_Shift(bool bReleased)
	{
		SPressed_Shift = !bReleased;
		Update();
	}

	static void _cdecl OnKeyUpdate_S(bool bReleased)
	{
		SReleased_S = bReleased;
		Update();
	}

	static void _cdecl OnKeyUpdate_W(bool bReleased)
	{
		SReleased_W = bReleased;
		Update();
	}
};

bool KeyboardEventDetector_SWShift::SPressed_Shift = false;
bool KeyboardEventDetector_SWShift::SReleased_S = false;
bool KeyboardEventDetector_SWShift::SReleased_W = false;

void LostVR::SetupInput()
{
	static bool SInitialized = false;
	if (SInitialized)
	{
		return;
	}

	HWND hwnd[16];
	UINT nwnd;
	if (GetWndFromProcessID(GetCurrentProcessId(), hwnd, nwnd))
	{
		LVMSG2(SLogPrefix, "LostVR::SetupInput", "window count: %d, window0: 0x%x", nwnd, hwnd[0]);
		//for (int i = 0; i < nwnd; ++i)
			//LVMSG2(SLogPrefix, "LostVR::SetupInput", "window: 0x%x", hwnd[i]);

		SetupInputDevice(&hwnd[nwnd - 1]);
		RegisterKeyboardCallback(DIK_LSHIFT, KeyboardEventDetector_SWShift::OnKeyUpdate_Shift);
		RegisterKeyboardCallback(DIK_RSHIFT, KeyboardEventDetector_SWShift::OnKeyUpdate_Shift);
		RegisterKeyboardCallback(DIK_S, KeyboardEventDetector_SWShift::OnKeyUpdate_S);
		RegisterKeyboardCallback(DIK_W, KeyboardEventDetector_SWShift::OnKeyUpdate_W);
		SInitialized = true;
	}
}

IVRDevice::IVRDevice(const std::string& key) : Key(key)
{
	LostVR::Get()->RegisterVRDevice(GetKeyString(), this);
}
