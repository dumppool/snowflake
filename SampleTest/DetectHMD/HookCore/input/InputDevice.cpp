/*
 * file InputDevice.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */

#include "HookCorePCH.h"
#include "InputDevice.h"

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
		DIDF_RELAXIS,
		sizeof(MouseState),
		(sizeof(SMSObjectFormat) / sizeof(DIOBJECTDATAFORMAT)),
		SMSObjectFormat
	};

	class InputDevice
	{
	protected:
		LPDIRECTINPUT8          DirectInput;   

		LPDIRECTINPUTDEVICE8    Mouse;
		LPDIRECTINPUTDEVICE8    Keyboard;
		LPDIRECTINPUTDEVICE8    Joystick;

		UCHAR KeyStates[2][256];
		DIJOYSTATE JoyStates[2];

		std::vector<IInputDeviceCallback*> CallbackObjects;

	public:
		InputDevice();
		~InputDevice();

		void Initialize(HWND wnd, bool bEnableMouse = true, bool bEnableKeyboard = true, bool bEnableJoystick = false);
		void Poll();
		void RegisterCallback(IInputDeviceCallback* obj);

	protected:
		void DestroyDevice();

		bool IsMouseEnabled() const;
		bool IsKeyboardEnabled() const;
		bool IsJoystickEnabled() const;


		void PollMouseState();
		void PollKeyboardState();
		void PollJoystickState();

	};

	static InputDevice SInputDevice;

	InputDevice::InputDevice() : DirectInput(nullptr)
		, Mouse(nullptr)
		, Keyboard(nullptr)
		, Joystick(nullptr)
	{
		memset(KeyStates, 0, sizeof(KeyStates));
		memset(JoyStates, 0, sizeof(JoyStates));
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

		VERIFY_HRESULT2(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL),
		{ return; }, SLogPrefix, head, "create direct input 8 failed: 0x%x(%d)", hr, hr);

		if (bEnableMouse)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_SysMouse, &Mouse, NULL), 
			{ bContinue = false; },	SLogPrefix, head, "create mouse device failed: 0x%x(%d)", hr, hr);
			
			if (bContinue)
			{
				VERIFY_HRESULT2(Mouse->SetDataFormat(&SMSDataFormat),
				{ bContinue = false; SAFE_RELEASE(Mouse); }, SLogPrefix, head, "set mouse data format failed: 0x%x(%d)", hr, hr);
			}

			if (bContinue)
			{
				VERIFY_HRESULT2(Mouse->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND), 
				{ bContinue = false; SAFE_RELEASE(Mouse); }, SLogPrefix, head, "set mouse coorperative level failed: 0x%x(%d)", hr, hr);
			}
		}

		if (!bContinue)
		{
			LVERROR(head, "initialize mouse failed");
		}

		if (bEnableKeyboard)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, NULL),
			{ bContinue = false; }, SLogPrefix, head, "create keyboard device failed: 0x%x(%d)", hr, hr);

			if (bContinue)
			{
				VERIFY_HRESULT2(Keyboard->SetDataFormat(&c_dfDIKeyboard),
				{ bContinue = false; SAFE_RELEASE(Keyboard); }, SLogPrefix, head, "set keyboard data format failed: 0x%x(%d)", hr, hr);
			}

			if (bContinue)
			{
				VERIFY_HRESULT2(Keyboard->SetCooperativeLevel(wnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND),
				{ bContinue = false; SAFE_RELEASE(Keyboard); }, SLogPrefix, head, "set keyboard coorperative level failed: 0x%x(%d)", hr, hr);
			}
		}

		if (!bContinue)
		{
			LVERROR(head, "initialize keyboard failed");
		}

		if (bEnableJoystick)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_Joystick, &Joystick, NULL),
			{ bContinue = false; }, SLogPrefix, head, "create joystick device failed: 0x%x(%d)", hr, hr);

			if (bContinue)
			{
				VERIFY_HRESULT2(Joystick->SetDataFormat(&c_dfDIJoystick),
				{ bContinue = false; SAFE_RELEASE(Joystick); }, SLogPrefix, head, "create joystick device failed: 0x%x(%d)", hr, hr);
			}

			if (bContinue)
			{
				VERIFY_HRESULT2(Joystick->SetCooperativeLevel(wnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND),
				{ bContinue = false; SAFE_RELEASE(Joystick); }, SLogPrefix, head, "set joystick coorperative level failed: 0x%x(%d)", hr, hr);
			}
		}

		if (!bContinue)
		{
			LVERROR(head, "initialize joystick failed");
			return;
		}

		LVMSG2(SLogPrefix, head, "initialize InputDevice successfully");
		//LVMSG2(SLogPrefix, head, "err code, DIERR_ACQUIRED: 0x%x, DIERR_INPUTLOST: 0x%x, DIERR_OLDDIRECTINPUTVERSION: 0x%x, DIERR_BETADIRECTINPUTVERSION: 0x%x",
			//DIERR_ACQUIRED, DIERR_INPUTLOST, DIERR_OLDDIRECTINPUTVERSION, DIERR_BETADIRECTINPUTVERSION);
	}

	void InputDevice::DestroyDevice()
	{
		if (Mouse)
		{
			Mouse->Unacquire();
		}

		if (Keyboard)
		{
			Keyboard->Unacquire();
		}

		if (Joystick)
		{
			Joystick->Unacquire();
		}

		SAFE_RELEASE(Mouse);
		SAFE_RELEASE(Keyboard);
		SAFE_RELEASE(Joystick);
		SAFE_RELEASE(DirectInput);
	}

	bool InputDevice::IsMouseEnabled() const
	{
		return Mouse != nullptr;
	}

	bool InputDevice::IsKeyboardEnabled() const
	{
		return Keyboard != nullptr;
	}

	bool InputDevice::IsJoystickEnabled() const
	{
		return Joystick != nullptr;
	}

	void InputDevice::Poll()
	{
		for (auto obj : CallbackObjects)
		{
			if (obj != nullptr)
			{
				obj->OnInputBegin();
			}
		}

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

		for (auto obj : CallbackObjects)
		{
			if (obj != nullptr)
			{
				obj->OnInputEnd();
			}
		}
	}

	void InputDevice::PollMouseState()
	{
		const CHAR* head = "InputDevice::PollMouseState";

		if (Mouse == nullptr)
		{
			return;
		}

		HRESULT hr = S_FALSE;

		MouseState ms;

		hr = Mouse->Poll();
		if (FAILED(hr))
		{
			hr = Mouse->Acquire();
			//LVMSG2(SLogPrefix, head, "mouse poll failed, acquire result: 0x%x(%d)", hr, hr);
			return;
		}

		hr = Mouse->GetDeviceState(sizeof(MouseState), &ms);
		if (FAILED(hr))
		{
			LVMSG2(SLogPrefix, head, "get mouse state failed: 0x%x(%d)", hr, hr);
			return;
		}

		const CHAR* down = "down";
		const CHAR* up = "up";
		//LVMSG2(SLogPrefix, head, "mouse state, axis x: %d, axis y: %d, btn 0: %s, btn 1: %s, btn 2: %s",
		//	ms.lAxisX, ms.lAxisY, ms.abButtons[0] & 0x80 ? down : up, ms.abButtons[1] & 0x80 ? down : up, ms.abButtons[2] & 0x80 ? down : up);
	}

	void InputDevice::PollKeyboardState()
	{
		const CHAR* head = "InputDevice::PollKeyboardState";

		if (Keyboard == nullptr)
		{
			return;
		}

		HRESULT hr = S_FALSE;

		hr = Keyboard->Poll();
		if (FAILED(hr))
		{
			hr = Keyboard->Acquire();
			//LVMSG2(SLogPrefix, head, "key poll failed, acquire result: 0x%x(%d)", hr, hr);
			return;
		}

		memcpy((UCHAR*)KeyStates + 256, KeyStates, 256);
		hr = Keyboard->GetDeviceState(256, KeyStates[0]);
		if (FAILED(hr))
		{
			LVMSG2(SLogPrefix, head, "get key state failed: 0x%x(%d)", hr, hr);
			return;
		}

		for (int i = 0; i < 256; ++i)
		{
			if (*((UCHAR*)KeyStates + i) != *((UCHAR*)KeyStates + i + 256))
			{
				for (auto obj : CallbackObjects)
				{
					if (obj != nullptr)
					{
						obj->OnKeyboardAction(i, *((UCHAR*)KeyStates + i) == 0 ? EInputAction::Released : EInputAction::Pressed);
					}
				}
			}
		}
	}

	void InputDevice::PollJoystickState()
	{
		const CHAR* head = "InputDevice::PollJoystickState";

		if (Joystick == nullptr)
		{
			return;
		}

		HRESULT hr = S_FALSE;

		hr = Joystick->Poll();
		if (FAILED(hr))
		{
			hr = Joystick->Acquire();
			return;
		}

		memcpy((DIJOYSTATE*)JoyStates + 1, JoyStates, sizeof(DIJOYSTATE));
		hr = Joystick->GetDeviceState(sizeof(DIJOYSTATE), &JoyStates);
		if (FAILED(hr))
		{
			LVMSG2(SLogPrefix, head, "get joystick state failed: 0x%x(%d)", hr, hr);
			return;
		}

		static const int SJoyAxisKeys_LONG[] = {
			DIJOFS_X,
			DIJOFS_Y,
			DIJOFS_Z,
			DIJOFS_RX,
			DIJOFS_RY,
			DIJOFS_RZ,
			DIJOFS_SLIDER(0),
			DIJOFS_SLIDER(1),
		};

		static const int SJoyAxisKeys_DWORD[] = {
			DIJOFS_POV(0),
			DIJOFS_POV(1),
			DIJOFS_POV(2),
			DIJOFS_POV(3),
		};

		static const int SJoyActionKeyNum = 32;

		for (auto key : SJoyAxisKeys_LONG)
		{
			LONG curr = *(LONG*)((uint8*)JoyStates + key);
			for (auto obj : CallbackObjects)
			{
				if (obj != nullptr)
				{
					obj->OnJoystickAxis(key, curr);
				}
			}
		}

		for (auto key : SJoyAxisKeys_DWORD)
		{
			DWORD curr = *(DWORD*)((uint8*)JoyStates + key);
			for (auto obj : CallbackObjects)
			{
				if (obj != nullptr)
				{
					obj->OnJoystickAxis(key, curr);
				}
			}
		}

		for (int i = 0; i < 32; ++i)
		{
			int key = DIJOFS_BUTTON(i);
			uint8 prev = *((uint8*)((DIJOYSTATE*)JoyStates + 1) + key);
			uint8 curr = *((uint8*)JoyStates + key);
			if (prev != curr)
			{
				for (auto obj : CallbackObjects)
				{
					if (obj != nullptr)
					{
						obj->OnJoystickAction(i, curr == 0 ? EInputAction::Released : EInputAction::Pressed);
					}
				}
			}
		}
	}

	void InputDevice::RegisterCallback(IInputDeviceCallback* obj)
	{
		CallbackObjects.push_back(obj);
	}
}

void SetupInputDevice(void * p)
{
	lostvr::SInputDevice.Initialize(*(HWND*)p, false, true, true);
}

void PollInputDevice(void * p)
{
	lostvr::SInputDevice.Poll();
}

void WINAPI RegisterInputCallback(IInputDeviceCallback* obj)
{
	lostvr::SInputDevice.RegisterCallback(obj);
}