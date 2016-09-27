// HookExperiment2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

int main_experiment2()
{
	ListProcess();
	DWORD processId;
	std::wcout << "Enter the target process Id: ";
	std::cin >> processId;
	WCHAR* dllToInject = L"..\\debug\\MorpheusInterceptor_x32.dll";
	WCHAR* dllToInject64 = L"..\\x64\\debug\\MorpheusInterceptor_x64.dll";
	wprintf(L"Attempting to inject");

	DWORD data = 0;//custom data
	NTSTATUS nt = RhInjectLibrary(
		processId,   // The process to inject into
		0,           // ThreadId to wake up upon injection
		EASYHOOK_INJECT_DEFAULT,
		dllToInject, // 32-bit
		dllToInject64,		 // 64-bit not provided
		&data, // data to send to injected DLL entry point
		sizeof(DWORD)// size of data to send
	);
	if (nt != 0)
	{
		wprintf(L"RhInjectLibrary failed with error code = %d\n", nt);
		PWCHAR err = RtlGetLastErrorString();
		std::wcout << err << "\n";
	}
	else
	{
		std::wcout << L"Library injected successfully.\n";
	}
	std::wcout << "Press Enter to exit";
	std::wstring input;
	std::getline(std::wcin, input);
	std::getline(std::wcin, input);
	return 0;
}

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
			}
			_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, aProcesses[i]);
			CloseHandle(hProcess);
		}
	}

}