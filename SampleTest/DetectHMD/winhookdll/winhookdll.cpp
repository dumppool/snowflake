// winhookdll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <errno.h>
#include <vector>
#include <map>

#define LVMSG(Cap, ...)\ printf(__VA_ARGS__);

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)


struct HOOKDATA
{
	int		Type;
	HOOKPROC	HookProc;
	HHOOK Handle;

	HOOKDATA(int type, HOOKPROC proc) : Type(type)
		, HookProc(proc)
		, Handle(nullptr)
	{
	}
};

std::map<UINT, HOOKDATA*> s_HookDatas;

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll CallWndProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll GetMsgProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK DebugProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll DebugProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll CBTProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT(-1);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll MouseProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return LRESULT();
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll KeyboardProc";
	CHAR msg[1024];
	char key = wParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, wParam, lParam);
	MessageBoxA(NULL, msg, head, 0);
	return CallNextHookEx(s_HookDatas[WH_KEYBOARD]->Handle, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProcLL(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll KeyboardProcLL";
	KBDLLHOOKSTRUCT kb = *(KBDLLHOOKSTRUCT*)lParam;
	CHAR msg[1024];
	char key = kb.vkCode;
	snprintf(msg, sizeof(msg), "nCode: %d, wparam: %c, lparam: %d", nCode, key, wParam);
	MessageBoxA(NULL, msg, head, 0);
	return CallNextHookEx(s_HookDatas[WH_KEYBOARD_LL]->Handle, nCode, wParam, lParam);
}

LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "dll MessageProc";
	CHAR msg[1024];
	MSG m = *(MSG*)lParam;
	snprintf(msg, sizeof(msg), "nCode: %d, wnd: 0x%x, msg: %d, lparam: %d, wparam: %d", nCode, m.hwnd, m.message, m.lParam, m.wParam);
	MessageBoxA(NULL, msg, head, 0);
	return CallNextHookEx(s_HookDatas[WH_MSGFILTER]->Handle, nCode, wParam, lParam);
}

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

extern "C" _declspec(dllexport) void _cdecl installhook(HINSTANCE hInstance, HWND hWnd)
{
	if (s_HookDatas.empty())
	{
		s_HookDatas[WH_MSGFILTER] = new HOOKDATA(WH_MSGFILTER, (HOOKPROC)MessageProc);
		//s_HookDatas[WH_KEYBOARD] = new HOOKDATA(WH_KEYBOARD, (HOOKPROC)KeyboardProc);
		//s_HookDatas[WH_KEYBOARD_LL] = new HOOKDATA(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProcLL);
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
			for (auto& hk : s_HookDatas)
			{
				CHAR cnt[128];
				snprintf(cnt, sizeof(cnt), "dll set windows hook ex, type: %d, thread: %d, module: 0x%x, hinst: 0x%x",
					hk.second->Type, tid, GetModuleHandleA("winhookdll.dll"), HINST_THISCOMPONENT);
				MessageBoxA(NULL, cnt, "C", 0);
				hk.second->Handle = SetWindowsHookExA(hk.second->Type, hk.second->HookProc, 0, tid);
			}
		}
	}
}