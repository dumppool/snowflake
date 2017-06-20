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
using namespace std;

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

//static const int SCameraBack = 1;
//static const int SCameraFront = 2;
//static const int SSwitchMouseMove = 3;
//static const int SSwitchMouseButtonHold = 4;
//static const int SSwitchJoystick = 5;

#define SCameraBack				string("lenspull")
#define SCameraFront			string("closerview")
#define SSwitchMouseMove		string("ctrlview")
#define SSwitchMouseButtonHold	string("ctrlmouse")
#define SSwitchJoystick			string("openhandle")

#define SJoystickButtonA		string("A")
#define SJoystickButtonB		string("B")
#define SJoystickButtonX		string("X")
#define SJoystickButtonY		string("Y")
#define SJoystickButtonLB		string("LB")
#define SJoystickButtonRB		string("RB")
#define SJoystickButtonLT		string("LT")
#define SJoystickButtonRT		string("RT")

// only accept lower-case character
static int CharToDIK(char c)
{
	return MapVirtualKeyA(VkKeyScanA(c), 0);
}

// only accept lower-case key
// only return true if is not a modifier
static bool CustomCharToDIK(const string& key, int& output)
{
	static map<string, int> SDIKMap;
	if (SDIKMap.empty())
	{
		SDIKMap["pagedown"] = (DIK_PGDN);
		SDIKMap["pageup"] = (DIK_PGUP);
		SDIKMap["home"] = (DIK_HOME);
		SDIKMap["end"] = (DIK_END);
		SDIKMap["left"] = (DIK_LEFT);
		SDIKMap["right"] = (DIK_RIGHT);
		SDIKMap["up"] = (DIK_UP);
		SDIKMap["down"] = (DIK_DOWN);
	}

	if (SDIKMap.find(key) != SDIKMap.end())
	{
		output = SDIKMap[key];
		return true;
	}
	else
	{
		return false;
	}
}

// only return true if is a modifier
static bool ModifierToDIK(const string& key, vector<int>& output)
{
	static map<string, vector<int>> SDIKMap;
	if (SDIKMap.empty())
	{
		SDIKMap["ctrl"].push_back(DIK_LCONTROL);
		SDIKMap["ctrl"].push_back(DIK_RCONTROL);
		SDIKMap["shift"].push_back(DIK_LSHIFT);
		SDIKMap["shift"].push_back(DIK_RSHIFT);
		SDIKMap["alt"].push_back(DIK_LALT);
		SDIKMap["alt"].push_back(DIK_RALT);
	}

	if (SDIKMap.find(key) != SDIKMap.end())
	{
		output = SDIKMap[key];
		return true;
	}
	else
	{
		return false;
	}
}

static bool CustomCharToVK(const string& key, int& output)
{
	static map<string, int> SVKMap;
	if (SVKMap.empty())
	{
		SVKMap["pagedown"] = (VK_NEXT);
		SVKMap["pageup"] = (VK_PRIOR);
		SVKMap["home"] = (VK_HOME);
		SVKMap["end"] = (VK_END);
		SVKMap["left"] = (VK_LEFT);
		SVKMap["right"] = (VK_RIGHT);
		SVKMap["up"] = (VK_UP);
		SVKMap["down"] = (VK_DOWN);
	}

	if (SVKMap.find(key) != SVKMap.end())
	{
		output = SVKMap[key];
		return true;
	}
	else
	{
		return false;
	}
}

class FInputEventHandler : IInputDeviceCallback
{
	uint8			KeyStates[256];
	EInputAction	KeyEvents[256];
	bool			bHasKeyInput;

	int				Z0;
	int				Z1;

	DIJOYSTATE		JoyStates;
	EInputAction	ButtonEvents[32];
	bool			ButtonPressed[12];
	bool			bHasButtonInput;

	bool			bIsCounting;

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

private:
	struct FKeysAction
	{
		string ID;
		int Key;
		vector<vector<int>> Modifiers;
		function<void()> Action;

