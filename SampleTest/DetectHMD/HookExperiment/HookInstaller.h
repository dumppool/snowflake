#pragma once
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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

#define HookCoreModule "HookCore3.dll"

void InjectDLL(HANDLE hProcess, wstring libName);
uintptr_t FindRemoteDLL(DWORD pid, wstring libName);
void InjectFunctionCall(HANDLE hProcess, uintptr_t renderdoc_remote, const char *funcName, void *data, const size_t dataLen);
PROCESS_INFORMATION RunProcess(const char *app, const char *workingDir, const char *cmdLine);
uint32_t InjectIntoProcess(uint32_t pid, const char *logfile, bool waitForExit);
uint32_t LaunchAndInjectIntoProcess(const char *app, const char *workingDir, const char *cmdLine, const char *logfile, bool waitForExit, DWORD& pid);
void InjectFinished(DWORD pid);