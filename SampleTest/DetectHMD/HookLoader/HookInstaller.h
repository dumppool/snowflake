/*
 * file HookInstaller.h
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <assert.h>

#include <TlHelp32.h>
#include <Psapi.h>

using namespace std;

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
  do                         \
  {                          \
    if(p)                    \
    {                        \
      delete[](p);           \
      (p) = NULL;            \
    }                        \
  } while((void)0, 0)
#endif

#define LVLog printf("\n");printf
#define LVLogDebug printf("\n");printf
#define LVLogWarn printf("\n");printf
#define LVLogErr printf("\n");printf
#define LVEraseMem(mem, sz) {::memset(mem, 0, sz);}
#define LVEraseEl(obj) {::memset(&obj, 0, sizeof(obj));}

#ifndef LVMSG
#define LVMSG
#endif

inline wstring UTF8ToWide(const string &utf8)
{
	if (utf8.length() == 0) {
		return wstring();
	}

	// compute the length of the buffer we'll need
	int charcount = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);

	if (charcount == 0) {
		return wstring();
	}

	// convert
	wchar_t* buf = new wchar_t[charcount];
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buf, charcount);
	wstring result(buf);
	delete[] buf;
	return result;
}

// static
inline string WideToUTF8(const wstring &wide) {
	if (wide.length() == 0) {
		return string();
	}

	// compute the length of the buffer we'll need
	int charcount = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1,
		NULL, 0, NULL, NULL);
	if (charcount == 0) {
		return string();
	}

	// convert
	char *buf = new char[charcount];
	WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, buf, charcount,
		NULL, NULL);

	string result(buf);
	delete[] buf;
	return result;
}

void InjectDLL(HANDLE hProcess, wstring libName);
uintptr_t FindRemoteDLL(DWORD pid, wstring libName);
void InjectFunctionCall(HANDLE hProcess, uintptr_t renderdoc_remote, const char *funcName, void *data, const size_t dataLen);
PROCESS_INFORMATION RunProcess(const char *app, const char *workingDir, const char *cmdLine);
uint32_t InjectIntoProcess(uint32_t pid, const char *logfile, bool waitForExit);
uint32_t LaunchAndInjectIntoProcess(const char *app, const char *workingDir, const char *cmdLine, const char *logfile, bool waitForExit, DWORD& pid);
void InjectFinished(DWORD pid);

static bool SUseWin32 = false;

class GlobalSharedData
{
protected:
	const WCHAR*	HookCoreDllName;
	WCHAR*			HookCoreDllPath;

	const WCHAR*	OpenVRDllName;
	WCHAR*			OpenVRDllPath;

	WCHAR*			ShaderFilePath;

public:

	GlobalSharedData() : 
#ifdef _WIN64
		HookCoreDllName(L"HookCore_x64.dll")
#else
		HookCoreDllName(L"HookCore.dll")
#endif
		, HookCoreDllPath(nullptr)
		, OpenVRDllName(L"openvr_api.dll")
		, OpenVRDllPath(nullptr)
		, ShaderFilePath(nullptr)
	{}

	~GlobalSharedData()
	{
		delete[] HookCoreDllPath;
		HookCoreDllPath = nullptr;
	}

	const WCHAR* GetHookCoreDllName() const
	{
		return HookCoreDllName;
	}

	WCHAR* GetHookCoreDllPath()
	{
		if (HookCoreDllPath != nullptr)
		{
			return HookCoreDllPath;
		}

		HookCoreDllPath = new WCHAR[MAX_PATH];
		HookCoreDllPath[0] = '\0';

		WCHAR buf[MAX_PATH];
		GetModuleFileName(NULL, &buf[0], MAX_PATH);

		int lastslash = MAX_PATH;
		while ((--lastslash) >= 0)
		{
			if (buf[lastslash] == '\\')
			{
				buf[lastslash+1] = '\0';
				break;
			}
		}

		errno_t err = 0;
		err = wcscat_s(&HookCoreDllPath[0], MAX_PATH, buf);
		if (err != 0)
		{
			LVLogErr("GlobalSharedData::GetHookCoreDllPath\twcscat_s %ls failed with error: %d\n", buf, err);
			return nullptr;
		}

		err = wcscat_s(&HookCoreDllPath[0], MAX_PATH, GetHookCoreDllName());
		if (err != 0)
		{
			LVLogErr("GlobalSharedData::GetHookCoreDllPath\twcscat_s %ls failed with error: %d\n", GetHookCoreDllName(), err);
			return nullptr;
		}

		LVLog("GlobalSharedData::GetHookCoreDllPath\thook core dir: %ls\n", HookCoreDllPath);
		return HookCoreDllPath;
	}

	const WCHAR* GetOpenVRDllName() const
	{
		return OpenVRDllName;
	}

	WCHAR* GetOpenVRDllPath()
	{
		if (OpenVRDllPath != nullptr)
		{
			return OpenVRDllPath;
		}

		OpenVRDllPath = new WCHAR[MAX_PATH];
		OpenVRDllPath[0] = '\0';

		HMODULE handle = GetModuleHandleW(GetHookCoreDllName());
		if (handle == NULL)
		{
			LVMSG("GlobalSharedData::GetOpenVRDllPath", "not found: %ls", GetHookCoreDllName());
			return nullptr;
		}

		WCHAR path[MAX_PATH];
		GetModuleFileName(handle, &path[0], MAX_PATH - 1);

		int lastslash = MAX_PATH;
		while ((--lastslash) >= 0)
		{
			if (path[lastslash] == '\\')
			{
				path[lastslash + 1] = '\0';
				break;
			}
		}

		errno_t err = 0;
		err = wcscat_s(&OpenVRDllPath[0], MAX_PATH - 1, path);
		if (err != 0)
		{
			LVMSG("GlobalSharedData::GetOpenVRDllPath", "wcscat_s %ls failed with error: %d", path, err);
			return nullptr;
		}

#ifdef _WIN64
		const WCHAR* dep = L"dep_x64";
#else
		const WCHAR* dep = L"dep_x86";
#endif
		err = wcscat_s(&OpenVRDllPath[0], MAX_PATH - 1, dep);
		if (err != 0)
		{
			LVMSG("GlobalSharedData::GetOpenVRDllPath", "wcscat_s %ls failed with error: %d", dep, err);
			return nullptr;
		}

		err = wcscat_s(&OpenVRDllPath[0], MAX_PATH - 1, GetOpenVRDllName());
		if (err != 0)
		{
			LVMSG("GlobalSharedData::GetOpenVRDllPath", "wcscat_s %ls failed with error: %d", GetOpenVRDllName(), err);
			return nullptr;
		}

		LVMSG("GlobalSharedData::GetOpenVRDllPath", "%ls dir: %ls", GetOpenVRDllName(), OpenVRDllPath);
		return OpenVRDllPath;
	}

	WCHAR* GetShaderFilePath()
	{
		if (ShaderFilePath != nullptr)
		{
			return ShaderFilePath;
		}

		ShaderFilePath = new WCHAR[MAX_PATH];
		ShaderFilePath[0] = '\0';

		HMODULE handle = GetModuleHandleW(GetHookCoreDllName());
		if (handle == NULL)
		{
			LVMSG("GlobalSharedData::GetShaderFilePath", "not found: %ls", GetHookCoreDllName());
			return nullptr;
		}

		WCHAR path[MAX_PATH];
		GetModuleFileName(handle, &path[0], MAX_PATH - 1);

		int lastslash = MAX_PATH;
		while ((--lastslash) >= 0)
		{
			if (path[lastslash] == '\\')
			{
				path[lastslash] = '\0';
				break;
			}
		}

		wsprintf(ShaderFilePath, L"%s\\res\\projector.hlsl", path);
		LVMSG("GlobalShadredData::GetShaderFilePath", "%ls", ShaderFilePath);
		return ShaderFilePath;
	}
};

static GlobalSharedData SGlobalSharedDataInst;