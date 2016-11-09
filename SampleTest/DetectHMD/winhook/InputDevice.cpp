#include "stdafx.h"
#include "InputDevice.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL          0x80000000
#endif

namespace lostvr
{
	static const CHAR* SLogPrefix = "InputDevice";

	struct MouseState
	{
		LONG lAxisX;
		LONG lAxisY;
		BYTE abButtons[3];
		BYTE bPadding;       // Structure must be DWORD multiple in size.   
	};

	DIOBJECTDATAFORMAT SMSObjectFormat[] =
	{
		{ &GUID_XAxis, FIELD_OFFSET(MouseState, lAxisX),    // X axis
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
		{ &GUID_YAxis, FIELD_OFFSET(MouseState, lAxisY),    // Y axis
		DIDFT_AXIS | DIDFT_ANYINSTANCE, 0 },
		{ 0, FIELD_OFFSET(MouseState, abButtons[0]),        // Button 0
		DIDFT_BUTTON | DIDFT_ANYINSTANCE, 0 },
		{ 0, FIELD_OFFSET(MouseState, abButtons[1]),        // Button 1 (optional)
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0 },
		{ 0, FIELD_OFFSET(MouseState, abButtons[2]),        // Button 2 (optional)
		DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0 }
	};

	DIDATAFORMAT            SMSDataFormat =
	{
		sizeof(DIDATAFORMAT),
		sizeof(DIOBJECTDATAFORMAT),
		DIDF_ABSAXIS,
		sizeof(MouseState),
		(sizeof(SMSObjectFormat) / sizeof(DIOBJECTDATAFORMAT)),
		SMSObjectFormat
	};

	class InputDevice
	{
	protected:
		LPDIRECTINPUT8          DirectInput;   

		LPDIRECTINPUTDEVICE8    DID_Mouse;
		LPDIRECTINPUTDEVICE8    DID_Keyboard;
		LPDIRECTINPUTDEVICE8    DID_Joystick;

	public:
		InputDevice();
		~InputDevice();

		void Initialize(HWND wnd, bool bEnableMouse = true, bool bEnableKeyboard = true, bool bEnableJoystick = false);
		void DestroyDevice();

		bool IsMouseEnabled() const;
		bool IsKeyboardEnabled() const;
		bool IsJoystickEnabled() const;

		void Poll();

		void PollMouseState();
		void PollKeyboardState();
		void PollJoystickState();
	};

	static InputDevice SInputDevice;

	InputDevice::InputDevice() : DirectInput(nullptr)
		, DID_Mouse(nullptr)
		, DID_Keyboard(nullptr)
		, DID_Joystick(nullptr)
	{
	}

	InputDevice::~InputDevice()
	{
		DestroyDevice();
	}

	void InputDevice::Initialize(HWND wnd, bool bEnableMouse, bool bEnableKeyboard, bool bEnableJoystick)
	{
		const CHAR* head = "InputDevice::Initialize";
		LVMSG2(SLogPrefix, head, "window: 0x%x, thread id: %d", wnd, GetCurrentThreadId());

		DestroyDevice();

		HRESULT hr = S_FALSE;
		bool bContinue = true;

		//hr = DirectInput8Create(GetModuleHandleA(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
		//if (FAILED(hr))
		//{
		//	return;
		//}

		//hr = DirectInput->CreateDevice(GUID_SysMouse, &DID_Mouse, NULL);
		//if (FAILED(hr))
		//{
		//	return;
		//}

		//hr = DID_Mouse->SetDataFormat(&g_dfMouse2);
		//if (FAILED(hr))
		//{
		//	return;
		//}

		//hr = DID_Mouse->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		//if (FAILED(hr))
		//{
		//	return;
		//}


		VERIFY_HRESULT2(DirectInput8Create(GetModuleHandleA(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL),
		{ return; }, SLogPrefix, head, "create direct input 8 failed: 0x%x(%d)", hr, hr);

		if (bEnableMouse)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_SysMouse, &DID_Mouse, NULL),
			{ bContinue = false; }, SLogPrefix, head, "create mouse device failed: 0x%x(%d)", hr, hr);

			if (bContinue)
			{
				VERIFY_HRESULT2(DID_Mouse->SetDataFormat(&SMSDataFormat),
				{ bContinue = false; SAFE_RELEASE(DID_Mouse); }, SLogPrefix, head, "set mouse data format failed: 0x%x(%d)", hr, hr);
			}

			if (bContinue)
			{
				VERIFY_HRESULT2(DID_Mouse->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND),
				{ bContinue = false; SAFE_RELEASE(DID_Mouse); }, SLogPrefix, head, "set mouse coorperative level failed: 0x%x(%d)", hr, hr);
			}
		}

		if (bEnableKeyboard)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_SysKeyboard, &DID_Keyboard, NULL),
			{ bContinue = false; }, SLogPrefix, head, "create keyboard device failed: 0x%x(%d)", hr, hr);

		}

		if (bEnableJoystick)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_Joystick, &DID_Joystick, NULL),
			{ bContinue = false; }, SLogPrefix, head, "create joystick device failed: 0x%x(%d)", hr, hr);
		}

		LVMSG2(SLogPrefix, head, "initialize InputDevice successfully");
		LVMSG2(SLogPrefix, head, "err code, DIERR_ACQUIRED: 0x%x, DIERR_INPUTLOST: 0x%x, DIERR_OLDDIRECTINPUTVERSION: 0x%x, DIERR_BETADIRECTINPUTVERSION: 0x%x",
			DIERR_ACQUIRED, DIERR_INPUTLOST, DIERR_OLDDIRECTINPUTVERSION, DIERR_BETADIRECTINPUTVERSION);
	}

	void InputDevice::DestroyDevice()
	{
		if (DID_Mouse)
		{
			DID_Mouse->Unacquire();
		}

		if (DID_Keyboard)
		{
			DID_Keyboard->Unacquire();
		}

		if (DID_Joystick)
		{
			DID_Joystick->Unacquire();
		}

		SAFE_RELEASE(DID_Mouse);
		SAFE_RELEASE(DID_Keyboard);
		SAFE_RELEASE(DID_Joystick);
		SAFE_RELEASE(DirectInput);
	}

	bool InputDevice::IsMouseEnabled() const
	{
		return DID_Mouse != nullptr;
	}

	bool InputDevice::IsKeyboardEnabled() const
	{
		return DID_Keyboard != nullptr;
	}

	bool InputDevice::IsJoystickEnabled() const
	{
		return DID_Joystick != nullptr;
	}

	void InputDevice::Poll()
	{
		if (IsMouseEnabled())
		{
			PollMouseState();
		}

		if (IsKeyboardEnabled())
		{
			PollKeyboardState();
		}

		if (IsJoystickEnabled())
		{
			PollJoystickState();
		}
	}

	void InputDevice::PollMouseState()
	{
		const CHAR* head = "InputDevice::PollMouseState";

		if (DID_Mouse == nullptr)
		{
			return;
		}

		HRESULT hr = S_FALSE;

		MouseState ms;

		hr = DID_Mouse->Poll();
		if (FAILED(hr))
		{
			hr = DID_Mouse->Acquire();
			LVMSG2(SLogPrefix, head, "mouse poll failed, acquire result: 0x%x(%d), thread id: %d", hr, hr, GetCurrentThreadId());
			return;
		}

		hr = DID_Mouse->GetDeviceState(sizeof(MouseState), &ms);
		if (FAILED(hr))
		{
			LVMSG2(SLogPrefix, head, "get mouse state failed: 0x%x(%d)", hr, hr);
			return;
		}

		const CHAR* down = "down";
		const CHAR* up = "up";
		LVMSG2(SLogPrefix, head, "mouse state, axis x: %d, axis y: %d, btn 0: %s, btn 1: %s, btn 2: %s",
			ms.lAxisX, ms.lAxisY, ms.abButtons[0] & 0x80 ? down : up, ms.abButtons[1] & 0x80 ? down : up, ms.abButtons[2] & 0x80 ? down : up);
	}

	void InputDevice::PollKeyboardState()
	{
	}

	void InputDevice::PollJoystickState()
	{
	}
};

void SetupInputDevice(void * p)
{
	lostvr::SInputDevice.Initialize(*(HWND*)p);
}

void PollInputDevice(void * p)
{
	lostvr::SInputDevice.Poll();
}
