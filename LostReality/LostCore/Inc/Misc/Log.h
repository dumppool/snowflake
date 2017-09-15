/*
* file Log.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

FORCEINLINE static void log_cap_cnt(const CHAR* prefix, const CHAR* head, const CHAR* fmt, ...)
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
	logfile << head << ": " << string(msg).c_str() << endl;
	logfile.flush();
}

#define LVMSG_PREFIX(prefix, head, ...) {\
log_cap_cnt(prefix, head, __VA_ARGS__);}

//#ifdef MODULE_MSG_PREFIX
#define LVMSG(head, ...) LVMSG_PREFIX(MODULE_MSG_PREFIX, head, __VA_ARGS__)
//#endif

//#ifdef MODULE_ERR_PREFIX
#define LVERR(head, ...) LVMSG_PREFIX(MODULE_ERR_PREFIX, head, __VA_ARGS__)
//#endif

//#ifdef MODULE_WARN_PREFIX
#define LVWARN(head, ...) LVMSG_PREFIX(MODULE_WARN_PREFIX, head, __VA_ARGS__)
//#endif

#define LVASSERT_PREFIX(prefix, Condition, head, ...) {\
	if (!(Condition))\
	{\
		LVMSG_PREFIX(prefix, head, __VA_ARGS__);\
		LVMSG_PREFIX(prefix, "assert failed", "file: %s, line: %d.", __FILE__, __LINE__);\
		assert(0);\
	}\
}

#define LVASSERT(Condition, head, ...) LVASSERT_PREFIX(SLogPrefixError, Condition, head, __VA_ARGS__)