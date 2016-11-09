// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO:  在此处引用程序需要的其他头文件

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
#endif
#define LVMSG(Cap, ...) {\
log_cap_cnt(SLogFilePrefixDefault, Cap, __VA_ARGS__);}

#define LVMSG2(Prefix, Cap, ...) {\
log_cap_cnt(Prefix, Cap, __VA_ARGS__);}

static const CHAR* SLogFilePrefixDefault = "Default";

#include <time.h>
#include <fstream>
using namespace std;
inline static void log_cap_cnt(const CHAR* prefix, const char* cap, const char* fmt, ...)
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
	snprintf(filepath, 1023, "%s\\%s-%d.log", logpath, prefix, pid);

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

	//OutputDebugStringA(msg);
	//OutputDebugStringA("\n");
}

#include <assert.h>
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

#define VERIFY_HRESULT2(Result, FailureBranch, Prefix, Cap, ...)\
{\
	HRESULT _hr = Result;\
	if (FAILED(hr))\
	{\
		LVMSG2(Prefix, Cap, __VA_ARGS__);\
		FailureBranch;\
	}\
}