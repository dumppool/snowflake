/*
* file LostCore-D3D11.h
*
* author luoxw
* date 2017/01/18
*
*
*/

#pragma once

#include "MaterialInterface.h"
#include "FontInterface.h"
#include "TextureInterface.h"
#include "RenderContextInterface.h"
#include "PrimitiveGroupInterface.h"

#ifdef LOSTCORED3D11_EXPORTS
#define LOSTCORED3D11_API extern "C" __declspec(dllexport)
#else
#define LOSTCORED3D11_API extern "C" __declspec(dllimport)
#endif

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

	LOSTCORED3D11_API int32 CreateRenderContext(LostCore::EContextID id, LostCore::IRenderContext** context);
	static TExportFuncWrapper<LostCore::EContextID, LostCore::IRenderContext**> WrappedCreateRenderContext("CreateRenderContext", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyRenderContext(LostCore::IRenderContext* context);
	static TExportFuncWrapper<LostCore::IRenderContext*> WrappedDestroyRenderContext("DestroyRenderContext", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 CreatePrimitiveGroup(LostCore::IPrimitiveGroup** pg);
	static TExportFuncWrapper<LostCore::IPrimitiveGroup**> WrappedCreatePrimitiveGroup("CreatePrimitiveGroup", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyPrimitiveGroup(LostCore::IPrimitiveGroup* pg);
	static TExportFuncWrapper<LostCore::IPrimitiveGroup*> WrappedDestroyPrimitiveGroup("DestroyPrimitiveGroup", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 CreateMaterial_Segment(LostCore::IMaterial** material);
	static TExportFuncWrapper<LostCore::IMaterial**> WrappedCreateMaterial_Segment("CreateMaterial_Segment", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyMaterial_Segment(LostCore::IMaterial* material);
	static TExportFuncWrapper<LostCore::IMaterial*> WrappedDestroyMaterial_Segment("DestroyMaterial_Segment", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 CreateMaterial_SceneObject(LostCore::IMaterial** material);
	static TExportFuncWrapper<LostCore::IMaterial**> WrappedCreateMaterial_SceneObject("CreateMaterial_SceneObject", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyMaterial_SceneObject(LostCore::IMaterial* material);
	static TExportFuncWrapper<LostCore::IMaterial*> WrappedDestroyMaterial_SceneObject("DestroyMaterial_SceneObject", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 CreateMaterial_SceneObjectSkinned(LostCore::IMaterial** material);
	static TExportFuncWrapper<LostCore::IMaterial**> WrappedCreateMaterial_SceneObjectSkinned("CreateMaterial_SceneObjectSkinned", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyMaterial_SceneObjectSkinned(LostCore::IMaterial* material);
	static TExportFuncWrapper<LostCore::IMaterial*> WrappedDestroyMaterial_SceneObjectSkinned("DestroyMaterial_SceneObjectSkinned", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 CreateMaterial_UIObject(LostCore::IMaterial** material);
	static TExportFuncWrapper<LostCore::IMaterial**> WrappedCreateMaterial_UIObject("CreateMaterial_UIObject", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyMaterial_UIObject(LostCore::IMaterial* material);
	static TExportFuncWrapper<LostCore::IMaterial*> WrappedDestroyMaterial_UIObject("DestroyMaterial_UIObject", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 CreateGdiFont(LostCore::IFontInterface** font);
	static TExportFuncWrapper<LostCore::IFontInterface**> WrappedCreateGdiFont("CreateGdiFont", GetModule_LostCoreD3D11);

	LOSTCORED3D11_API int32 DestroyGdiFont(LostCore::IFontInterface* font);
	static TExportFuncWrapper<LostCore::IFontInterface*> WrappedDestroyGdiFont("DestroyGdiFont", GetModule_LostCoreD3D11);
}
