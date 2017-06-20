/*!
 * file WindowsRouters.cpp
 *
 * author luoxw
 * date 十一月 2016
 *
 * 
 */
#include "HookCorePCH.h"
#include "RouteAPIs.h"

#include "TlHelp32.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

LRESULT CALLBACK CallWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "CallWndProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT(-1);
}

LRESULT CALLBACK GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "GetMsgProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT(-1);
}

LRESULT CALLBACK DebugProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "DebugProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT(-1);
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "CBTProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT(-1);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "MouseProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT();
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "KeyboardProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT(-1);
}

LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	const CHAR* head = "MessageProc";
	LVMSG(head, "code: %d, wParam: %d, lParam: %d", nCode, wParam, lParam);
	return LRESULT(-1);
}


namespace lostvr
{
	struct HOOKDATA
	{
		int		Type;
		HOOKPROC	HookProc;

		HOOKDATA(int type, HOOKPROC proc) : Type(type)
			, HookProc(proc)
		{
		}
	};

	class WndProcRouter : public RouteObject
	{
	public:
		WndProcRouter();

		virtual bool InstallRoute() override;
		virtual bool UninstallRoute() override;

		virtual const CHAR* GetRouteString() const override
		{
			return "[WndProcRouter]";
		}

	protected:
		std::vector<HOOKDATA> HookDatas;
		std::vector<HHOOK> HookHandles;
		std::vector<HWINEVENTHOOK> WinEventHookHandles;
	};

	static WndProcRouter* SWndProcRouter = nullptr;

