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

LostVR::LostVR() : bAllowConnect(true),
InputHandler(nullptr)
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

class FInputEventHandler : IInputDeviceCallback
{
	uint8			KeyStates[256];
	EInputAction	KeyEvents[256];
	bool			bHasKeyInput;

	DIJOYSTATE		JoyStates;
	EInputAction	ButtonEvents[32];
	bool			bHasButtonInput;

	EInputAction ConsumeKeyAction(int key)
	{
		const CHAR* head = "KeyboardEventDetector::ConsumeKeyAction";
		if (key < 0 || 256 <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return EInputAction::UnDefined;
		}

		EInputAction ret = KeyEvents[key];
		KeyEvents[key] = EInputAction::UnDefined;
		return ret;
	}

	bool IsKeyDown(int key) const
	{
		const CHAR* head = "KeyboardEventDetector::IsKeyDown";
		if (key < 0 || 256 <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return false;
		}

		return (KeyStates[key] != 0);
	}

	EInputAction ConsumeButtonAction(int key)
	{
		const CHAR* head = "KeyboardEventDetector::ConsumeButtonAction";
		if (key < 0 || 32 <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return EInputAction::UnDefined;
		}

		EInputAction ret = ButtonEvents[key];
		ButtonEvents[key] = EInputAction::UnDefined;
		return ret;
	}

	bool IsButtonDown(int key) const
	{
		const CHAR* head = "KeyboardEventDetector::IsButtonDown";
		if (key < 0 || 32 <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return false;
		}

		int v0 = JoyStates.rgbButtons[key];
		return (v0 != 0);
	}

	void OutputJoystickInfo()
	{
		char info[256];
		snprintf(info, 255, "\nx:%d, y:%d, z:%d, rx:%d, ry:%d, rz:%d, pov0:%d, pov1:%d, pov2:%d, pov3:%d, slider0:%d, slider1:%d",
			(int)JoyStates.lX, (int)JoyStates.lY, (int)JoyStates.lZ,
			(int)JoyStates.lRx, (int)JoyStates.lRy, (int)JoyStates.lRz,
			(int)JoyStates.rgdwPOV[0], (int)JoyStates.rgdwPOV[1], (int)JoyStates.rgdwPOV[2], (int)JoyStates.rgdwPOV[3],
			(int)JoyStates.rglSlider[0], (int)JoyStates.rglSlider[1]);

		OutputDebugStringA(info);
	}

public:
	virtual void CALLBACK OnKeyboardAction(int key, EInputAction value) override
	{
		const CHAR* head = "KeyboardEventDetector::OnKeyboardAction";
		if (key < 0 || 256 <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return;
		}

		bHasKeyInput = true;
		KeyEvents[key] = value;
		KeyStates[key] = value == EInputAction::Pressed ? 1 : 0;
	}

	virtual void CALLBACK OnJoystickAction(int key, EInputAction value) override
	{
		const CHAR* head = "KeyboardEventDetector::OnJoystickAction";
		if (key < 0 || 32 <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return;
		}

		bHasButtonInput = true;
		ButtonEvents[key] = value;
		JoyStates.rgbButtons[key] = value == EInputAction::Pressed ? 1 : 0;
	}

	virtual void CALLBACK OnJoystickAxis(int key, long value) override
	{
		const CHAR* head = "KeyboardEventDetector::OnJoystickAxis";
		if (key < 0 || sizeof(DIJOYSTATE) <= key)
		{
			LVERROR(head, "out of range: %d", key);
			return;
		}

		if (key <= DIJOFS_POV(3) && DIJOFS_POV(0) <= key)
		{
			*(DWORD*)((uint8*)&JoyStates + key) = (DWORD)value;
		}
		else
		{
			*(long*)((uint8*)&JoyStates + key) = (long)value;
		}
	}

	virtual void CALLBACK OnInputBegin() override
	{
		bHasKeyInput = false;
		bHasButtonInput = false;
	}

