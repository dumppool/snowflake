// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

#define ENABLEHOOKLIB_MHOOK
//#define ENABLEHOOKLIB_EASYHOOK

#ifdef ENABLEHOOKLIB_EASYHOOK
#include "easyhook/Public/easyhook.h"
#endif

#ifdef ENABLEHOOKLIB_MHOOK
#include "mhook-lib/mhook.h"
#endif


#include <assert.h>
#include <vector>
#include <string>

#define LOG_FILEPATH "D:\\HookCore3.log"
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
	int sz = vsnprintf(msg, 1023, fmt, args);
	//msg[sz] = '\0';
	va_end(args);

	ofstream logfile(LOG_FILEPATH, ios::app | ios::out);
	if (!logfile)
	{
		::MessageBoxA(0, "failed to open log file", "LOG", 0);
		return;
	}

	time_t t = ::time(0);
	tm curr;
	::localtime_s(&curr, &t);
	logfile << 1900 + curr.tm_year << "/" << curr.tm_mon + 1 << "/" << curr.tm_mday << " " << curr.tm_hour << ":" << curr.tm_min << ":" << curr.tm_sec << "\t";
	logfile << "PIE: " << ::GetCurrentProcessId() << "\t\t";
	logfile << cap << ": " << string(msg).c_str() << endl;
	logfile.flush();
}

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")

// TODO:  在此处引用程序需要的其他头文件
