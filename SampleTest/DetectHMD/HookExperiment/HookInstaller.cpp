#include "HookInstaller.h"
#include "string_utils.h"

void InjectDLL(HANDLE hProcess, wstring libName)
{
	wchar_t dllPath[MAX_PATH + 1] = { 0 };
	wcscpy_s(dllPath, libName.c_str());

	static HMODULE kernel32 = GetModuleHandleA("kernel32.dll");

	if (kernel32 == NULL)
	{
		LVLogErr("Couldn't get handle for kernel32.dll");
		return;
	}

	void *remoteMem =
		VirtualAllocEx(hProcess, NULL, sizeof(dllPath), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (remoteMem)
	{
		printf("\nready to load %s\n", HookCoreModule);
		//system("PAUSE");
		WriteProcessMemory(hProcess, remoteMem, (void *)dllPath, sizeof(dllPath), NULL);

		HANDLE hThread = CreateRemoteThread(
			hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(kernel32, "LoadLibraryW"),
			remoteMem, 0, NULL);
		if (hThread)
		{
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
		}
		VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
	}
	else
	{
		LVLogErr("Couldn't allocate remote memory for DLL '%ls'", libName.c_str());
	}
}

uintptr_t FindRemoteDLL(DWORD pid, wstring libName)
{
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;

	libName = strlower(libName);

	// up to 10 retries
	for (int i = 0; i < 10; i++)
	{
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			DWORD err = GetLastError();

			LVLogWarn("CreateToolhelp32Snapshot(%u) -> 0x%08x", pid, err);

			// retry if error is ERROR_BAD_LENGTH
			if (err == ERROR_BAD_LENGTH)
				continue;
		}

		// didn't retry, or succeeded
		break;
	}

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		LVLogErr("Couldn't create toolhelp dump of modules in process %u", pid);
		return 0;
	}

	MODULEENTRY32 me32;
	LVEraseEl(me32);
	me32.dwSize = sizeof(MODULEENTRY32);

	BOOL success = Module32First(hModuleSnap, &me32);

	if (success == FALSE)
	{
		DWORD err = GetLastError();

		LVLogErr("Couldn't get first module in process %u: 0x%08x", pid, err);
		CloseHandle(hModuleSnap);
		return 0;
	}

	uintptr_t ret = 0;

	int numModules = 0;

	do
	{
		wchar_t modnameLower[MAX_MODULE_NAME32 + 1];
		LVEraseEl(modnameLower);
		wcsncpy_s(modnameLower, me32.szModule, MAX_MODULE_NAME32);

		wchar_t *wc = &modnameLower[0];
		while (*wc)
		{
			*wc = towlower(*wc);
			wc++;
		}

		numModules++;

		if (wcsstr(modnameLower, libName.c_str()))
		{
			ret = (uintptr_t)me32.modBaseAddr;
		}
	} while (ret == 0 && Module32Next(hModuleSnap, &me32));

	if (ret == 0)
	{
		LVLogErr("Couldn't find module '%ls' among %d modules", libName.c_str(), numModules);
	}

	CloseHandle(hModuleSnap);

	return ret;
}

void InjectFunctionCall(HANDLE hProcess, uintptr_t renderdoc_remote, const char *funcName,
	void *data, const size_t dataLen)
{
	if (dataLen == 0)
	{
		LVLogErr("Invalid function call injection attempt");
		return;
	}

	LVLogDebug("Injecting call to %s", funcName);

	HMODULE renderdoc_local = GetModuleHandleA(HookCoreModule);

	uintptr_t func_local = (uintptr_t)GetProcAddress(renderdoc_local, funcName);

	// we've found SetCaptureOptions in our local instance of the module, now calculate the offset and
	// so get the function
	// in the remote module (which might be loaded at a different base address
	uintptr_t func_remote = func_local + renderdoc_remote - (uintptr_t)renderdoc_local;

	void *remoteMem = VirtualAllocEx(hProcess, NULL, dataLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	SIZE_T numWritten;
	WriteProcessMemory(hProcess, remoteMem, data, dataLen, &numWritten);

	HANDLE hThread =
		CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)func_remote, remoteMem, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	ReadProcessMemory(hProcess, remoteMem, data, dataLen, &numWritten);

	CloseHandle(hThread);
	VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
}

