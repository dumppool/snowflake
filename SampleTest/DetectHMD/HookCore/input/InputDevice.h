/*
 * file InputDevice.h
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

extern _declspec(dllexport) void _cdecl SetupInputDevice(void * p);
extern _declspec(dllexport) void _cdecl PollInputDevice(void* p);

enum EInputAction : uint8
{
	UnDefined,
	Pressed,
	Released,
};

class IInputDeviceCallback
{
public:
	virtual void CALLBACK OnKeyboardAction(int key, EInputAction value) = 0;
	virtual void CALLBACK OnJoystickAction(int key, EInputAction value) = 0;
	virtual void CALLBACK OnJoystickAxis(int key, long value) = 0;
	virtual void CALLBACK OnInputBegin() = 0;
	virtual void CALLBACK OnInputEnd() = 0;
};

extern _declspec(dllexport) void WINAPI RegisterInputCallback(IInputDeviceCallback* obj);