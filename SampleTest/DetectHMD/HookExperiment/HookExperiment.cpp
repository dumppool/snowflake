// HookExperiment.cpp : 定义控制台应用程序的入口点。

#include "HookInstaller.h"

//extern void SetHook_D3D11();

using namespace std;

typedef unsigned short uint16;
typedef unsigned int uint32;

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

	return 0;
}

int main_experiment2()
{
	ListProcess();
	DWORD processId;
	std::wcout << "Enter the target process Id: ";
	std::cin >> processId;

	uint32_t ret = InjectIntoProcess(processId, nullptr, false);
	printf("\nmain_experiment2: InjectIntoProcess returned %d", ret);

	printf("\n");
	::system("PAUSE");
	InjectFinished(processId);
	return 0;
}

int main()
{
	HANDLE hModule = ::LoadLibrary(TEXT(HookCoreModule));

	//HMODULE hVR = ::LoadLibrary(TEXT("C:\\Users\\Administrator\\Documents\\GitHub\\snowflake\\SampleTest\\DetectHMD\\HookCore3\\openvr-lib\\bin\\win64\\openvr_api.dll"));
	//void* pFn = ::GetProcAddress(hVR, "VR_Init");
	//if (pFn)
	//{
	//	printf("1");
	//}
	//else
	//{
	//	printf("2");
	//}

#if 0
	DWORD pid = 0;
	LaunchAndInjectIntoProcess(
		"D:/GitUnreal/UnrealEngine-4.11/Engine/Binaries/Win64/UE4Editor.exe", 
		//"G:/Games/Heroes of the Storm/Support64/HeroesSwitcher_x64.exe",
		//"G:/Games/World of Warcraft/Wow-64.exe",
		//"D:/GitUnreal/UnrealEngine-4.11/Engine/Binaries/Win64/UE4Editor.exe",
		nullptr, 
		"E:/UnrealProjects/LostVR411/XYVR410.uproject -game -cameratarget=1 -log", 
		//"",
		//" E:/UnrealProjects/LostVR411/XYVR410.uproject -enablehmd = 0 - hidewindow = 0 - unique - cameratarget = 1 -game",
		nullptr, false,	pid);

	printf("\n");
	::system("PAUSE");
	InjectFinished(pid);
#else
	main_experiment2();
#endif
    return 0;
}