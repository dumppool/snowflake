// HookLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#include "HookInstaller.h"

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

int HookMain()
{
	ListProcess();
	DWORD processId;
	std::wcout << "Enter the target process Id: ";
	std::cin >> processId;

	uint32_t ret = InjectIntoProcess(processId, nullptr, false);
	printf("\nInjectIntoProcess returned %d", ret);

	printf("\n");
	::system("PAUSE");
	InjectFinished(processId);
	return 0;
}

int main()
{
	HANDLE hModule = ::LoadLibrary(TEXT(HookCoreModule));
	HookMain();
    return 0;
}

