/*
* file FBXEditor.h
*
* author luoxw
* date 2017/08/02
*
*
*/

#pragma once

namespace LostCore
{
	typedef void(WINAPI *PFN_Logger)(int32 level, const char* msg);
	extern "C" LOSTCORE_API void SetLogger(PFN_Logger logger);

	extern "C" LOSTCORE_API void InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height);
	extern "C" LOSTCORE_API void LoadFBX(const char* file, bool clearScene);
}