	virtual void CALLBACK OnInputEnd() override
	{
		if (bHasKeyInput)
		{
			if (ConsumeKeyAction(DIK_S) == EInputAction::Released && (IsKeyDown(DIK_LSHIFT) || IsKeyDown(DIK_RSHIFT)))
			{
				auto dev = LostVR::Get()->GetDevice();
				if (dev != nullptr)
				{
					dev->AddMovement(EMovement::CameraBack);
				}
			}
			
			if (ConsumeKeyAction(DIK_W) == EInputAction::Released && (IsKeyDown(DIK_LSHIFT) || IsKeyDown(DIK_RSHIFT)))
			{
				auto dev = LostVR::Get()->GetDevice();
				if (dev != nullptr)
				{
					dev->AddMovement(EMovement::CameraFront);
				}
			}
			
			if (ConsumeKeyAction(DIK_M) == EInputAction::Released && (IsKeyDown(DIK_LSHIFT) || IsKeyDown(DIK_RSHIFT)))
			{
				SGlobalSharedDataInst.SetBFakeMouseMove(!SGlobalSharedDataInst.GetBFakeMouseMove());
			}


			if (ConsumeKeyAction(DIK_J) == EInputAction::Released && (IsKeyDown(DIK_LSHIFT) || IsKeyDown(DIK_RSHIFT)))
			{
				SGlobalSharedDataInst.SetBEnableJoystick(!SGlobalSharedDataInst.GetBEnableJoystick());
			}


			if (ConsumeKeyAction(DIK_N) == EInputAction::Released && (IsKeyDown(DIK_LSHIFT) || IsKeyDown(DIK_RSHIFT)))
			{
				SGlobalSharedDataInst.SetBHoldWhenMoving(!SGlobalSharedDataInst.GetBHoldWhenMoving());
			}
		}

		if (SGlobalSharedDataInst.GetBEnableJoystick())
		{
			if (bHasButtonInput)
			{
				EInputAction action = ConsumeButtonAction(0);
				if (action == EInputAction::Pressed)
				{
					FakeMousePress(true);
				}
				else if (action == EInputAction::Released)
				{
					FakeMouseRelease(true);
				}

				action = ConsumeButtonAction(1);
				if (action == EInputAction::Pressed)
				{
					FakeMousePress(false);
				}
				else if (action == EInputAction::Released)
				{
					FakeMouseRelease(false);
				}

				action = ConsumeButtonAction(6);
				if (action == EInputAction::Pressed)
				{
					FakeKeyPress(VK_ESCAPE);
				}
				else if (action == EInputAction::Released)
				{
					FakeKeyRelease(VK_ESCAPE);
				}
			}

			const uint32 THRESHOLD_SMALL = 1 << 12;
			const uint32 THRESHOLD_LARGE = (1 << 15) | (1 << 14);
			static bool SX0 = false;
			static bool SX1 = false;
			static bool SY0 = false;
			static bool SY1 = false;
			if (JoyStates.lX < THRESHOLD_SMALL)
			{
				if (!SX0)
				{
					SX0 = true;
					FakeKeyPress('A');
				}
			}
			else
			{
				if (SX0)
				{
					SX0 = false;
					FakeKeyRelease('A');
				}
			}

			if (JoyStates.lX > THRESHOLD_LARGE)
			{
				if (!SX1)
				{
					SX1 = true;
					FakeKeyPress('D');
				}
			}
			else
			{
				if (SX1)
				{
					SX1 = false;
					FakeKeyRelease('D');
				}
			}

			if (JoyStates.lY < THRESHOLD_SMALL)
			{
				if (!SY0)
				{
					SY0 = true;
					FakeKeyPress('W');
				}
			}
			else
			{
				if (SY0)
				{
					SY0 = false;
					FakeKeyRelease('W');
				}
			}

			if (JoyStates.lY > THRESHOLD_LARGE)
			{
				if (!SY1)
				{
					SY1 = true;
					FakeKeyPress('S');
				}
			}
			else
			{
				if (SY1)
				{
					SY1 = false;
					FakeKeyRelease('S');
				}
			}
		}
	}

	void Register()
	{
		memset(KeyStates, 0, sizeof(KeyStates));
		memset(KeyEvents, 0, sizeof(KeyEvents));
		memset(&JoyStates, 0, sizeof(JoyStates));
		memset(ButtonEvents, 0, sizeof(ButtonEvents));
		RegisterInputCallback(this);
	}
};

void LostVR::SetupInput()
{
	static bool SInitialized = false;

	HWND wnd;
	if (InputHandler != nullptr)
	{
		return;
	}

	if ((wnd = SGlobalSharedDataInst.GetTargetWindow()) == NULL)
	{
		return;
	}

	SetupInputDevice(&wnd);
	InputHandler = new FInputEventHandler;
	InputHandler->Register();
	SInitialized = true;
}

IVRDevice::IVRDevice(const std::string& key) : Key(key)
{
	LostVR::Get()->RegisterVRDevice(GetKeyString(), this);
}
