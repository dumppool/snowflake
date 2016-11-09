#pragma once

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

extern _declspec(dllexport) void _cdecl SetupInputDevice(void * p);
extern _declspec(dllexport) void _cdecl PollInputDevice(void* p);
extern _declspec(dllexport) void _cdecl RegisterKeyboardCallback(int dikey, void* func);