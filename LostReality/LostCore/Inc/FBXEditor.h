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
	LOSTCORE_API void SetLogger(PFN_Logger logger);

	LOSTCORE_API void InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height);
	LOSTCORE_API void LoadFBX(
		const char* file,
		const char* primitiveOutput,
		const char* animationOutput,
		bool clearScene,
		bool importTexCoord,
		bool importAnimation,
		bool importVertexColor,
		bool mergeNormalTangentAll,
		bool importNormal,
		bool forceRegenerateNormal,
		bool generateNormalIfNotFound,
		bool importTangent,
		bool forceRegenerateTangent,
		bool generateTangentIfNotFound);
}
