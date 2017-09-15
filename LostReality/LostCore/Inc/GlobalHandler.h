/*
* file GlobalHandler.h
*
* author luoxw
* date 2017/08/14
*
*
*/

#pragma once

#ifdef GET_MODULE
#undef GET_MODULE
#undef EXPORT_API
#endif
#define GET_MODULE LostCore::GetModule_LostCore
#define EXPORT_API LOSTCORE_API

namespace LostCore
{
	static const char* SModuleName = "LostCore.dll";
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

	static HMODULE GetModule_LostCore()
	{
		if (SModuleHandle == NULL)
		{
			StartupModule("");
		}

		return SModuleHandle;
	}

	enum class EUpdateFlag : uint8
	{
		UpdateAnimClear = 0,
		UpdateAnimAdd,
	};

	enum class EAssetFlag : uint8
	{
		AssetModel = 0,
		AssetAnimation,
	};

	enum class ELogFlag : uint8
	{
		LogInfo = 0,
		LogWarning,
		LogError,
	};

	typedef void(WINAPI *PFN_AnimUpdate)(int32 flag, const char* anim);
	typedef void(WINAPI *PFN_Logger)(int32 level, const char* msg);

	typedef function<void()> Callback_V;
	typedef function<void(bool)> Callback_B;
	typedef function<void(int32, int32)> Callback_II;
	typedef function<void(int32, int32, bool)> Callback_IIB;
	typedef function<void(float, float, float)> Callback_FFF;
	typedef function<void(const char*)> Callback_S;
	typedef function<void(HWND wnd, bool windowed, int32 width, int32 height)> Callback_HBII;
	typedef function<void(
		const char* file,
		const char* primitiveOutput,
		const char* animationOutput,
		bool clearScene,
		bool importTexCoord,
		bool importAnimation,
		bool importVertexColor,
		bool mergeNormalTangentAll,
		bool importNormal,
		bool forceRegenerateNormal,
		bool generateNormalIfNotFound,
		bool importTangent,
		bool forceRegenerateTangent,
		bool generateTangentIfNotFound)> Callback_X;

	class FGlobalHandler
	{

	public:
		static FGlobalHandler* Get()
		{
			static FGlobalHandler Inst;
			return &Inst;
		}

	private:
		IRenderContext** RenderContextPP;
		float FrameTime;

		float DisplayNormalLength;

		Callback_FFF MoveCameraCallback;
		Callback_FFF RotateCameraCallback;

		float AnimateRate;
		uint32 DisplayFlags;
		Callback_S PlayAnimationCallback;
		PFN_AnimUpdate AnimUpdateFunc;
		Callback_S LoadModelCallback;
		Callback_S LoadAnimationCallback;
		PFN_Logger LoggingFunc;
		Callback_HBII InitializeWindowCallback;
		Callback_X LoadFBXCallback;
		Callback_V ClearSceneCallback;
		Callback_IIB PickingCallback;
		Callback_V ShutdownCallback;

	public: // 导出函数调用.		
		EReturnCode SetDisplayNormalLength(float value);
		EReturnCode MoveCamera(float x, float y, float z);
		EReturnCode RotateCamera(float p, float y, float r);
		EReturnCode SetAnimateRate(float rate);
		EReturnCode SetDisplayFlags(uint32 flags);
		EReturnCode SetAnimUpdater(PFN_AnimUpdate animUpdate);
		EReturnCode PlayAnimation(const char* anim);
		EReturnCode LoadAsset(uint32 flag, const char* url);
		EReturnCode SetLogger(PFN_Logger logger);
		EReturnCode InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height);
		EReturnCode LoadFBX(
			const char* url,
			const char* primitiveOutput,
			const char* animationOutput,
			bool clearScene,
			bool importTexCoord,
			bool importAnimation,
			bool importVertexColor,
			bool mergeNormalTangentAll,
			bool importNormal,
			bool forceRegenerateNormal,
			bool generateNormalIfNotFound,
			bool importTangent,
			bool forceRegenerateTangent,
			bool generateTangentIfNotFound);

		EReturnCode ClearScene();
		EReturnCode Picking(int32 x, int32 y, bool clicked);
		EReturnCode Shutdown();

	public: // 模块内部方法.
		FGlobalHandler();

		float GetDisplayNormalLength() const;
		uint32 GetDisplayFlags() const;
		float GetAnimateRate() const;

		void SetRenderContextPP(IRenderContext** rc);
		IRenderContext* GetRenderContext() const;

		void SetFrameTime(float sec);
		float GetFrameTime() const;

		void SetMoveCameraCallback(Callback_FFF callback);
		void SetRotateCameraCallback(Callback_FFF callback);
		bool IsDisplay(uint32 flag) const;
		void ClearAnimationList();
		void AddAnimation(const string& anim);
		void SetCallbackPlayAnimation(Callback_S playAnim);
		void SetCallbackLoadModel(Callback_S loadModel);
		void SetCallbackLoadAnimation(Callback_S loadAnimation);
		void Logging(int32 level, const string& msg);
		void SetCallbackInitializeWindow(Callback_HBII initWin);
		void SetCallbackLoadFBX(Callback_X loadFBX);
		void SetCallbackClearScene(Callback_V clearScene);
		void SetCallbackPicking(Callback_IIB picking);
		void SetCallbackShutdown(Callback_V shutdown);
	};

	EXPORT_WRAP_1_DCL(SetDisplayNormalLength, float);
	EXPORT_WRAP_3_DCL(MoveCamera, float, float, float); // x, y, z
	EXPORT_WRAP_3_DCL(RotateCamera, float, float, float); // pitch, yaw, roll
	EXPORT_WRAP_1_DCL(SetAnimateRate, float);
	EXPORT_WRAP_1_DCL(SetDisplayFlags, uint32);
	EXPORT_WRAP_1_DCL(SetAnimUpdater, LostCore::PFN_AnimUpdate);
	EXPORT_WRAP_1_DCL(PlayAnimation, const char*);
	EXPORT_WRAP_2_DCL(LoadAsset, uint32, const char*);
	EXPORT_WRAP_1_DCL(SetLogger, LostCore::PFN_Logger);
	EXPORT_WRAP_4_DCL(InitializeWindow, HWND, bool, int32, int32);
	EXPORT_WRAP_14_DCL(LoadFBX, const char*, const char*, const char*, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
	EXPORT_WRAP_0_DCL(ClearScene);
	EXPORT_WRAP_3_DCL(Picking, int32, int32, bool);
	EXPORT_WRAP_0_DCL(Shutdown);
}