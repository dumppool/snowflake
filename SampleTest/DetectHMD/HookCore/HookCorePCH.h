#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

//#pragma warning(disable : 4996)
#include <assert.h>
#include <vector>
#include <string>

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

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")

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


extern "C" _declspec(dllexport) void _cdecl InstallHook(int* Result);
extern "C" _declspec(dllexport) void _cdecl UninstallHook(int* Result);