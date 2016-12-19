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

void LostVR::OnPresent_Direct3D9Ex(IDirect3DDevice9Ex* device)
{
	ScopedHighFrequencyCounter Count("LostVR::OnPresent_Direct3D9Ex");

	auto dev = GetDevice();
	if (dev == nullptr)
	{
		bAllowConnect = true;
		return;
	}

	//dev->OnPresent_Direct3D9(device);

	//SetupInput();
	//PollInputDevice(nullptr);
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

	static void RegisterInputCallback()
	{
		RegisterKeyboardCallback(DIK_LSHIFT, KeyboardEventDetector_SWShift::OnKeyUpdate_Shift);
		RegisterKeyboardCallback(DIK_RSHIFT, KeyboardEventDetector_SWShift::OnKeyUpdate_Shift);
		RegisterKeyboardCallback(DIK_S, KeyboardEventDetector_SWShift::OnKeyUpdate_S);
		RegisterKeyboardCallback(DIK_W, KeyboardEventDetector_SWShift::OnKeyUpdate_W);
	}
};

bool KeyboardEventDetector_SWShift::SPressed_Shift = false;
bool KeyboardEventDetector_SWShift::SReleased_S = false;
bool KeyboardEventDetector_SWShift::SReleased_W = false;

class KeyboardEventDetector_ArrowShift
{
public:
	static bool SPressed_Shift;
	static bool SReleased_Left;
	static bool SReleased_Right;
	static bool SReleased_Up;
	static bool SReleased_Down;

	static void Update()
	{
		if (KeyboardEventDetector_ArrowShift::SPressed_Shift)
		{
			if (KeyboardEventDetector_ArrowShift::SReleased_Left)
			{
				//SendMessageA(SGlobalSharedDataInst.GetTargetWindow(), )
			}
		}
	}

	static void _cdecl OnKeyUpdate_Shift(bool bReleased)
	{

	}

	static void _cdecl OnKeyUpdate_Left(bool bReleased)
	{

	}

	static void _cdecl OnKeyUpdate_Right(bool bReleased)
	{

	}

	static void _cdecl OnKeyUpdate_Top(bool bReleased)
	{

	}

	static void _cdecl OnKeyUpdate_Down(bool bReleased)
	{

	}

	static void RegisterInputCallback()
	{
		RegisterKeyboardCallback(DIK_LSHIFT, KeyboardEventDetector_ArrowShift::OnKeyUpdate_Shift);
		RegisterKeyboardCallback(DIK_RSHIFT, KeyboardEventDetector_ArrowShift::OnKeyUpdate_Shift);
		RegisterKeyboardCallback(DIK_LEFTARROW, KeyboardEventDetector_ArrowShift::OnKeyUpdate_Left);
		RegisterKeyboardCallback(DIK_RIGHTARROW, KeyboardEventDetector_ArrowShift::OnKeyUpdate_Right);
		RegisterKeyboardCallback(DIK_UPARROW, KeyboardEventDetector_ArrowShift::OnKeyUpdate_Top);
		RegisterKeyboardCallback(DIK_DOWNARROW, KeyboardEventDetector_ArrowShift::OnKeyUpdate_Down);
	}
};

bool KeyboardEventDetector_ArrowShift::SPressed_Shift = false;
bool KeyboardEventDetector_ArrowShift::SReleased_Left = false;
bool KeyboardEventDetector_ArrowShift::SReleased_Right  = false;
bool KeyboardEventDetector_ArrowShift::SReleased_Up = false;
bool KeyboardEventDetector_ArrowShift::SReleased_Down = false;

void LostVR::SetupInput()
{
	static bool SInitialized = false;
	if (SInitialized)
	{
		return;
	}

	if (SGlobalSharedDataInst.GetTargetWindow() == NULL)
	{
		return;
	}

	HWND wnd = SGlobalSharedDataInst.GetTargetWindow();
	SetupInputDevice(&wnd);
	KeyboardEventDetector_SWShift::RegisterInputCallback();
	SInitialized = true;
}

IVRDevice::IVRDevice(const std::string& key) : Key(key)
{
	LostVR::Get()->RegisterVRDevice(GetKeyString(), this);
}
