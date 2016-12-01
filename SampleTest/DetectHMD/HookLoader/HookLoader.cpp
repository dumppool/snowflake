// HookLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "HookInstaller.h"
#include "StringUtils.h"

using namespace std;

int ListProcess()
{
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}
	cProcesses = cbNeeded / sizeof(DWORD);

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, aProcesses[i]);
			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
					&cbNeeded))
				{
					GetModuleBaseName(hProcess, hMod, szProcessName,
						sizeof(szProcessName) / sizeof(TCHAR));
				}

				_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, aProcesses[i]);
			}

			CloseHandle(hProcess);
		}
	}

	return 0;
}


PROCESS_INFORMATION LaunchProcess(const char *app, const char *workingDir, const char *cmdLine)
{
	if (app == nullptr)
	{
		return PROCESS_INFORMATION();
	}

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	SECURITY_ATTRIBUTES psec;
	SECURITY_ATTRIBUTES tsec;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&psec, sizeof(psec));
	ZeroMemory(&tsec, sizeof(tsec));

	psec.nLength = sizeof(psec);
	tsec.nLength = sizeof(tsec);

	std::wstring appdir = dirname(UTF8ToWide(std::string(app)));

	WCHAR workdir[MAX_PATH];
	_snwprintf_s(workdir, MAX_PATH - 1, L"%s\0",
		(workingDir != nullptr && workingDir[0] != 0) ? UTF8ToWide(workingDir).c_str() : appdir.c_str());

	WCHAR cmd[1024];
	_snwprintf_s(cmd, 1023, L"\"%s\" %s\0", UTF8ToWide(std::string(app)).c_str(), UTF8ToWide(std::string(cmdLine)).c_str());

	BOOL retValue = CreateProcessW(NULL, cmd, &psec, &tsec, false, CREATE_UNICODE_ENVIRONMENT, NULL, workdir, &si, &pi);

	return pi;
}

int HookMain(DWORD* pid)
{
	DWORD processId;
	if (pid == nullptr)
	{
		ListProcess();
		std::wcout << "Enter the target process Id: ";
		std::cin >> processId;
	}
	else
	{
		processId = *pid;
	}

	uint32_t ret = InjectIntoProcess(processId, nullptr, false);
	printf("\nInjectIntoProcess returned %d", ret);

	printf("\n");
	::system("PAUSE");
	InjectFinished(processId);
	return 0;
}

int main(int argc, char* argv[])
{
	Sleep(1000);

	bool bRunningHook = false;
	printf("num of commands: %d\n", argc);
	for (int i = 0; i < argc; ++i)
	{
		printf("command: %s\n", argv[i]);
		if (0 == strcmp(argv[i], "x86"))
		{
			SUseWin32 = true;
			printf("use win32 version\n");
		}

		if (!bRunningHook && (-1 != strcmp(argv[i], "pid")))
		{
			DWORD pid = atol(argv[i] + 4);
			HookMain(&pid);
			bRunningHook = true;
		}

		if (!bRunningHook  && (-1 != strcmp(argv[i], "app")))
		{
			const CHAR* app = argv[i] + 4;
			PROCESS_INFORMATION pi = LaunchProcess(app, "", "");
			HookMain(&pi.dwProcessId);
			bRunningHook = true;
		}
	}

	//HANDLE hModule = ::LoadLibrary(SGlobalSharedDataInst.GetHookCoreDllPath());
	if (!bRunningHook)
	{
		HookMain(nullptr);
	}
	
	return 0;
}

