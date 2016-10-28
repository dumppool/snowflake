#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//#pragma warning(disable : 4996)
#include <assert.h>
#include <vector>
#include <string>
#include <iostream>

//#define INCLUDE_DIRECTXTK
#ifdef INCLUDE_DIRECTXTK
#include "../DirectXTK/Inc/ScreenGrab.h"
#pragma comment(lib, "../DirectXTK/Bin/Desktop_2015/Win32/Debug/DirectXTK.lib")
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
  do                    \
  {                     \
    if(p)               \
    {                   \
      (p)->Release();   \
      (p) = NULL;       \
    }                   \
  } while((void)0, 0)
#define SAFE_DELETE(p)  \
  do                    \
  {                     \
    if(p)               \
    {                   \
      delete (p);		\
      (p) = nullptr;    \
    }                   \
  } while((void)0, 0)
#define SAFE_RELEASE_NOCLEAR(p) \
  do                            \
  {                             \
    if(p)                       \
    {                           \
      (p)->Release();           \
    }                           \
  } while((void)0, 0)
#endif

#define LVMSG(Cap, ...) {\
log_cap_cnt(Cap, __VA_ARGS__);}

#include <time.h>
#include <fstream>
using namespace std;
inline static void log_cap_cnt(const char* cap, const char* fmt, ...)
{
	char msg[1024];

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, 1023, fmt, args);
	//msg[sz] = '\0';
	va_end(args);

	time_t t = ::time(0);
	tm curr;
	::localtime_s(&curr, &t);
	char cdate[1024];
	snprintf(cdate, 1023, "%d/%d/%d %d:%d:%d", 1900 + curr.tm_year, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);

	char* envdir = nullptr;
	size_t sz = 0;
	if (!_dupenv_s(&envdir, &sz, "APPDATA") == 0 || envdir == nullptr)
	{
		::MessageBoxA(0, "", "failed to get environment path", 0);
		return;
	}

	DWORD pid = ::GetCurrentProcessId();
	char filepath[1024];
	snprintf(filepath, 1023, "%s\\%s-%d.log", envdir, "HookCore", pid);

	free(envdir);
	envdir = nullptr;

	ofstream logfile(filepath, ios::app | ios::out);
	if (!logfile)
	{
		::MessageBoxA(0, filepath, "failed to open log file", 0);
		return;
	}
	//else
	//{
	//	::MessageBoxA(0, filepath, "open log file", 0);
	//	return;
	//}

	logfile << cdate << "\t";
	logfile << "PID: " << pid << "\t";
	logfile << cap << ": " << string(msg).c_str() << endl;
	logfile.flush();
}

#define LVASSERT(Condition, Cap, ...) {\
	if (!(Condition))\
	{\
		LVMSG(Cap, __VA_ARGS__);\
		LVMSG("assert failed", "file: %s, line: %d.", __FILE__, __LINE__);\
		assert(0);\
	}\
}

#include "d3d9.h"
#pragma comment(lib, "d3d9.lib")

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")

#include "dxgi.h"
#pragma comment(lib, "dxgi.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// d3dx includes
#include <DirectXMath.h>
#include <DirectXColors.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef int int32;

typedef DirectX::XMFLOAT2 LVVec2;
typedef DirectX::XMFLOAT3 LVVec3;
typedef DirectX::XMFLOAT4 LVVec;
typedef DirectX::XMMATRIX LVMatrix;

#define ALIGNED_LOSTVR(x) __declspec(align(x))
#define VERIFY_HRESULT(result, cmd, info)\
{\
	if (FAILED(result))\
	{\
		assert(0 && info);\
		cmd;\
	}\
}

// Vertex data for a colored cube.
ALIGNED_LOSTVR(16) struct MeshVertex
{
	LVVec3 Position;
	LVVec2 Texcoord;
	MeshVertex() = default;
	MeshVertex(LVVec3 p, LVVec2 t) : Position(p), Texcoord(t) { }
};

ALIGNED_LOSTVR(16) struct FrameBufferWVP
{
	LVMatrix W;
	LVMatrix V;
	LVMatrix P;
	FrameBufferWVP() = default;
	FrameBufferWVP(const LVMatrix& w, const LVMatrix& v, const LVMatrix& p)
		: W(w), V(v), P(p) {}
};

#ifndef LVLog
#define LVLog
#endif

#ifndef LVLogErr
#define LVLogErr
#endif

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

class HighFrequencyCounter
{
	LARGE_INTEGER TicksPerSec;
	LARGE_INTEGER Timestamp;

public:
	HighFrequencyCounter()
	{
		QueryPerformanceFrequency(&TicksPerSec);
	}

	void Start()
	{
		QueryPerformanceCounter(&Timestamp);
	}

	//************************************
	// Method:    Stop
	// FullName:  HighFrequencyCounter::Stop
	// Access:    public 
	// Returns:   double in ms
	// Qualifier:
	//************************************
	double Stop()
	{
		LARGE_INTEGER end;
		QueryPerformanceCounter(&end);
		return (double)((end.QuadPart - Timestamp.QuadPart) * 1000.0 / TicksPerSec.QuadPart);
	}
};

class ScopedHighFrequencyCounter
{
	const CHAR* MsgHead;
	HighFrequencyCounter Counter;

public:
	ScopedHighFrequencyCounter(const CHAR* head) : MsgHead(head), Counter()
	{
		Counter.Start();
	}

	~ScopedHighFrequencyCounter()
	{
		double duration = Counter.Stop();
		{
#ifndef NDEBUG
			LVMSG("ScopedHighFrequencyCounter", "%s\t\t%fms", MsgHead?MsgHead:"<null>", duration);
#endif
		}
	}
};

static GlobalSharedData SGlobalSharedDataInst;

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result);
extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* Result);