		FKeysAction(const string& id, int key, const vector<vector<int>>& modifiers)
			: ID(id)
			, Key(key)
			, Modifiers(modifiers)
			, Action(nullptr)
		{
			static map<string, function<void()>> SActionMap;
			if (SActionMap.empty())
			{
				SActionMap[SCameraBack] = []() {
					auto dev = LostVR::Get()->GetDevice();
					if (dev != nullptr)
					{
						dev->AddMovement(EMovement::CameraBack);
					}};
				SActionMap[SCameraFront] = []() {
					auto dev = LostVR::Get()->GetDevice();
					if (dev != nullptr)
					{
						dev->AddMovement(EMovement::CameraFront);
					}};
				SActionMap[SSwitchMouseButtonHold] = []() {
					SGlobalSharedDataInst.SetBHoldWhenMoving(!SGlobalSharedDataInst.GetBHoldWhenMoving()); };
				SActionMap[SSwitchMouseMove] = []() {
					SGlobalSharedDataInst.SetBFakeMouseMove(!SGlobalSharedDataInst.GetBFakeMouseMove()); };
				SActionMap[SSwitchJoystick] = []() {
					SGlobalSharedDataInst.SetBEnableJoystick(!SGlobalSharedDataInst.GetBEnableJoystick()); };
			}

			if (SActionMap.find(ID) == SActionMap.end())
			{
				LVERROR("FKeysAction", "invalid id: %s", id.c_str());
			}
			else
			{
				Action = SActionMap[ID];
			}
		}

		bool IsValid() const
		{
			return Key > 0 && !ID.empty() && Action != nullptr;
		}

	private:

		FKeysAction()
			: ID("")
			, Key(-1)
		{
			Modifiers.clear();
		}
	};

	struct FJoystickButtonAction
	{
		string ID;
		int Button;
		int KeyCode;

		FJoystickButtonAction(const string& id, int keyCode)
			: ID(id)
			, Button(-1)
			, KeyCode(keyCode)
		{
			static map<string, int> SButtonMap;
			if (SButtonMap.empty())
			{
				SButtonMap[SJoystickButtonA] = 0;
				SButtonMap[SJoystickButtonB] = 1;
				SButtonMap[SJoystickButtonX] = 2;
				SButtonMap[SJoystickButtonY] = 3;
				SButtonMap[SJoystickButtonLB] = 4;
				SButtonMap[SJoystickButtonRB] = 5;
			}

			if (SButtonMap.find(id) != SButtonMap.end())
			{
				Button = SButtonMap[id];
			}
			else
			{
				LVERROR("FJoystickButtonAction", "invalid id: %s", id.c_str());
			}
		}

		bool IsValid() const
		{
			return !ID.empty() && Button >= 0 && KeyCode >= 0;
		}

	private:
		FJoystickButtonAction()
			: ID("")
			, Button(-1)
			, KeyCode(-1)
		{
		}
	};

	vector<FKeysAction> GeneralActions;
	vector<FJoystickButtonAction> JoystickButtonAction;

