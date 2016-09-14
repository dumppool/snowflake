#pragma once

#include <Windows.h>

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* p);
extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* p);