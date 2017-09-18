/*
 * file HookCorePCH.h
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shellapi.h>

//#pragma warning(disable : 4996)
#include <assert.h>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>

#include "json.hpp"
using FJson = nlohmann::json;
//#include <functional>

//#define INCLUDE_DIRECTXTK
#ifdef INCLUDE_DIRECTXTK
#include "../DirectXTK/Inc/ScreenGrab.h"
#pragma comment(lib, "../DirectXTK/Bin/Desktop_2015/Win32/Debug/DirectXTK.lib")
#endif

#define ENABLE_LOG_DETAIL 1

#ifdef _DEBUG
#define LOG_DETAIL ENABLE_LOG_DETAIL
#else
#define LOG_DETAIL 0
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
#define SAFE_DELETE_ARRAY(p)  \
  do                          \
  {                           \
    if(p)                     \
    {                         \
      delete[] (p);		      \
      (p) = nullptr;          \
    }                         \
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

#ifndef LOGFILE_PREFIX
#define LOGFILE_PREFIX "Undefined"
#endif

static const CHAR* SLogFilePrefixDefault = "Default";
static const CHAR* SLogError = "ErrorReport";
static const CHAR* SLogDetail = "FrameDetail";

#define LVMSG(Cap, ...) {\
log_cap_cnt(SLogFilePrefixDefault, Cap, __VA_ARGS__);}

#define LVMSG2(Prefix, Cap, ...) {\
log_cap_cnt(Prefix, Cap, __VA_ARGS__);}

#define LVERROR(Head, ...) LVMSG2(SLogError, Head, __VA_ARGS__)
#define LVDETAIL(Head, ...) LVMSG2(SLogDetail, Head, __VA_ARGS__)

#include <time.h>
#include <fstream>
using namespace std;
inline static void log_cap_cnt(const CHAR* Prefix, const CHAR* cap, const CHAR* fmt, ...)
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

	char logpath[1024];
	snprintf(logpath, 1023, "%s\\_LostVR", envdir);
	free(envdir);
	envdir = nullptr;

	DWORD pid = ::GetCurrentProcessId();
	char filepath[1024];
	snprintf(filepath, 1023, "%s\\%s-%d.log", logpath, Prefix, pid);

	ofstream logfile(filepath, ios::app | ios::out);
	if (!logfile)
	{
		SECURITY_ATTRIBUTES secu;
		secu.bInheritHandle = TRUE;
		secu.lpSecurityDescriptor = nullptr;
		secu.nLength = sizeof(SECURITY_ATTRIBUTES);
		if (::CreateDirectoryA(logpath, &secu) == FALSE)
		{
			::MessageBoxA(0, logpath, "failed to create directory", 0);
			return;
		}
	}

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

#define LVASSERT2(Condition, Prefix, Cap, ...) {\
	if (!(Condition))\
	{\
		LVMSG2(Prefix, Cap, __VA_ARGS__);\
		LVMSG2(Prefix, "assert failed", "file: %s, line: %d.", __FILE__, __LINE__);\
		assert(0);\
	}\
}

inline bool GetWndFromProcessID(DWORD pid, HWND hwnd[], UINT& nwnd)
{
	HWND temp = NULL;
	nwnd = 0;
	while ((temp = FindWindowEx(NULL, temp, NULL, NULL)) && sizeof(hwnd) > nwnd)
	{
		DWORD p;
		GetWindowThreadProcessId(temp, &p);
		if (pid == p)
		{
			hwnd[nwnd++] = temp;
			//break;
		}
	}

	HWND root = hwnd[nwnd-1];
	temp = root;
	//nwnd = 1;

	while ((temp = GetParent(temp)) != NULL)
	{
		DWORD p;
		GetWindowThreadProcessId(temp, &p);
		if (pid == p)
		{
			root = temp;
		}
		else
		{
			break;
		}
	}

	LVMSG("GetWndFromProcessID", "last wnd: %x, root: %x", hwnd[nwnd - 1], root);
	hwnd[0] = root;
	nwnd = 1;
	return true;
}

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

inline char LowerChar(char c)
{
	if (c >= 'A' && 'Z' >= c) 
	{ 
		return c - ('A' - 'a');
	}
	
	return c;
}

inline char UpperChar(char c)
{
	if (c >= 'a' && 'z' >= c)
	{
		return c + ('A' - 'a');
	}

	return c;
}

inline string SubChar(const string& src, char c)
{
	string dst(src);
	auto it = dst.begin();
	while (it != dst.end())
	{
		if ((*it) == c)
		{
			it = dst.erase(it);
			continue;
		}

		++it;
	}

	return dst;
}

inline vector<string> SplitChar(const string& src, char sep)
{
	vector<string> dst;
	string copy(src);
	auto it = copy.begin();
	auto it2 = copy.begin();
	while (1)
	{
		if ((*it) == sep)
		{
			if (it != it2)
			{
				dst.push_back(string(it2, it));
			}

			it = it2 = it + 1;
		}
		else
		{
			++it;
		}

		if (it == copy.end())
		{
			if (it != it2)
			{
				dst.push_back(string(it2, it));
			}

			break;
		}
	}

	return dst;
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
typedef DirectX::XMVECTOR LVQuat;
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

#define VERIFY_HRESULT2(Result, FailureBranch, Prefix, Cap, ...)\
{\
	HRESULT _hr = Result;\
	if (FAILED(_hr))\
	{\
		LVMSG2(Prefix, Cap, __VA_ARGS__);\
		FailureBranch;\
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
	WCHAR*			HookCoreDllPath;

	WCHAR*			DependencyDirectory;
	WCHAR*			OpenVRDllPath;
	WCHAR*			LibOVRDllPath;

	WCHAR*			ShaderFilePath;

	HWND			TargetWindow;
	bool			bDrawCursor;
	bool			bFakeMouseMove;
	bool			bHoldWhenMoving;
	bool			bEnableJoystick;

	HWND			DefaultWindow;

public:

	HINSTANCE hInst;

	GlobalSharedData();
	~GlobalSharedData();

	const WCHAR* GetHookCoreDllName() const;
	WCHAR* GetHookCoreDllPath();

	const WCHAR* GetOpenVRDllName() const;
	WCHAR* GetOpenVRDllPath();

	const WCHAR* GetLibOVRDllName() const;
	WCHAR* GetLibOVRDllPath();

	WCHAR* GetShaderFilePath();

	HWND GetTargetWindow() const;
	void SetTargetWindow(HWND wnd);

	bool GetBDrawCursor() const;
	void SetBDrawCursor(bool bEnable);

	bool GetBFakeMouseMove() const;
	void SetBFakeMouseMove(bool bEnable);

	bool GetBHoldWhenMoving() const;
	void SetBHoldWhenMoving(bool bEnable);

	bool GetBEnableJoystick() const;
	void SetBEnableJoystick(bool bEnable);

	const WCHAR* GetDependencyDirectoryName() const;
	WCHAR* GetDependencyDirectory();

	HWND GetDefaultWindow();
};

class HighFrequencyCounter
{
	LARGE_INTEGER TicksPerSec;
	LARGE_INTEGER Timestamp;

public:
	HighFrequencyCounter();

	void Start();

	//************************************
	// Method:    Stop
	// FullName:  HighFrequencyCounter::Stop
	// Access:    public 
	// Returns:   double in ms
	// Qualifier:
	//************************************
	double Stop();

	double Count();

	bool Started() const;
};

class ScopedHighFrequencyCounter
{
	const CHAR* MsgHead;
	HighFrequencyCounter Counter;

public:
	ScopedHighFrequencyCounter(const CHAR* head);
	~ScopedHighFrequencyCounter();
};

extern GlobalSharedData SGlobalSharedDataInst;

extern void FakeKeyPress(uint32 key);
extern void FakeKeyRelease(uint32 key);

extern void FakeMousePress(bool left);
extern void FakeMouseRelease(bool left);

extern void FakeMouseMove(uint32 x, uint32 y);

extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result);
extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* Result);