static PROCESS_INFORMATION RunProcess(const char *app, const char *workingDir, const char *cmdLine)
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	SECURITY_ATTRIBUTES pSec;
	SECURITY_ATTRIBUTES tSec;

	LVEraseEl(pi);
	LVEraseEl(si);
	LVEraseEl(pSec);
	LVEraseEl(tSec);

	pSec.nLength = sizeof(pSec);
	tSec.nLength = sizeof(tSec);

	wstring workdir = L"";

	if (workingDir != NULL && workingDir[0] != 0)
		workdir = UTF8ToWide(string(workingDir));
	else
		workdir = UTF8ToWide(dirname(string(app)));

	wchar_t *paramsAlloc = NULL;

	wstring wapp = UTF8ToWide(string(app));

	// CreateProcessW can modify the params, need space.
	size_t len = wapp.length() + 10;

	wstring wcmd = L"";

	if (cmdLine != NULL && cmdLine[0] != 0)
	{
		wcmd = UTF8ToWide(string(cmdLine));
		len += wcmd.length();
	}

	paramsAlloc = new wchar_t[len];

	LVEraseMem(paramsAlloc, len * sizeof(wchar_t));

	wcscpy_s(paramsAlloc, len, L"\"");
	wcscat_s(paramsAlloc, len, wapp.c_str());
	wcscat_s(paramsAlloc, len, L"\"");

	if (cmdLine != NULL && cmdLine[0] != 0)
	{
		wcscat_s(paramsAlloc, len, L" ");
		wcscat_s(paramsAlloc, len, wcmd.c_str());
	}

	BOOL retValue = CreateProcessW(NULL, paramsAlloc, &pSec, &tSec, false,
		CREATE_SUSPENDED | CREATE_UNICODE_ENVIRONMENT, NULL,
		workdir.c_str(), &si, &pi);

	SAFE_DELETE_ARRAY(paramsAlloc);

	if (!retValue)
	{
		LVLogErr("Process %s could not be loaded.", app);
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		LVEraseEl(pi);
	}

	return pi;
}

uint32_t InjectIntoProcess(uint32_t pid, const char *logfile, bool waitForExit)
{
	wstring wlogfile = logfile == NULL ? L"" : UTF8ToWide(logfile);

	HANDLE hProcess = 
		//OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ | SYNCHRONIZE,
			FALSE, pid);

	LVLog("Injecting HookCore into process %lu", pid);

	wchar_t renderdocPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(GetModuleHandleA(HookCoreModule), &renderdocPath[0], MAX_PATH - 1);

	BOOL isWow64 = FALSE;
	BOOL success = IsWow64Process(hProcess, &isWow64);

	if (!success)
	{
		DWORD err = GetLastError();
		LVLogErr("Couldn't determine bitness of process, err: %08x", err);
		CloseHandle(hProcess);
		return 0;
	}

#if !defined(WIN64)
	BOOL selfWow64 = FALSE;

	HANDLE hSelfProcess = GetCurrentProcess();

	success = IsWow64Process(hSelfProcess, &selfWow64);

	CloseHandle(hSelfProcess);

	if (!success)
	{
		DWORD err = GetLastError();
		LVLogErr("Couldn't determine bitness of self, err: %08x", err);
		return 0;
	}

	if (selfWow64 && !isWow64)
	{
		LVLogErr("Can't capture x64 process with x86 version");
		CloseHandle(hProcess);
		return 0;
	}
