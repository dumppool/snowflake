// HookExperiment.cpp : 定义控制台应用程序的入口点。
//

#include <Windows.h>

#include <stdio.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <iostream>

#include <TlHelp32.h>
#include <Psapi.h>

using namespace std;

typedef unsigned short uint16;
typedef unsigned int uint32;

wstring UTF8ToWide(const string &utf8) 
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
string WideToUTF8(const wstring &wide) {
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

struct MoudleHelper
{
public:
};

struct ProcessLoader
{
public:
	ProcessLoader(const char* Url, const char* Params, bool bLauncheDetached = false, bool bLaunchHidden = false, bool bLaunchReallyHidden = false);

	void Reset();
	void Update();
	string ToString() const;

	void Process();
	void ProcessModule(MODULEENTRY32 InME);

protected:
	static const unsigned int SProcessCount = 1024u;
	vector<DWORD> ProcessIds;
	DWORD ProcessBytes;

	PROCESS_INFORMATION ProcInfo;

	string Info;
	vector<string> TargetProcessKeyNames;
	vector<string> ModuleNamesOfInterest;
};

ProcessLoader::ProcessLoader(const char* Url, const char* Params, bool bLaunchDetached, bool bLaunchHidden, bool bLaunchReallyHidden)
{
	Reset();

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
		Process();
	}
}

void ProcessLoader::Reset()
{
	ProcessBytes = 0;
	ProcessIds.resize(SProcessCount);
}

void ProcessLoader::Update()
{
	//Reset();
	do
	{
		::Sleep(1);

		ProcessIds.clear();
		ProcessBytes = 0;
		if (!::EnumProcesses(&ProcessIds[0], ProcessIds.size(), &ProcessBytes))
		{
			continue;
		}

		for (unsigned int i = 0; i < ProcessBytes / sizeof(DWORD); ++i)
		{
			DWORD Id = ProcessIds[i];
			Process(Id);
		}
	} while(1)

	//return true;
}

string ProcessLoader::ToString() const
{
	return Info;
}

void ProcessLoader::Process()
{
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcInfo.dwProcessId);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		DWORD ErrCode = ::GetLastError();
		if (ErrCode != ERROR_ACCESS_DENIED && 1)
		{
			printf("failed to get snapshot, reason: 0x%8x\n", ErrCode);
		}
		return;
	}

	MODULEENTRY32 ME32;
	::memset(&ME32, sizeof(ME32), 0);
	ME32.dwSize = sizeof(MODULEENTRY32);
	if (!::Module32First(hSnapshot, &ME32))
	{
		DWORD ErrCode = ::GetLastError();
		printf("failed to get module entry, reason: 0x%8x\n", ErrCode);
		return;
	}

	bool bFound = false;
	for (auto& Key : TargetProcessKeyNames)
	{
		if (UTF8ToWide(Key).compare(ME32.szModule) == 0)
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		return;
	}

	do
	{
		ProcessModule(ME32);
	} while (::Module32Next(hSnapshot, &ME32));
}

void ProcessLoader::ProcessModule(MODULEENTRY32 ME32)
{
	if (!_stricmp(WideToUTF8(ME32.szModule).c_str(), "kernel32.dll") || !_stricmp(WideToUTF8(ME32.szModule).c_str(), "powrprof.dll") ||
		!_stricmp(WideToUTF8(ME32.szModule).c_str(), "opengl32.dll") || !_stricmp(WideToUTF8(ME32.szModule).c_str(), "gdi32.dll")
		|| !_stricmp(WideToUTF8(ME32.szModule).c_str(), "KernelBase.dll"))
		return;

	string ModuleInfo;
	byte* BaseAddr = (byte*)ME32.hModule;

	wchar_t modpath[1024] = { 0 };
	::GetModuleFileName(ME32.hModule, modpath, 1023);
	if (modpath[0] == 0)
	{
		printf("skip: %ls\n", ME32.szModule);
		return;
	}
	else
	{
		printf("... : %ls\n", ME32.szModule);
	}

	PIMAGE_DOS_HEADER Dosheader = (PIMAGE_DOS_HEADER)BaseAddr;
	HMODULE hMod = ::LoadLibrary(ME32.szExePath);
	if (Dosheader->e_magic != 0x5a4d)
	{
		printf("magic is 0x%4x not 0x%4x.\n", Dosheader->e_magic, 0x4a5d);
		::FreeLibrary(hMod);
		return;
	}

	ModuleInfo.append(WideToUTF8(ME32.szModule)).append(" :\n");

	char* PE00 = (char*)(BaseAddr + Dosheader->e_lfanew);
	PIMAGE_FILE_HEADER FileHeader = (PIMAGE_FILE_HEADER)(PE00 + 4);
	PIMAGE_OPTIONAL_HEADER OptHeader = (PIMAGE_OPTIONAL_HEADER)((BYTE*)FileHeader + sizeof(IMAGE_FILE_HEADER));
	DWORD IATOffset = OptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	IMAGE_IMPORT_DESCRIPTOR* ImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(BaseAddr + IATOffset);
	while (IATOffset && ImportDesc->FirstThunk)
	{
		const char* DllName = (const char*)(BaseAddr + ImportDesc->Name);
		ModuleInfo.append("\tdll name: ").append(DllName).append("\n");
		IMAGE_THUNK_DATA* OrigFirst = (IMAGE_THUNK_DATA*)(BaseAddr + ImportDesc->OriginalFirstThunk);
		IMAGE_THUNK_DATA* First = (IMAGE_THUNK_DATA*)(BaseAddr + ImportDesc->FirstThunk);

		while (OrigFirst->u1.AddressOfData)
		{
			void** IATEntry = (void**)(&First->u1.AddressOfData);

#ifdef WIN64
			if (IMAGE_SNAP_BY_ORDINAL64(OrigFirst->u1.AddressOfData))
#else
			if (IMAGE_SNAP_BY_ORDINAL32(OrigFirst->u1.AddressOfData))
#endif
			{
				WORD Ordinal = IMAGE_ORDINAL64(OrigFirst->u1.AddressOfData);
				printf("\tordinal: %lu\n", Ordinal);
			}
			else
			{
				IMAGE_IMPORT_BY_NAME* Import = (IMAGE_IMPORT_BY_NAME*)(BaseAddr + OrigFirst->u1.AddressOfData);
				const char* ImportName = (const char*)Import->Name;
				//ModuleInfo.append("\t\t name: ").append(ImportName).append("\n");
				//cout << "name: " << ImportName << endl;
			}

			++OrigFirst;
			++First;
		}

		++ImportDesc;
	}
}

int main()
{
	ProcessLoader Loader("D:/GitUnreal/UnrealEngine-4.11/Engine/Binaries/Win64/UE4Editor.exe", "E:/UnrealProjects/LostVR411/XYVR410.uproject -game");
	cout << Loader.ToString() << endl;
	::system("PAUSE");
    return 0;
}

