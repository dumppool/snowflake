// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "HookCore.h"
#include <stdio.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	DWORD pid = ::GetCurrentProcessId();
	char pidstr[1024];
	sprintf_s(pidstr, "process id: %lu.", pid);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		//InstallHook();
		::MessageBoxA(NULL, pidstr, "HookCore DLL Attach", 0);
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//UninstallHook();
		::MessageBoxA(NULL, pidstr, "HookCore DLL Detach", 0);
		break;
	}
	return TRUE;
}

