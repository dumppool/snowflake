/*
 * file HookCore.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"
#include "renderers/RouteAPIs.h"

using namespace lostvr;

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result)
{
	RouteModule::Get()->InstallRouters();
	if (Result != nullptr)
	{
		*Result = 111;
	}

	return;
}

extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* p)
{
	RouteModule::Get()->UninstallRouters();
	return;
}

#ifdef WITH_VRONLINE

extern "C" _declspec(dllexport) void _cdecl NativeInjectionEntryPointMhook(int* Result)
{
	RouteModule::Get()->InstallRouters();
}

#include "easyhook/Public/easyhook.h"
extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	RouteModule::Get()->UninstallRouters();
}

#endif