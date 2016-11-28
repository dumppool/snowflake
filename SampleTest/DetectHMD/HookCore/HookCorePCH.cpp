/*
 * file HookCorePCH.cpp
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#include "HookCorePCH.h"

GlobalSharedData SGlobalSharedDataInst;

 GlobalSharedData::~GlobalSharedData()
 {
	 delete[] HookCoreDllPath;
	 HookCoreDllPath = nullptr;
 }

 const WCHAR * GlobalSharedData::GetHookCoreDllName() const
 {
	 return HookCoreDllName;
 }

 WCHAR * GlobalSharedData::GetHookCoreDllPath()
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
			 buf[lastslash + 1] = '\0';
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

 const WCHAR * GlobalSharedData::GetOpenVRDllName() const
 {
	 return OpenVRDllName;
 }

 WCHAR * GlobalSharedData::GetOpenVRDllPath()
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
	 const WCHAR* dep = L"dep_x64\\";
#else
	 const WCHAR* dep = L"dep_x86\\";
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

 WCHAR * GlobalSharedData::GetShaderFilePath()
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

 HWND GlobalSharedData::GetTargetWindow() const
 {
	 return TargetWindow;
 }

 void GlobalSharedData::SetTargetWindow(HWND wnd)
 {
	 TargetWindow = wnd;
 }

 bool GlobalSharedData::GetBDrawCursor() const
 {
	 return bDrawCursor;
 }

 void GlobalSharedData::SetBDrawCursor(bool bEnable)
 {
	 bDrawCursor = bEnable;
 }

 ScopedHighFrequencyCounter::ScopedHighFrequencyCounter(const CHAR * head) : MsgHead(head), Counter()
 {
	 Counter.Start();
 }

 ScopedHighFrequencyCounter::~ScopedHighFrequencyCounter()
 {
	 double duration = Counter.Stop();
	 {
#if 0
		 LVMSG("ScopedHighFrequencyCounter", "%s\t\t%fms", MsgHead ? MsgHead : "<null>", duration);
#endif
	 }
 }

 HighFrequencyCounter::HighFrequencyCounter()
 {
	 QueryPerformanceFrequency(&TicksPerSec);
 }

 void HighFrequencyCounter::Start()
 {
	 QueryPerformanceCounter(&Timestamp);
 }

 double HighFrequencyCounter::Stop()
 {
	 LARGE_INTEGER end;
	 QueryPerformanceCounter(&end);
	 return (double)((end.QuadPart - Timestamp.QuadPart) * 1000.0 / TicksPerSec.QuadPart);
 }