	void SetupGeneralAction(const FKeysAction& inAction)
	{
		bool bFound = false;
		for (auto& action : GeneralActions)
		{
			if (action.ID == inAction.ID)
			{
				action = inAction;
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			GeneralActions.push_back(inAction);
		}
	}

	void UpdateGeneralAction()
	{
		for (auto& action : GeneralActions)
		{
			if (!action.IsValid())
			{
				continue;
			}

			if (ConsumeKeyAction(action.Key) == EInputAction::Released)
			{
				bool bMissed = false;
				for (auto m : action.Modifiers)
				{
					bool bPressed = false;
					for (auto n : m)
					{
						if (IsKeyDown(n))
						{
							bPressed = true;
							break;
						}
					}

					if (!bPressed)
					{
						bMissed = true;
						break;
					}
				}

				if (!bMissed)
				{
					action.Action();
				}
			}
		}
	}

	void SetupJoystickButtonAction(const FJoystickButtonAction& inAction)
	{
		bool bFound = false;
		for (auto& action : JoystickButtonAction)
		{
			if (action.ID == inAction.ID)
			{
				action = inAction;
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			JoystickButtonAction.push_back(inAction);
		}
	}

	void UpdateJoystickButtonAction()
	{
		for (auto& action : JoystickButtonAction)
		{
			if (!action.IsValid())
			{
				continue;
			}

			EInputAction state;

			state = ConsumeButtonAction(action.Button);
			if (state == EInputAction::Pressed)
			{
				FakeKeyPress(action.KeyCode);
			}
			else if (state == EInputAction::Released)
			{
				FakeKeyRelease(action.KeyCode);
			}
		}
	}

	void ParseInputSettings_General(const FJson& settings)
	{
		static vector<string> SGeneralIDs;
		if (SGeneralIDs.empty())
		{
			SGeneralIDs.push_back(SCameraBack);
			SGeneralIDs.push_back(SCameraFront);
			SGeneralIDs.push_back(SSwitchMouseMove);
			SGeneralIDs.push_back(SSwitchMouseButtonHold);
			SGeneralIDs.push_back(SSwitchJoystick);
		}

		for (const auto& id : SGeneralIDs)
		{
			if (settings.find(id) != settings.end())
			{
				string content = settings[id];
				transform(content.begin(), content.end(), content.begin(), LowerChar);

				content = SubChar(content, ' ');
				auto keys = SplitChar(content, '+');
				int mainKey;
				vector<vector<int>> modKeys;
				for (auto& k : keys)
				{
					vector<int> modKey;
					if (ModifierToDIK(k, modKey))
					{
						modKeys.push_back(modKey);
					}
					else if (CustomCharToDIK(k, mainKey))
					{
					}
					else if (k.size() > 0)
					{
						mainKey = CharToDIK(k[0]);
					}
				}

				SetupGeneralAction(FKeysAction(id, mainKey, modKeys));
			}
		}
	}

	void ParseInputSettings_Joystick(const FJson& settings)
	{
		static vector<string> SJoystickButtons;
		if (SJoystickButtons.empty())
		{
			SJoystickButtons.push_back(SJoystickButtonA);
			SJoystickButtons.push_back(SJoystickButtonB);
			SJoystickButtons.push_back(SJoystickButtonX);
			SJoystickButtons.push_back(SJoystickButtonY);
			SJoystickButtons.push_back(SJoystickButtonLB);
			SJoystickButtons.push_back(SJoystickButtonRB);
		}

		for (const auto& id : SJoystickButtons)
		{
			if (settings.find(id) != settings.end())
			{
				string content = settings[id];
				transform(content.begin(), content.end(), content.begin(), UpperChar);

				content = SubChar(content, ' ');

				int key;
				if (!CustomCharToVK(content, key) && content.size() > 0)
				{
					key = content[0];
				}

				SetupJoystickButtonAction(FJoystickButtonAction(id, key));
			}
		}

		if (settings.find(SJoystickButtonLT) != settings.end())
		{
			string content = settings[SJoystickButtonLT];
			transform(content.begin(), content.end(), content.begin(), UpperChar);
			content = SubChar(content, ' ');
			if (content.size() > 0)
			{
				Z0 = content[0];
			}
		}

		if (settings.find(SJoystickButtonRT) != settings.end())
		{
			string content = settings[SJoystickButtonRT];
			transform(content.begin(), content.end(), content.begin(), UpperChar);
			content = SubChar(content, ' ');
			if (content.size() > 0)
			{
				Z1 = content[0];
			}
		}
	}

	void ParseInputSettings(const FJson& settings)
	{
		assert(settings.find("common") != settings.end() && "can not find section: common");
		assert(settings.find("key") != settings.end() && "can not find section: key");

		ParseInputSettings_General(settings["common"]);
		ParseInputSettings_Joystick(settings["key"]);
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
			UpdateGeneralAction();
		}

		if (SGlobalSharedDataInst.GetBEnableJoystick())
		{
			if (bHasButtonInput)
			{
				UpdateJoystickButtonAction();
			}

			const uint32 THRESHOLD_SMALL = 1 << 12;
			const uint32 THRESHOLD_LARGE = (1 << 15) | (1 << 14);
			if (JoyStates.lX < THRESHOLD_SMALL)
			{
				if (!ButtonPressed[0])
				{
					ButtonPressed[0] = true;
					FakeKeyPress('A');
				}
			}
			else
			{
				if (ButtonPressed[0])
				{
					ButtonPressed[0] = false;
					FakeKeyRelease('A');
				}
			}

			if (JoyStates.lX > THRESHOLD_LARGE)
			{
				if (!ButtonPressed[1])
				{
					ButtonPressed[1] = true;
					FakeKeyPress('D');
				}
			}
			else
			{
				if (ButtonPressed[1])
				{
					ButtonPressed[1] = false;
					FakeKeyRelease('D');
				}
			}

			if (JoyStates.lY < THRESHOLD_SMALL)
			{
				if (!ButtonPressed[2])
				{
					ButtonPressed[2] = true;
					FakeKeyPress('W');
				}
			}
			else
			{
				if (ButtonPressed[2])
				{
					ButtonPressed[2] = false;
					FakeKeyRelease('W');
				}
			}

			if (JoyStates.lY > THRESHOLD_LARGE)
			{
				if (!ButtonPressed[3])
				{
					ButtonPressed[3] = true;
					FakeKeyPress('S');
				}
			}
			else
			{
				if (ButtonPressed[3])
				{
					ButtonPressed[3] = false;
					FakeKeyRelease('S');
				}
			}

			if (JoyStates.lRx < THRESHOLD_SMALL)
			{
				if (!ButtonPressed[4])
				{
					ButtonPressed[4] = true;
					FakeKeyPress('7');
				}
			}
			else
			{
				if (ButtonPressed[4])
				{
					ButtonPressed[4] = false;
					FakeKeyRelease('7');
				}
			}

			if (JoyStates.lRx > THRESHOLD_LARGE)
			{
				if (!ButtonPressed[5])
				{
					ButtonPressed[5] = true;
					FakeKeyPress('8');
				}
			}
			else
			{
				if (ButtonPressed[5])
				{
					ButtonPressed[5] = false;
					FakeKeyRelease('8');
				}
			}

			if (JoyStates.lRy < THRESHOLD_SMALL)
			{
				if (!ButtonPressed[6])
				{
					ButtonPressed[6] = true;
					FakeKeyPress('9');
				}
			}
			else
			{
				if (ButtonPressed[6])
				{
					ButtonPressed[6] = false;
					FakeKeyRelease('9');
				}
			}

			if (JoyStates.lRy > THRESHOLD_LARGE)
			{
				if (!ButtonPressed[7])
				{
					ButtonPressed[7] = true;
					FakeKeyPress('0');
				}
			}
			else
			{
				if (ButtonPressed[7])
				{
					ButtonPressed[7] = false;
					FakeKeyRelease('0');
				}
			}

			int32 pov = JoyStates.rgdwPOV[0];
			if (pov >= 31500 || (pov >= 0 && 4500 >= pov))
			{
				if (!ButtonPressed[8])
				{
					ButtonPressed[8] = true;
					FakeKeyPress(VK_SPACE);
				}
			}
			else
			{
				if (ButtonPressed[8])
				{
					ButtonPressed[8] = false;
					FakeKeyRelease(VK_SPACE);
				}
			}

			if (pov >= 13500 && 22500 >= pov)
			{
				if (!ButtonPressed[9])
				{
					ButtonPressed[9] = true;
					FakeKeyPress(VK_TAB);
				}
			}
			else
			{
				if (ButtonPressed[9])
				{
					ButtonPressed[9] = false;
					FakeKeyRelease(VK_TAB);
				}
			}

			if (JoyStates.lZ < THRESHOLD_SMALL)
			{
				if (!ButtonPressed[10])
				{
					ButtonPressed[10] = true;
					FakeKeyPress(Z1);
				}
			}
			else
			{
				if (ButtonPressed[10])
				{
					ButtonPressed[10] = false;
					FakeKeyRelease(Z1);
				}
			}

			if (JoyStates.lZ > THRESHOLD_LARGE)
			{
				if (!ButtonPressed[11])
				{
					ButtonPressed[11] = true;
					FakeKeyPress(Z0);
				}
			}
			else
			{
				if (ButtonPressed[11])
				{
					ButtonPressed[11] = false;
					FakeKeyRelease(Z0);
				}
			}
		}

		//OutputJoystickInfo();
	}

	void Register()
	{
		const char* head = "FInputEventHandler::Register";

		bool bSet = false;
		int nargs = 0;
		auto argv = CommandLineToArgvW(GetCommandLineW(), &nargs);
		for (int i = 0; i < nargs; ++i)
		{
			auto arg = WideToUTF8(argv[i]);
			size_t pos = arg.find("-gamekeyset=");
			if (pos != string::npos)
			{
				string filePath(arg.begin() + pos + 12, arg.end());
				//filePath = "C:\\Users\\Administrator\\AppData\\Local\\Temp\\gamekeyset2.tmp";

				ifstream cmdFile;
				cmdFile.open(filePath);
				if (cmdFile.fail())
				{
					char errstr[128];
					strerror_s(errstr, errno);
					LVERROR(head, "open path file(%s) failed: %s", filePath.c_str(), errstr);
				}
				else
				{
					FJson settings;
					cmdFile >> settings;

					LVMSG(head, "settings(%s)", filePath.c_str());

					cmdFile.close();
					ParseInputSettings(settings);
				}

				bSet = true;
				break;
			}
		}

		if (!bSet)
		{
			SetupGeneralAction(FKeysAction(SCameraBack, DIK_S, { { DIK_LCONTROL, DIK_RCONTROL } }));
			SetupGeneralAction(FKeysAction(SCameraFront, DIK_W, { { DIK_LCONTROL, DIK_RCONTROL } }));
			SetupGeneralAction(FKeysAction(SSwitchMouseButtonHold, DIK_N, { { DIK_LCONTROL, DIK_RCONTROL } }));
			SetupGeneralAction(FKeysAction(SSwitchMouseMove, DIK_M, { { DIK_LCONTROL, DIK_RCONTROL } }));
			SetupGeneralAction(FKeysAction(SSwitchJoystick, DIK_J, { { DIK_LCONTROL, DIK_RCONTROL } }));

			SetupJoystickButtonAction(FJoystickButtonAction(SJoystickButtonA, '1'));
			SetupJoystickButtonAction(FJoystickButtonAction(SJoystickButtonB, '2'));
			SetupJoystickButtonAction(FJoystickButtonAction(SJoystickButtonX, '3'));
			SetupJoystickButtonAction(FJoystickButtonAction(SJoystickButtonY, '4'));
			SetupJoystickButtonAction(FJoystickButtonAction(SJoystickButtonLB, '5'));
			SetupJoystickButtonAction(FJoystickButtonAction(SJoystickButtonRB, '6'));

			Z0 = '7';
			Z1 = '8';
		}

		memset(KeyStates, 0, sizeof(KeyStates));
		memset(KeyEvents, 0, sizeof(KeyEvents));
		memset(&JoyStates, 0, sizeof(JoyStates));
		memset(ButtonPressed, 0, sizeof(ButtonPressed));
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
