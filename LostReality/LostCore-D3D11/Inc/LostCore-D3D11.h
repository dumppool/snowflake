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

	//enum class EDeviceType : uint8
	//{
	//	Undefined,
	//	DXGI0,
	//	DXGI1,
	//	DXGI2,
	//};

	//extern "C" LOSTCORED3D11_API EReturnCode CreateDevice(
	//	D3D11::EDeviceType type,
	//	ID3D11Device** ppDevice,
	//	ID3D11DeviceContext** ppContext);
	//static TExportFuncWrapper<
	//	D3D11::EDeviceType,
	//	ID3D11Device**,
	//	ID3D11DeviceContext**> WrappedCreateDevice("CreateDevice", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateSwapChain(
	//	ID3D11Device* d3dDevice,
	//	HWND wnd,
	//	bool bWindowed,
	//	UINT width,
	//	UINT height,
	//	IDXGISwapChain** ppSwapChain);
	//static TExportFuncWrapper<
	//	ID3D11Device*,
	//	HWND,
	//	bool,
	//	UINT,
	//	UINT,
	//	IDXGISwapChain**> WrappedCreateSwapChain("CreateSwapChain", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CompileShader(LPCWSTR file, LPCSTR entry, LPCSTR target, ID3DBlob ** blob);
	//static TExportFuncWrapper<LPCWSTR, LPCSTR, LPCSTR, ID3DBlob**> WrappedCompileShader("CompileShader", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode GetDesc_DefaultTexture2D(D3D11_TEXTURE2D_DESC* pDesc);
	//static TExportFuncWrapper<D3D11_TEXTURE2D_DESC*> WrappedGetDesc_DefaultTexture2D("GetDesc_DefaultTexture2D", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode GetDesc_DefaultBuffer(D3D11_BUFFER_DESC* pDesc);
	//static TExportFuncWrapper<D3D11_BUFFER_DESC*> WrappedGetDesc_DefaultBuffer("GetDesc_DefaultBuffer", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateTexture2D(
	//	D3D11_TEXTURE2D_DESC* pDesc,
	//	ID3D11Device* device,
	//	ID3D11Texture2D** ppTex,
	//	ID3D11ShaderResourceView** ppSRV,
	//	ID3D11RenderTargetView** ppRTV);
	//static TExportFuncWrapper<
	//	D3D11_TEXTURE2D_DESC*,
	//	ID3D11Device*,
	//	ID3D11Texture2D**,
	//	ID3D11ShaderResourceView**,
	//	ID3D11RenderTargetView**> WrappedCreateTexture2D("CreateTexture2D", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateShaderResourceView(
	//	ID3D11Texture2D* tex,
	//	ID3D11ShaderResourceView** ppSRV);
	//static TExportFuncWrapper<
	//	ID3D11Texture2D*,
	//	ID3D11ShaderResourceView**> WrappedCreateShaderResourceView("CreateShaderResourceView", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateRenderTargetView(
	//	ID3D11Texture2D* tex,
	//	ID3D11RenderTargetView** ppRTV);
	//static TExportFuncWrapper<
	//	ID3D11Texture2D*,
	//	ID3D11RenderTargetView**> WrappedCreateRenderTargetView("CreateRenderTargetView", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateMesh_Rect(
	//	ID3D11Device* device,
	//	float width,
	//	float height,
	//	UINT vertexSize,
	//	ID3D11Buffer** ppVB,
	//	ID3D11Buffer** ppIB);
	//static TExportFuncWrapper<
	//	ID3D11Device*,
	//	float,
	//	float,
	//	UINT,
	//	ID3D11Buffer**,
	//	ID3D11Buffer**> WrappedCreateMesh_Rect("CreateMesh_Rect", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateBuffer(
	//	ID3D11Device* device,
	//	D3D11_BUFFER_DESC* pDesc,
	//	ID3D11Buffer** ppBuf);
	//static TExportFuncWrapper<
	//	ID3D11Device*,
	//	D3D11_BUFFER_DESC*,
	//	ID3D11Buffer**> WrappedCreateBuffer("CreateBuffer", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateBlendState_AlphaBlend(
	//	ID3D11Device* device,
	//	ID3D11BlendState** ppBS);
	//static TExportFuncWrapper<
	//	ID3D11Device*,
	//	ID3D11BlendState**> WrappedCreateBlendState_AlphaBlend("CreateBlendState_AlphaBlend", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateBlendState_Add(
	//	ID3D11Device* device,
	//	ID3D11BlendState** ppBS);
	//static TExportFuncWrapper<
	//	ID3D11Device*,
	//	ID3D11BlendState**> WrappedCreateBlendState_Add("CreateBlendState_Add", GetModule_LostCoreD3D11);

	//extern "C" LOSTCORED3D11_API EReturnCode CreateRasterizer(ID3D11Device* device, ID3D11RasterizerState** ppRS);
	//static TExportFuncWrapper<ID3D11Device*, ID3D11RasterizerState**> WrappedCreateRasterizer("CreateRasterizer", GetModule_LostCoreD3D11);

	//extern EReturnCode GetDeviceTypeString(EDeviceType type, char** desc);
	//static TExportFuncWrapper<EDeviceType, const char**> WrappedGetDeviceTypeString("GetDeviceTypeString", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode CreateRenderContext(LostCore::EContextID id, LostCore::IRenderContext** context);
	static TExportFuncWrapper<LostCore::EContextID, LostCore::IRenderContext**> WrappedCreateRenderContext("CreateRenderContext", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode DestroyRenderContext(LostCore::IRenderContext* context);
	static TExportFuncWrapper<LostCore::IRenderContext*> WrappedDestroyRenderContext("DestroyRenderContext", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode CreatePrimitiveGroup(LostCore::IPrimitiveGroup** pg);
	static TExportFuncWrapper<LostCore::IPrimitiveGroup**> WrappedCreatePrimitiveGroup("CreatePrimitiveGroup", GetModule_LostCoreD3D11);

	extern "C" LOSTCORED3D11_API EReturnCode DestroyPrimitiveGroup(LostCore::IPrimitiveGroup* pg);
	static TExportFuncWrapper<LostCore::IPrimitiveGroup*> WrappedDestroyPrimitiveGroup("DestroyPrimitiveGroup", GetModule_LostCoreD3D11);
}
