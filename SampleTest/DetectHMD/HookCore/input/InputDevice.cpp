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

	typedef void (_cdecl *PFN_KeyboardCallback)(bool bReleased);

	class InputDevice
	{
	protected:
		LPDIRECTINPUT8          DirectInput;   

		LPDIRECTINPUTDEVICE8    Mouse;
		LPDIRECTINPUTDEVICE8    Keyboard;
		LPDIRECTINPUTDEVICE8    Joystick;

		UCHAR KeyStates[2][256];

		std::map<int, std::vector<void*>> KeyboardCallback;

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

		void RegisterKeyboardCallback(int dikey, void* func);
	};

	static InputDevice SInputDevice;

	InputDevice::InputDevice() : DirectInput(nullptr)
		, Mouse(nullptr)
		, Keyboard(nullptr)
		, Joystick(nullptr)
	{
		memset(KeyStates, 0, sizeof(KeyStates));
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

		if (bEnableJoystick)
		{
			bContinue = true;
			VERIFY_HRESULT2(DirectInput->CreateDevice(GUID_Joystick, &Joystick, NULL),
			{ bContinue = false; }, SLogPrefix, head, "create joystick device failed: 0x%x(%d)", hr, hr);
		}

		LVMSG2(SLogPrefix, head, "initialize InputDevice successfully");
		LVMSG2(SLogPrefix, head, "err code, DIERR_ACQUIRED: 0x%x, DIERR_INPUTLOST: 0x%x, DIERR_OLDDIRECTINPUTVERSION: 0x%x, DIERR_BETADIRECTINPUTVERSION: 0x%x",
			DIERR_ACQUIRED, DIERR_INPUTLOST, DIERR_OLDDIRECTINPUTVERSION, DIERR_BETADIRECTINPUTVERSION);
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
				if (*((UCHAR*)KeyStates + i) == 0)
				{
					for (auto func : KeyboardCallback[i])
					{
						if ((PFN_KeyboardCallback)func != nullptr)
						{
							((PFN_KeyboardCallback)func)(true);
						}
					}
				}
				else
				{
					for (auto func : KeyboardCallback[i])
					{
						if ((PFN_KeyboardCallback)func != nullptr)
						{
							((PFN_KeyboardCallback)func)(false);
						}
					}
				}
			}
		}
	}

	void InputDevice::PollJoystickState()
	{
	}

	void InputDevice::RegisterKeyboardCallback(int dikey, void * func)
	{
		KeyboardCallback[dikey].push_back(func);
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

void RegisterKeyboardCallback(int dikey, void * func)
{
	lostvr::SInputDevice.RegisterKeyboardCallback(dikey, func);
}
