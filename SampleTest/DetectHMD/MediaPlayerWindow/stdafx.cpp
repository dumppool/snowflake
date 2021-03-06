
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define EXE_INCLUDE

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

GlobalSharedData SGlobalSharedDataInst;

GlobalSharedData::GlobalSharedData() : HookCoreDllPath(nullptr)
, DependencyDirectory(nullptr)
, OpenVRDllPath(nullptr)
, LibOVRDllPath(nullptr)
, ShaderFilePath(nullptr)
, TargetWindow(NULL)
, bDrawCursor(true)
, DefaultWindow(NULL)
{}

GlobalSharedData::~GlobalSharedData()
{
	SAFE_DELETE_ARRAY(HookCoreDllPath);
	SAFE_DELETE_ARRAY(DependencyDirectory);
	SAFE_DELETE_ARRAY(OpenVRDllPath);
	SAFE_DELETE_ARRAY(LibOVRDllPath);

	if (DefaultWindow != NULL)
	{
		DestroyWindow(DefaultWindow);
	}
}

const WCHAR * GlobalSharedData::GetHookCoreDllName() const
{
#ifdef _WIN64
	return L"HookCore_x64.dll";
#else
	return L"HookCore.dll";
#endif
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
	return L"openvr_api.dll";
}

WCHAR * GlobalSharedData::GetOpenVRDllPath()
{
	const CHAR* head = "GlobalSharedData::GetOpenVRDllPath";

	if (OpenVRDllPath != nullptr)
	{
		return OpenVRDllPath;
	}

	OpenVRDllPath = new WCHAR[MAX_PATH];
	OpenVRDllPath[0] = L'\0';

	wsprintf(OpenVRDllPath, L"%s\\%s", GetDependencyDirectory(), GetOpenVRDllName());
	LVMSG(head, "open vr dll path: %ls", OpenVRDllPath);
	return OpenVRDllPath;
}

const WCHAR * GlobalSharedData::GetLibOVRDllName() const
{
#ifdef _WIN64
	return L"LibOVRRT64_1.dll";
#else
	return L"LibOVRRT32_1.dll";
#endif
}

WCHAR * GlobalSharedData::GetLibOVRDllPath()
{
	const CHAR* head = "GlobalSharedData::GetLibOVRDllPath";

	if (LibOVRDllPath != nullptr)
	{
		return LibOVRDllPath;
	}

	LibOVRDllPath = new WCHAR[MAX_PATH];
	LibOVRDllPath[0] = L'\0';

	wsprintf(LibOVRDllPath, L"%s\\%s", GetDependencyDirectory(), GetLibOVRDllName());
	LVMSG(head, "libOVR dll path: %ls", LibOVRDllPath);
	return LibOVRDllPath;
}

WCHAR * GlobalSharedData::GetShaderFilePath()
{
	if (ShaderFilePath != nullptr)
	{
		return ShaderFilePath;
	}

	ShaderFilePath = new WCHAR[MAX_PATH];
	ShaderFilePath[0] = L'\0';

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

WCHAR * GlobalSharedData::GetShaderFilePath_MediaPlayer()
{
	const CHAR* head = "GlobalSharedData::GetShaderfFilePath_MediaPlayer";

	if (ShaderFilePath != nullptr)
	{
		return ShaderFilePath;
	}

	ShaderFilePath = new WCHAR[MAX_PATH];
	ShaderFilePath[0] = L'\0';

#ifdef EXE_INCLUDE
	HMODULE handle = GetModuleHandleW(NULL);
#else
	HMODULE handle = GetModuleHandleW(GetHookCoreDllName());
	if (handle == NULL)
	{
		LVMSG(head, "not found: %ls", GetHookCoreDllName());
		return nullptr;
	}
#endif

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

	wsprintf(ShaderFilePath, L"%s\\res\\media_player.hlsl", path);
	LVMSG(head, "%ls", ShaderFilePath);
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

const WCHAR * GlobalSharedData::GetDependencyDirectoryName() const
{
#ifdef _WIN64
	return L"dep_x64";
#else
	return L"dep_x86";
#endif
}

WCHAR * GlobalSharedData::GetDependencyDirectory()
{
	const CHAR* head = "GlobalSharedData::GetDependencyDirectory";

	if (DependencyDirectory != nullptr)
	{
		return DependencyDirectory;
	}

	DependencyDirectory = new WCHAR[MAX_PATH];
	DependencyDirectory[0] = '\0';

	HMODULE handle = GetModuleHandleW(GetHookCoreDllName());
	if (handle == NULL)
	{
		LVMSG(head, "not found: %ls", GetHookCoreDllName());
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

	wsprintf(DependencyDirectory, L"%s\\%s", path, GetDependencyDirectoryName());
	LVMSG(head, "dependency directory: %ls", DependencyDirectory);
	return DependencyDirectory;
}

static LRESULT CALLBACK WindowProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

void GlobalSharedData::SetDefaultWindow(HWND hwnd)
{
	DefaultWindow = hwnd;
}

HWND GlobalSharedData::GetDefaultWindow()
{
	if (DefaultWindow == NULL)
	{
		WNDCLASSEXW wc;
		memset(&wc, 0, sizeof(wc));
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpszClassName = L"HookApp";
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.cbWndExtra = 0;
		wc.hInstance = HINST_THISCOMPONENT;
		RegisterClassExW(&wc);

		// adjust the window size and show at InitDevice time
		DefaultWindow = CreateWindowW(wc.lpszClassName, L"DefaultWIN", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, 0, 0, HINST_THISCOMPONENT, 0);

		if (DefaultWindow)
		{
			ShowWindow(DefaultWindow, 0);
		}
	}

	return DefaultWindow;
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
