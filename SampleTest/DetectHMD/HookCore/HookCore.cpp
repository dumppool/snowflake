#include "HookCore.h"

#include <stdio.h>
#include <d3d11.h>
#include "mhook-lib/mhook.h"
#pragma comment(lib, "d3d11.lib")

//#include <TlHelp32.h>
//#include <stdint.h>
//#include <algorithm>
//#include <vector>
//#include <map>
//
//#include "string_utils.h"

//using namespace std;

static PFN_D3D11_CREATE_DEVICE D3D11CreateDevice_OriginalPtr = nullptr;
static HRESULT WINAPI D3D11CreateDevice_Hook(
	_In_opt_ IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
	UINT SDKVersion, _COM_Outptr_opt_ ID3D11Device** ppDevice,
	_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
	_COM_Outptr_opt_ ID3D11DeviceContext** ppImmediateContext)
{
	//printf("D3D11CreateDevice_hook get called.\n");
	HRESULT ret = S_FALSE;

	if (D3D11CreateDevice_OriginalPtr)
	{
		::MessageBoxA(NULL, "SUCEED", "D3D11CreateDevice_OriginalPtr", 0);
		ret = D3D11CreateDevice_OriginalPtr(pAdapter, DriverType, Software, Flags, pFeatureLevels,
			FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
	}
	else
	{
		::MessageBoxA(NULL, "FAILED", "D3D11CreateDevice_OriginalPtr", 0);
	}

	return ret;
}

static PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN D3D11CreateDeviceAndSwapChain_OriginalPtr = nullptr;
static HRESULT WINAPI D3D11CreateDeviceAndSwapChain_Hook(__in_opt IDXGIAdapter *pAdapter, D3D_DRIVER_TYPE DriverType,
	HMODULE Software, UINT Flags,
	__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL *pFeatureLevels,
	UINT FeatureLevels, UINT SDKVersion,
	__in_opt CONST DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
	__out_opt IDXGISwapChain **ppSwapChain, __out_opt ID3D11Device **ppDevice,
	__out_opt D3D_FEATURE_LEVEL *pFeatureLevel,
	__out_opt ID3D11DeviceContext **ppImmediateContext)
{
	//printf("D3D11CreateDeviceAndSwapChain_Hook get called.\n");
	//::MessageBoxA(NULL, "D3D11CreateDeviceAndSwapChain_Hook", "", 0);
	if (D3D11CreateDeviceAndSwapChain_OriginalPtr)
	{
		::MessageBoxA(NULL, "SUCCEED", "D3D11CreateDeviceAndSwapChain_Hook", 0);
		return D3D11CreateDeviceAndSwapChain_OriginalPtr(pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels,
			SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext);
	}
	else
	{
		::MessageBoxA(NULL, "FAILED", "D3D11CreateDeviceAndSwapChain_Hook", 0);
		return S_FALSE;
	}
}

/*
struct FunctionTarget
{
	string Name;
	void* Orig;
	void* Hook;

	FunctionTarget(const string& InName, void* InHook) : Name(InName), Hook(InHook) {}
};

static void HookD3D11()
{
	map<string, vector<FunctionTarget>> HookTargets;
	vector<FunctionTarget> D3D11Func;
	D3D11Func.push_back(FunctionTarget("D3D11CreateDevice", D3D11CreateDevice_Hook));
	D3D11Func.push_back(FunctionTarget("D3D11CreateDeviceAndSwapChain", D3D11CreateDeviceAndSwapChain_Hook));
	HookTargets["d3d11.dll"] = D3D11Func;

	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;

	// up to 10 retries
	for (int i = 0; i < 10; i++)
	{
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

		if (hModuleSnap == INVALID_HANDLE_VALUE)
		{
			DWORD err = GetLastError();

			RDCWARN("CreateToolhelp32Snapshot() -> 0x%08x", err);

			// retry if error is ERROR_BAD_LENGTH
			if (err == ERROR_BAD_LENGTH)
				continue;
		}

		// didn't retry, or succeeded
		break;
	}

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		RDCERR("Couldn't create toolhelp dump of modules in process");
		return;
	}

	MODULEENTRY32 me32;
	RDCEraseEl(me32);
	me32.dwSize = sizeof(MODULEENTRY32);

	BOOL success = Module32First(hModuleSnap, &me32);

	if (success == FALSE)
	{
		DWORD err = GetLastError();

		RDCERR("Couldn't get first module in process: 0x%08x", err);
		CloseHandle(hModuleSnap);
		return;
	}

	uintptr_t ret = 0;

	do
	{
		const char* modName = me32.szModule;
		char lowername[512];
		HMODULE module = me32.hModule;
		{
			size_t i = 0;
			while (modName[i])
			{
				lowername[i] = (char)tolower(modName[i]);
				i++;
			}
			lowername[i] = 0;
		}

		// fraps seems to non-safely modify the assembly around the hook function, if
		// we modify its import descriptors it leads to a crash as it hooks OUR functions.
		// instead, skip modifying the import descriptors, it will hook the 'real' d3d functions
		// and we can call them and have fraps + renderdoc playing nicely together.
		// we also exclude some other overlay renderers here, such as steam's
		//
		// Also we exclude ourselves here - just in case the application has already loaded
		// renderdoc.dll, or tries to load it.
		if (strstr(lowername, "fraps") || strstr(lowername, "gameoverlayrenderer") ||
			strstr(lowername, "renderdoc.dll") == lowername)
			return;

		// for safety (and because we don't need to), ignore these modules
		if (!_stricmp(modName, "kernel32.dll") || !_stricmp(modName, "powrprof.dll") ||
			!_stricmp(modName, "opengl32.dll") || !_stricmp(modName, "gdi32.dll") ||
			strstr(lowername, "msvcr") == lowername || strstr(lowername, "msvcp") == lowername ||
			strstr(lowername, "nv-vk") == lowername || strstr(lowername, "amdvlk") == lowername ||
			strstr(lowername, "igvk") == lowername || strstr(lowername, "nvopencl") == lowername)
			return;

		byte *baseAddress = (byte *)module;

		// the module could have been unloaded after our toolhelp snapshot, especially if we spent a
		// long time
		// dealing with a previous module (like adding our hooks).
		wchar_t modpath[1024] = { 0 };
		GetModuleFileNameW(module, modpath, 1023);
		if (modpath[0] == 0)
			return;
		// increment the module reference count, so it doesn't disappear while we're processing it
		// there's a very small race condition here between if GetModuleFileName returns, the module is
		// unloaded then we load it again. The only way around that is inserting very scary locks
		// between here
		// and FreeLibrary that I want to avoid. Worst case, we load a dll, hook it, then unload it
		// again.
		HMODULE refcountModHandle = LoadLibraryW(modpath);

		PIMAGE_DOS_HEADER dosheader = (PIMAGE_DOS_HEADER)baseAddress;

		if (dosheader->e_magic != 0x5a4d)
		{
			RDCDEBUG("Ignoring module %s, since magic is 0x%04x not 0x%04x", modName,
				(uint32_t)dosheader->e_magic, 0x5a4dU);
			FreeLibrary(refcountModHandle);
			return;
		}

		char *PE00 = (char *)(baseAddress + dosheader->e_lfanew);
		PIMAGE_FILE_HEADER fileHeader = (PIMAGE_FILE_HEADER)(PE00 + 4);
		PIMAGE_OPTIONAL_HEADER optHeader =
			(PIMAGE_OPTIONAL_HEADER)((BYTE *)fileHeader + sizeof(IMAGE_FILE_HEADER));

		DWORD iatOffset = optHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

		IMAGE_IMPORT_DESCRIPTOR *importDesc = (IMAGE_IMPORT_DESCRIPTOR *)(baseAddress + iatOffset);

		while (iatOffset && importDesc->FirstThunk)
		{
			const char *dllName = (const char *)(baseAddress + importDesc->Name);
			vector<FunctionTarget> HookFuncs;

			for (auto Target : HookTargets)
			{
				if (!strlower(Target.first).compare(strlower(dllName)))
				{
					HookFuncs = Target.second;
				}
			}
			
			if (HookFuncs.size() > 0 && importDesc->OriginalFirstThunk > 0 && importDesc->FirstThunk > 0)
			{
				IMAGE_THUNK_DATA *origFirst =
					(IMAGE_THUNK_DATA *)(baseAddress + importDesc->OriginalFirstThunk);
				IMAGE_THUNK_DATA *first = (IMAGE_THUNK_DATA *)(baseAddress + importDesc->FirstThunk);

				while (origFirst->u1.AddressOfData)
				{
					void **IATentry = (void **)&first->u1.AddressOfData;
#ifdef WIN64
					if (IMAGE_SNAP_BY_ORDINAL64(origFirst->u1.AddressOfData))
#else
					if (IMAGE_SNAP_BY_ORDINAL32(origFirst->u1.AddressOfData))
#endif
					{
					//	// low bits of origFirst->u1.AddressOfData contain an ordinal
					//	WORD ordinal = IMAGE_ORDINAL64(origFirst->u1.AddressOfData);

					//	if (!hookset->OrdinalNames.empty())
					//	{
					//		if (ordinal >= hookset->OrdinalBase)
					//		{
					//			// rebase into OrdinalNames index
					//			DWORD nameIndex = ordinal - hookset->OrdinalBase;

					//			// it's perfectly valid to have more functions than names, we only
					//			// list those with names - so ignore any others
					//			if (nameIndex < hookset->OrdinalNames.size())
					//			{
					//				const char *importName = (const char *)hookset->OrdinalNames[nameIndex].c_str();

					//				auto found =
					//					std::lower_bound(hookset->FunctionHooks.begin(), hookset->FunctionHooks.end(),
					//						importName, hook_find());

					//				if (found != hookset->FunctionHooks.end() &&
					//					!strcmp(found->function.c_str(), importName) && found->excludeModule != module)
					//				{
					//					SCOPED_LOCK(lock);
					//					bool applied = found->ApplyHook(IATentry);

					//					if (!applied)
					//					{
					//						FreeLibrary(refcountModHandle);
					//						return;
					//					}
					//				}
					//			}
					//		}
					//		else
					//		{
					//			RDCERR("Import ordinal is below ordinal base in %s importing module %s", modName,
					//				dllName);
					//		}
					//	}
					//	else
					//	{
					//		// the very first time we try to apply hooks, we might apply them to a module
					//		// before we've looked up the ordinal names for the one it's linking against.
					//		// Subsequent times we're only loading one new module - and since it can't
					//		// link to itself we will have all ordinal names loaded.
					//		//
					//		// Setting this flag causes us to do a second pass right at the start
					//		missedOrdinals = true;
					//	}

					//	// continue
					//	origFirst++;
					//	first++;
					//	continue;
					}

					IMAGE_IMPORT_BY_NAME *import =
						(IMAGE_IMPORT_BY_NAME *)(baseAddress + origFirst->u1.AddressOfData);

					const char *importName = (const char *)import->Name;
					auto found = std::lower_bound(hookset->FunctionHooks.begin(),
						hookset->FunctionHooks.end(), importName, hook_find());

					if (found != hookset->FunctionHooks.end() &&
						!strcmp(found->function.c_str(), importName) && found->excludeModule != module)
					{
						//SCOPED_LOCK(lock);
						bool applied = found->ApplyHook(IATentry);

						if (!applied)
						{
							FreeLibrary(refcountModHandle);
							return;
						}
					}

					origFirst++;
					first++;
				}
			}

			importDesc++;
		}

		FreeLibrary(refcountModHandle);
	} while (ret == 0 && Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);
}*/

void InstallHook(int* Result)
{
	//int Result[1] = { 0 };
	printf("SetHook_D3D11...\n");

	PFN_D3D11_CREATE_DEVICE D3D11CreateDevice_OriginalPtr = (PFN_D3D11_CREATE_DEVICE)::GetProcAddress(::GetModuleHandle(TEXT("d3d11.dll")), "D3D11CreateDevice");
	if (Mhook_SetHook((PVOID*)&D3D11CreateDevice_OriginalPtr, D3D11CreateDevice_Hook))
	{
		::MessageBoxA(NULL, "Hook D3D11CreateDevice successfully", "D3D11CreateDevice", 0);
		printf("Hook D3D11CreateDevice successfully.\n");
	}
	else
	{
		::MessageBoxA(NULL, "Hook D3D11CreateDevice failed", "D3D11CreateDevice", 0);
		printf("Hook D3D11CreateDevice failed.\n");
		*Result = 1;
		return;
	}

	PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN D3D11CreateDeviceAndSwapChain_OriginalPtr = (PFN_D3D11_CREATE_DEVICE_AND_SWAP_CHAIN)::GetProcAddress(::GetModuleHandle(TEXT("d3d11.dll")), "D3D11CreateDeviceAndSwapChain");
	if (Mhook_SetHook((PVOID*)&D3D11CreateDeviceAndSwapChain_OriginalPtr, D3D11CreateDeviceAndSwapChain_Hook))
	{
		::MessageBoxA(NULL, "Hook D3D11CreateDeviceAndSwapChain successfully", "D3D11CreateDeviceAndSwapChain", 0);
		printf("Hook D3D11CreateDeviceAndSwapChain successfully.\n");
	}
	else
	{
		::MessageBoxA(NULL, "Hook D3D11CreateDeviceAndSwapChain failed", "D3D11CreateDeviceAndSwapChain", 0);
		printf("Hook D3D11CreateDeviceAndSwapChain failed.\n");
		*Result = 2;
		return;
	}

	*Result = -1;
	return;
}

void UninstallHook(int* p)
{

}