#else
	// farm off to x86 version
	if (isWow64)
	{
		wchar_t *slash = wcsrchr(renderdocPath, L'\\');

		if (slash)
			*slash = 0;

		wcscat_s(renderdocPath, L"\\x86\\renderdoccmd.exe");

		PROCESS_INFORMATION pi;
		STARTUPINFO si;
		SECURITY_ATTRIBUTES pSec;
		SECURITY_ATTRIBUTES tSec;

		LVEraseEl(pi);
		LVEraseEl(si);
		LVEraseEl(pSec);
		LVEraseEl(tSec);

		pSec.nLength = sizeof(pSec);
		tSec.nLength = sizeof(tSec);

		wchar_t *paramsAlloc = new wchar_t[2048];

		// serialise to string with two chars per byte
		string optstr;
		{
			optstr.reserve(sizeof(CaptureOptions) * 2 + 1);
			byte *b = (byte *)opts;
			for (size_t i = 0; i < sizeof(CaptureOptions); i++)
			{
				optstr.push_back(char('a' + ((b[i] >> 4) & 0xf)));
				optstr.push_back(char('a' + ((b[i]) & 0xf)));
			}
		}

		_snwprintf_s(paramsAlloc, 2047, 2047,
			L"\"%ls\" cap32for64 --pid=%d --log=\"%ls\" --capopts=\"%hs\"", renderdocPath, pid,
			wlogfile.c_str(), optstr.c_str());

		paramsAlloc[2047] = 0;

		BOOL retValue = CreateProcessW(NULL, paramsAlloc, &pSec, &tSec, false, CREATE_SUSPENDED, NULL,
			NULL, &si, &pi);

		SAFE_DELETE_ARRAY(paramsAlloc);

		if (!retValue)
		{
			LVLogErr("Can't spawn x86 renderdoccmd - missing files?");
			return 0;
		}

		ResumeThread(pi.hThread);
		WaitForSingleObject(pi.hThread, INFINITE);
		CloseHandle(pi.hThread);

		DWORD exitCode = 0;
		GetExitCodeProcess(pi.hProcess, &exitCode);
		CloseHandle(pi.hProcess);

		if (waitForExit)
			WaitForSingleObject(hProcess, INFINITE);

		CloseHandle(hProcess);

		return (uint32_t)exitCode;
	}
#endif

	InjectDLL(hProcess, renderdocPath);

	uintptr_t loc = FindRemoteDLL(pid, TEXT(HookCoreModule));

	uint32_t controlident = 1;
	int Result = 0;

	if (loc == 0)
	{
		LVLogErr("Can't locate %s in remote PID %d", HookCoreModule, pid);
	}
	else
	{
		char Params[] = "install";
		InjectFunctionCall(hProcess, loc, "InstallHook", &Result, sizeof(Result));
	}

	if (waitForExit)
		WaitForSingleObject(hProcess, INFINITE);

	CloseHandle(hProcess);

	return controlident;
}

void InjectFinished(DWORD pid)
{
	HANDLE hProcess =
		//OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION |
			PROCESS_VM_WRITE | PROCESS_VM_READ | SYNCHRONIZE,
			FALSE, pid);

	uintptr_t loc = FindRemoteDLL(pid, TEXT(HookCoreModule));
	int Result = 0;
	if (loc == 0)
	{
		LVLogErr("InjectFinished: Can't locate %s in remote PID %d", HookCoreModule, pid);
	}
	else
	{
		InjectFunctionCall(hProcess, loc, "UninstallHook", &Result, sizeof(Result));
	}

	CloseHandle(hProcess);
}

uint32_t LaunchAndInjectIntoProcess(const char *app, const char *workingDir, const char *cmdLine, const char *logfile, bool waitForExit, DWORD& pid)
{
	PROCESS_INFORMATION pi = RunProcess(app, workingDir, cmdLine);
	pid = pi.dwProcessId;

	if (pi.dwProcessId == 0)
		return 0;

	uint32_t ret = InjectIntoProcess(pi.dwProcessId, logfile, false);

	if (ret == 0)
	{
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return 0;
	}

	// done with it
	CloseHandle(pi.hProcess);

	ResumeThread(pi.hThread);

	if (waitForExit)
		WaitForSingleObject(pi.hThread, INFINITE);

	CloseHandle(pi.hThread);

	return ret;
}