	typedef LRESULT(CALLBACK* PFN_WndProc)(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	static LRESULT CALLBACK CustomeWndProc(_In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		const CHAR* head = "CustomeWndProc";
		if (SWndProcRouter != nullptr)
		{
			LVMSG(head, "hWnd: %d, Msg: %d, wParam: %d, lParam: %d", hWnd, Msg, wParam, lParam);
			PFN_WndProc func = (PFN_WndProc)SWndProcRouter->GetOriginalEntry();
			return func(hWnd, Msg, wParam, lParam);
		}
		else
		{
			return LRESULT(0);
		}
	}

	typedef LRESULT(WINAPI* PFN_CallWindowProc)(_In_ WNDPROC lpPrevWndFunc, _In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam);
	LRESULT WINAPI CustomeCallWindowProc(_In_ WNDPROC lpPrevWndFunc, _In_ HWND hWnd, _In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		const CHAR* head = "CustomeCallWindowProc";
		if (SWndProcRouter != nullptr)
		{
			LVMSG(head, "hWnd: %d, Msg: %d, wParam: %d, lParam: %d", hWnd, Msg, wParam, lParam);
			PFN_CallWindowProc func = (PFN_CallWindowProc)SWndProcRouter->GetOriginalEntry();
			return func(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
		}
		else
		{
			return LRESULT(0);
		}
	}

	static LRESULT CALLBACK HHookWndProc(_In_ UINT Msg, _In_ WPARAM wParam, _In_ LPARAM lParam)
	{
		const CHAR* head = "CustomeCallWindowProc";
		LVMSG(head, "hWnd: %d, Msg: %d, wParam: %d, lParam: %d", 0, Msg, wParam, lParam);
		return LRESULT(0);
	}

	WndProcRouter::WndProcRouter() : RouteObject(nullptr)
	{
		HookDatas.clear();
		HookHandles.clear();
		WinEventHookHandles.clear();
	}

	typedef HHOOK (WINAPI* PFN_SetWindowsHookExA)(
			_In_ int idHook,
			_In_ HOOKPROC lpfn,
			_In_opt_ HINSTANCE hmod,
			_In_ DWORD dwThreadId);

	typedef DWORD (WINAPI* PFN_GetWindowThreadProcessId)(
			_In_ HWND hWnd,
			_Out_opt_ LPDWORD lpdwProcessId);

	bool WndProcRouter::InstallRoute()
	{
		const CHAR* head = "WndProcRouter::InstallRoute";

		HINSTANCE hInst = HINST_THISCOMPONENT;
		HMODULE hmod = LoadLibraryA("User32.dll");
		if (hmod == nullptr)
		{
			LVMSG(head, "load User32.dll failed");
			return false;
		}

		PFN_SetWindowsHookExA fn = (PFN_SetWindowsHookExA)GetProcAddress(hmod, "SetWindowsHookExA");
		PFN_GetWindowThreadProcessId fn2 = (PFN_GetWindowThreadProcessId)GetProcAddress(hmod, "GetWindowThreadProcessId");
		if (fn == nullptr || fn2 == nullptr)
		{
			LVMSG(head, "get SetWindowsHookExA or GetWindowThreadProcessId failed");
			FreeLibrary(hmod);
			return false;
		}

		if (HookDatas.empty())
		{
			//HookDatas.push_back(HOOKDATA(WH_CALLWNDPROC, (HOOKPROC)CallWindowProc));
			//HookDatas.push_back(HOOKDATA(WH_CBT, (HOOKPROC)CBTProc));
			//HookDatas.push_back(HOOKDATA(WH_DEBUG, (HOOKPROC)DebugProc));
			//HookDatas.push_back(HOOKDATA(WH_GETMESSAGE, (HOOKPROC)GetMsgProc));
			HookDatas.push_back(HOOKDATA(WH_KEYBOARD, (HOOKPROC)KeyboardProc));
			HookDatas.push_back(HOOKDATA(WH_MOUSE, (HOOKPROC)MouseProc));
			//HookDatas.push_back(HOOKDATA(WH_MSGFILTER, (HOOKPROC)MessageProc));
		}

		HANDLE hSnapshort = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);    
		if (hSnapshort == INVALID_HANDLE_VALUE)
		{
			LVMSG(head, "CreateToolhelp32Snapshot failed");
			return false;
		}

		THREADENTRY32 stcThreadInfo;
		stcThreadInfo.dwSize = sizeof(stcThreadInfo);

		BOOL  bRet = Thread32First(hSnapshort, &stcThreadInfo);
		DWORD dwProId = -1;  // 保存上一个线程的进程ID   
		unsigned unCount = 0;

		while (1&&bRet)
		{
			if (GetCurrentProcessId() != stcThreadInfo.th32OwnerProcessID)
			{
				bRet = Thread32Next(hSnapshort, &stcThreadInfo);
				continue;
			}

			if (dwProId != stcThreadInfo.th32OwnerProcessID)
			{
				if (dwProId != -1)
				{
					unCount = 0;
				}

				dwProId = stcThreadInfo.th32OwnerProcessID;
				LVMSG(head, "process %d", dwProId);
				LVMSG(head, "thread ID:\t\tpid of thread\t\tpriority of thread\t\t%d", fn2(GetForegroundWindow(), NULL));
			}

			LVMSG(head, "%d\t\t\t%d\t\t\t%d", stcThreadInfo.th32ThreadID, stcThreadInfo.th32OwnerProcessID, stcThreadInfo.tpBasePri);
			++unCount;

			//if (stcThreadInfo.th32OwnerProcessID == GetCurrentProcessId())
			//{
			//	for (auto& data : HookDatas)
			//	{
			//		HHOOK h = fn(data.Type, data.HookProc, hInst, stcThreadInfo.th32ThreadID);

			//		if (h == nullptr)
			//		{
			//			DWORD err = GetLastError();
			//			LVMSG(head, "failed to set windows hook ex, type: %d, hinst: 0x%x, err: %d", data.Type, hInst, err);
			//		}
			//		else
			//		{
			//			LVMSG(head, "set windows hook ex, type: %d, handle: 0x%x", data.Type, h);
			//			HookHandles.push_back(h);
			//		}
			//	}
			//}

			bRet = Thread32Next(hSnapshort, &stcThreadInfo);
		}

		HWND wnd[128] = { NULL };
		UINT nwnd = 0;
		GetWndFromProcessID(GetCurrentProcessId(), wnd, nwnd);
		for (UINT i = 0; i < nwnd; ++i)
		{
			DWORD p;
			DWORD t = GetWindowThreadProcessId(wnd[i], &p);
			LVMSG(head, "found window: 0x%x, i: %d, foreground window: 0x%x, pid: %d, tid: %d", wnd[i], i, GetForegroundWindow(), p, t);
			for (auto& data : HookDatas)
			{
				HHOOK h = fn(data.Type, data.HookProc, hInst, t);
				//data.HHook = SetWindowsHookA(data.Type, data.HookProc);

				if (h == nullptr)
				{
					DWORD err = GetLastError();
					LVMSG(head, "failed to set windows hook ex, type: %d, hinst: 0x%x, err: %d", data.Type, hInst, err);
				}
				else
				{
					HookHandles.push_back(h);
					LVMSG(head, "set windows hook ex, type: %d, handle: 0x%x", data.Type, h);
				}
			}
		}

		//for (auto& data : HookDatas)
		//{
		//	DWORD dw;
		//	HHOOK h = fn(data.Type, data.HookProc, hInst, fn2(GetForegroundWindow(), &dw));
		//	//data.HHook = SetWindowsHookA(data.Type, data.HookProc);

		//	LVMSG(head, "set windows hook ex, proc id: %d, curr proc id: %d", dw, GetCurrentProcessId());
		//	if (h == nullptr)
		//	{
		//		DWORD err = GetLastError();
		//		LVMSG(head, "failed to set windows hook ex, type: %d, hinst: 0x%x, err: %d", data.Type, hInst, err);
		//	}
		//	else
		//	{
		//		HookHandles.push_back(h);
		//		LVMSG(head, "set windows hook ex, type: %d, handle: 0x%x", data.Type, h);
		//	}
		//}

		//CoInitialize(NULL);
		//WinEventHookHandles.push_back(SetWinEventHook(EVENT_SYSTEM_MENUSTART));

		FreeLibrary(hmod);
		return true;
	}

	bool WndProcRouter::UninstallRoute()
	{
		const CHAR* head = "WndProcRouter::UninstallRoute";
		for (auto& h : HookHandles)
		{
			BOOL ret = UnhookWindowsHookEx(h);
			LVMSG(head, "unset windows hook ex %s, handle: 0x%x", ret == 1?"succeeded":"failed", h);
		}

		HookDatas.clear();
		return true;
	}

};