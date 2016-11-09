#pragma once

extern _declspec(dllexport) void _cdecl SetupInputDevice(void * p);
extern _declspec(dllexport) void _cdecl PollInputDevice(void* p);
extern _declspec(dllexport) void _cdecl RegisterKeyboardCallback(int dikey, void* func);