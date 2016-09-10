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

struct ProcessHelper
{
public:
	ProcessHelper();
	ProcessHelper(const vector<wstring>& Keys);

	void Reset();
	bool Upate();
	string ToString() const;

	void UpdateProcessSnapshot(DWORD ProcId);

protected:
	static const unsigned int SProcessCount = 1024u;
	vector<DWORD> ProcessIds;
	DWORD ProcessBytes;

	string Info;
	string Info2;
	vector<wstring> TargetProcessKeyNames;
};

ProcessHelper::ProcessHelper()
{
	Reset();
}

ProcessHelper::ProcessHelper(const vector<wstring>& Keys)
{
	Reset();
	TargetProcessKeyNames = Keys;
}

void ProcessHelper::Reset()
{
	ProcessBytes = 0;
	ProcessIds.resize(SProcessCount);
}

bool ProcessHelper::Upate()
{
	Reset();
	if (!::EnumProcesses(&ProcessIds[0], ProcessIds.size(), &ProcessBytes))
	{
		return false;
	}

	for (unsigned int i = 0; i < ProcessBytes/sizeof(DWORD); ++i)
	{
		DWORD Id = ProcessIds[i];

		// skip PID of 0
		if (Id == 0)
		{
			//continue;
		}

		UpdateProcessSnapshot(Id);
	}

	return true;
}

string ProcessHelper::ToString() const
{
	string Result;
	Result.append("Info:\n").append(Info).append("\nInfo2:\n").append(Info2);
	return Result;
}

void ProcessHelper::UpdateProcessSnapshot(DWORD ProcId)
{
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcId);
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
		if (Key.compare(ME32.szModule) == 0)
		{
			bFound = true;
			break;
		}
	}

	if (!bFound)
	{
		return;
	}

	byte* BaseAddr = (byte*)ME32.hModule;
	PIMAGE_DOS_HEADER Dosheader = (PIMAGE_DOS_HEADER)BaseAddr;
	HMODULE hMod = ::LoadLibrary(ME32.szExePath);
	if (Dosheader->e_magic != 0x5a4d)
	{
		printf("magic is 0x%4x not 0x%4x.\n", Dosheader->e_magic, 0x4a5d);
		::FreeLibrary(hMod);
		return;
	}

	char* PE00 = (char*)(BaseAddr + Dosheader->e_lfanew);
	PIMAGE_FILE_HEADER FileHeader = (PIMAGE_FILE_HEADER)(PE00 + 4);
	PIMAGE_OPTIONAL_HEADER OptHeader = (PIMAGE_OPTIONAL_HEADER)((BYTE*)FileHeader + sizeof(IMAGE_FILE_HEADER));
	DWORD IATOffset = OptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	IMAGE_IMPORT_DESCRIPTOR* ImportDesc = (IMAGE_IMPORT_DESCRIPTOR*)(BaseAddr + IATOffset);
	while (IATOffset && ImportDesc->FirstThunk)
	{
		const char* DllName = (const char*)(BaseAddr + ImportDesc->Name);
		IMAGE_THUNK_DATA* OrigFirst = (IMAGE_THUNK_DATA*)(BaseAddr + ImportDesc->OriginalFirstThunk);
		IMAGE_THUNK_DATA* First = (IMAGE_THUNK_DATA*)(BaseAddr + ImportDesc->FirstThunk);

		while (OrigFirst->u1.AddressOfData)
		{
			void** IATEntry = (void**)(&First->u1.AddressOfData);
		}
	}
}

int main()
{
	vector<wstring> Keys;
	Keys.push_back(L"UE4Editor.exe");
	ProcessHelper Helper(Keys);
	if (Helper.Upate())
	{
		cout << Helper.ToString() << endl;
	}

	::system("PAUSE");
    return 0;
}

