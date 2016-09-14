// HookExperiment.cpp : 定义控制台应用程序的入口点。
#include "HookInstaller.h"

//extern void SetHook_D3D11();

using namespace std;

typedef unsigned short uint16;
typedef unsigned int uint32;

struct ProcessLoader
{
public:
	ProcessLoader(const char* Url, const char* Params, bool bLauncheDetached = false, bool bLaunchHidden = false, bool bLaunchReallyHidden = false);

protected:
	PROCESS_INFORMATION ProcInfo;
};

ProcessLoader::ProcessLoader(const char* Url, const char* Params, bool bLaunchDetached, bool bLaunchHidden, bool bLaunchReallyHidden)
{
	// initialize process attributes
	SECURITY_ATTRIBUTES Attr;
	Attr.nLength = sizeof(SECURITY_ATTRIBUTES);
	Attr.lpSecurityDescriptor = NULL;
	Attr.bInheritHandle = true;

	// initialize process creation flags
	uint32 CreateFlags = NORMAL_PRIORITY_CLASS;

	if (bLaunchDetached)
	{
		CreateFlags |= DETACHED_PROCESS;
	}

	// initialize window flags
	uint32 dwFlags = 0;
	uint16 ShowWindowFlags = SW_HIDE;
	if (bLaunchReallyHidden)
	{
		dwFlags = STARTF_USESHOWWINDOW;
	}
	else if (bLaunchHidden)
	{
		dwFlags = STARTF_USESHOWWINDOW;
		ShowWindowFlags = SW_SHOWMINNOACTIVE;
	}

	// initialize startup info
	STARTUPINFO StartupInfo = {
		sizeof(STARTUPINFO),
		NULL, NULL, NULL,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)CW_USEDEFAULT,
		(::DWORD)0, (::DWORD)0, (::DWORD)0,
		(::DWORD)dwFlags,
		ShowWindowFlags,
		0, NULL, NULL, NULL, NULL
	};

	// create the child process
	//FString CommandLine = FString::Printf(TEXT("\"%s\" %s"), URL, Parms);
	string CommandLine;
	CommandLine.append("\"").append(Url).append("\" ").append(Params);

	if (!::CreateProcess(NULL, const_cast<TCHAR*>(UTF8ToWide(CommandLine).c_str()), &Attr, &Attr, true, (::DWORD)CreateFlags, NULL, NULL, &StartupInfo, &ProcInfo))
	{
	}
	else
	{
		HANDLE hProcess =
			OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
				PROCESS_VM_WRITE | PROCESS_VM_READ | SYNCHRONIZE,
				FALSE, ProcInfo.dwProcessId);

		wchar_t renderdocPath[MAX_PATH] = { 0 };
		GetModuleFileNameW(GetModuleHandleA("renderdoc.dll"), &renderdocPath[0], MAX_PATH - 1);
		BOOL isWow64 = FALSE;
		BOOL success = IsWow64Process(hProcess, &isWow64);  if (!success)
		{
			DWORD err = GetLastError();
			printf("Couldn't determine bitness of process, err: %08x", err);
			CloseHandle(hProcess);
			return;
		}
	}
}

int main()
{
	//ProcessLoader Loader("D:/GitUnreal/UnrealEngine-4.11/Engine/Binaries/Win64/UE4Editor.exe", "E:/UnrealProjects/LostVR411/XYVR410.uproject -game");
	HANDLE hModule = ::LoadLibrary(TEXT(HookCoreModule));
	LaunchAndInjectIntoProcess(
		"D:/GitUnreal/UnrealEngine-4.11/Engine/Binaries/Win64/UE4Editor.exe", 
		nullptr, 
		"E:/UnrealProjects/LostVR411/XYVR410.uproject -game", 
		nullptr, 
		false);

 	::system("PAUSE");
    return 0;
}

