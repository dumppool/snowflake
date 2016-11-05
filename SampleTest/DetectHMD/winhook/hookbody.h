#pragma once

#include "stdafx.h"
#include <vector>
#include <process.h>

#define LVMSG(Cap, ...)\ printf(__VA_ARGS__);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "CallWndProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "GetMsgProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK DebugProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "DebugProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "CBTProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "MouseProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT();
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "KeyboardProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "MessageProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

struct HOOKDATA
{
	int		Type;
	HOOKPROC	HookProc;

	HOOKDATA(int type, HOOKPROC proc) : Type(type)
		, HookProc(proc)
	{
	}
};

static bool GetWndFromProcessID(DWORD pid, HWND hwnd[], UINT& nwnd)
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
		}
	}

	return true;
}

typedef void (_cdecl* PFN_installhook)(HINSTANCE hInstance, HWND hWnd);

inline unsigned __stdcall installhook_dll(void* buf)
{
	CHAR cnt[128];
	snprintf(cnt, sizeof(cnt), "installhook_dll: %d", GetCurrentThreadId());
	MessageBoxA(NULL, cnt, "B", 0);

	HMODULE hmod = LoadLibraryA("winhookdll.dll");
	PFN_installhook fn = (PFN_installhook)GetProcAddress(hmod, "installhook");
	fn(NULL, NULL);
	return 0;
}

inline void installhook(HINSTANCE hInstance, HWND hWnd)
{
	CHAR cnt[128];
	snprintf(cnt, sizeof(cnt), " call install thread: %d", GetCurrentThreadId());
	MessageBoxA(NULL, cnt, "A", 0);

	bool bUseDll = true;
	bool bMultiThread = 0;
	if (bUseDll)
	{
		if (bMultiThread)
		{
			unsigned tid;
			HANDLE hthread = (HANDLE)_beginthreadex(NULL, 0, installhook_dll, nullptr, CREATE_SUSPENDED, &tid);
			ResumeThread(hthread);
		}
		else
		{
			installhook_dll(nullptr);
		}
	}
	else
	{
		static std::vector<HOOKDATA> s_Hooks;

		if (s_Hooks.empty())
		{
			s_Hooks.push_back(HOOKDATA(WH_KEYBOARD, (HOOKPROC)KeyboardProc));
			//s_Hooks.push_back(HOOKDATA(WH_MOUSE, (HOOKPROC)MouseProc));
		}

		std::vector<DWORD> tids;
		HWND wnd[128] = { NULL };
		UINT nwnd = 0;
		GetWndFromProcessID(GetCurrentProcessId(), wnd, nwnd);
		for (UINT i = 0; i < nwnd; ++i)
		{
			DWORD tid = GetWindowThreadProcessId(wnd[i], nullptr);
			bool bFound = tid != NULL;
			for (auto i : tids)
			{
				if (i == tid)
				{
					bFound = false;
					break;
				}
			}

			if (bFound)
			{
				tids.push_back(tid);
				for (auto hk : s_Hooks)
				{
					SetWindowsHookExA(hk.Type, hk.HookProc, HINST_THISCOMPONENT, tid);
				}
			}
		}
	}
}