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
#define LOSTCORED3D11_API extern "C" __declspec(dllexport)
#else
#define LOSTCORED3D11_API extern "C" __declspec(dllimport)
#endif

#ifdef GET_MODULE
#undef GET_MODULE
#undef EXPORT_API
#endif
#define GET_MODULE D3D11::GetModule_LostCoreD3D11
#define EXPORT_API LOSTCORED3D11_API

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

	EXPORT_WRAP_2_DCL(CreateRenderContext, LostCore::EContextID, LostCore::IRenderContext**);
	EXPORT_WRAP_1_DCL(DestroyRenderContext, LostCore::IRenderContext*);
	EXPORT_WRAP_1_DCL(CreatePrimitiveGroup, LostCore::IPrimitiveGroup**);
	EXPORT_WRAP_1_DCL(DestroyPrimitiveGroup, LostCore::IPrimitiveGroup*);
	EXPORT_WRAP_1_DCL(CreateConstantBuffer, LostCore::IConstantBuffer**);
	EXPORT_WRAP_1_DCL(DestroyConstantBuffer, LostCore::IConstantBuffer*);
	EXPORT_WRAP_1_DCL(CreateMaterial, LostCore::IMaterial**);
	EXPORT_WRAP_1_DCL(DestroyMaterial, LostCore::IMaterial*);
	EXPORT_WRAP_1_DCL(CreateGdiFont, LostCore::IFontInterface**);
	EXPORT_WRAP_1_DCL(DestroyGdiFont, LostCore::IFontInterface*);
}
