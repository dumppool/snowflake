/*
* file LostCore-D3D11.h
*
* author luoxw
* date 2017/01/18
*
*
*/

#pragma once

#ifdef LOSTCORED3D11_EXPORTS
#define LOSTCORED3D11_API __declspec(dllexport)
#else
#define LOSTCORED3D11_API __declspec(dllimport)
#endif

// 此类是从 LostCore-D3D11.dll 导出的
class LOSTCORED3D11_API CLostCoreD3D11 {
public:
	CLostCoreD3D11(void);
	// TODO:  在此添加您的方法。
};

extern LOSTCORED3D11_API int nLostCoreD3D11;

LOSTCORED3D11_API int fnLostCoreD3D11(void);

namespace D3D11 {

	static const char* SModuleName = "LostCore-D3D11.dll";
	static HMODULE SModuleHandle = NULL; 

	static void StartupModule(const char* directory)
	{
		if (SModuleHandle != NULL)
		{
			return;
		}

		char dllPath[MAX_PATH];
		snprintf(dllPath, MAX_PATH, "%s%s", directory, SModuleName);
		SModuleHandle = LoadLibraryA(dllPath);

		if (SModuleHandle == NULL)
		{
			MessageBoxA(NULL, "failed to load dll", dllPath, 0);
			return;
		}
	}

	static void ShutdownModule()
	{
		if (SModuleHandle == NULL)
		{
			return;
		}

		FreeLibrary(SModuleHandle);
		SModuleHandle = NULL;
	}

	static HMODULE GetModule_LostCoreD3D11()
	{
		if (SModuleHandle == NULL)
		{
			StartupModule("");
		}

		return SModuleHandle;
	}

	extern "C" LOSTCORED3D11_API EReturnCode CreateRenderContext(LostCore::EContextID id, LostCore::IRenderContext** context);
	static TExportFuncWrapper<LostCore::EContextID, LostCore::IRenderContext**> WrappedCreateRenderContext("CreateRenderContext", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode DestroyRenderContext(LostCore::IRenderContext* context);
	static TExportFuncWrapper<LostCore::IRenderContext*> WrappedDestroyRenderContext("DestroyRenderContext", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode CreatePrimitiveGroup(LostCore::IPrimitiveGroup** pg);
	static TExportFuncWrapper<LostCore::IPrimitiveGroup**> WrappedCreatePrimitiveGroup("CreatePrimitiveGroup", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode DestroyPrimitiveGroup(LostCore::IPrimitiveGroup* pg);
	static TExportFuncWrapper<LostCore::IPrimitiveGroup*> WrappedDestroyPrimitiveGroup("DestroyPrimitiveGroup", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode CreateMaterial(LostCore::IMaterial** material);
	static TExportFuncWrapper<LostCore::IMaterial**> WrappedCreateMaterial("CreateMaterial", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode DestroyMaterial(LostCore::IMaterial* material);
	static TExportFuncWrapper<LostCore::IMaterial*> WrappedDestroyMaterial("DestroyMaterial", GetModule_LostCoreD3D11);
}
