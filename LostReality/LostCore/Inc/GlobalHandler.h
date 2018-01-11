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

typedef char* FStrArr;

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
		UpdateMonitorName,
		UpdateRayTestDistance,
	};

	enum class EAssetOperation : uint8
	{
		LoadModel = 0,
		LoadAnimation,
		LoadScene,
		SaveScene,
	};

	enum class ELogFlag : uint8
	{
		LogInfo = 0,
		LogWarning,
		LogError,
	};

	typedef void(WINAPI *PFN_UpdateFlagAndString)(int32 flag, const char* str);
	typedef void(WINAPI *PFN_UpdateFlagAnd32Bit)(int32 flag, uint32 val);
	typedef void(WINAPI *PFN_Logger)(int32 level, const char* msg);
	typedef void(WINAPI *PFN_UpdatePosAndRot)(float, float, float, float, float, float);

	typedef function<void()> Callback_V;
	typedef function<void(bool)> Callback_B;
	typedef function<void(int32, const char*)> Callback_IS;
	typedef function<void(int32, int32)> Callback_II;
	typedef function<void(int32, int32, bool, bool)> Callback_IIBB;
	typedef function<void(float, float, float)> Callback_FFF;
	typedef function<void(float, float, float, float, float, float)> Callback_FFFFFF;
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

	typedef function<void(FStrArr, int32, int32*)> Callback_SAI;

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

		PFN_UpdateFlagAndString UpdateFlagAndNameFunc;
		PFN_UpdateFlagAnd32Bit UpdateFlagAnd32BitFunc;
		PFN_Logger LoggingFunc;
		PFN_UpdatePosAndRot UpdatePosAndRotFunc;

		Callback_S PlayAnimationCallback;
		Callback_HBII InitializeWindowCallback;
		Callback_X LoadFBXCallback;
		Callback_V ClearSceneCallback;
		Callback_IIBB PickingCallback;
		Callback_V ShutdownCallback;
		Callback_II HoverCallback;
		Callback_II ClickCallback;
		Callback_II DraggingCallback;
		Callback_V EndDragCallback;
		Callback_IS AssetOperateCallback;
		Callback_V RecordProfileCallback;
		Callback_SAI GetConsoleNamesCallback;
		Callback_S DisplayConsoleCallback;

	public: // 导出函数调用.		
		EReturnCode InitializeProcessUnique();
		EReturnCode DestroyProcessUnique();
		EReturnCode SetProcessUnique(void* p);
		EReturnCode SetDisplayNormalLength(float value);
		EReturnCode MoveCamera(float x, float y, float z);
		EReturnCode RotateCamera(float p, float y, float r);
		EReturnCode SetAnimateRate(float rate);
		EReturnCode SetDisplayFlags(uint32 flags);
		EReturnCode SetUpdateFlagAndString(PFN_UpdateFlagAndString pfn);
		EReturnCode SetUpdateFlagAnd32Bit(PFN_UpdateFlagAnd32Bit pfn);
		EReturnCode PlayAnimation(const char* anim);
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
		EReturnCode Shutdown();
		EReturnCode SetUpdatePosAndRot(PFN_UpdatePosAndRot pfn);

		EReturnCode OnHover(int32 x, int32 y);
		EReturnCode OnClick(int32 x, int32 y);
		EReturnCode OnDragging(int32 x, int32 y);
		EReturnCode OnEndDrag();
		EReturnCode AssetOperate(int32 op, const char* url);
		EReturnCode RecordProfile();
		EReturnCode DeallocateStringArray(FStrArr str, int32 count);
		EReturnCode GetConsoleNames(FStrArr names, int32 sz, int32* count);
		EReturnCode DisplayConsole(const char* name);

	public: // 模块内部方法.
		FGlobalHandler();

		float GetDisplayNormalLength() const;
		uint32 GetDisplayFlags() const;
		float GetAnimateRate() const;

		void SetRenderContextPP(IRenderContext** rc);
		IRenderContext* GetRenderContext() const;

		bool IsDisplay(uint32 flag) const;
		void Logging(int32 level, const string& msg);

		void SetMoveCameraCallback(Callback_FFF callback);
		void SetRotateCameraCallback(Callback_FFF callback);
		void SetPlayAnimationCallback(Callback_S callback);
		void SetInitializeWindowCallback(Callback_HBII callback);
		void SetLoadFBXCallback(Callback_X callback);
		void SetClearSceneCallback(Callback_V callback);
		void SetShutdownCallback(Callback_V callback);
		void SetHoverCallback(Callback_II callback);
		void SetClickCallback(Callback_II callback);
		void SetDraggingCallback(Callback_II callback);
		void SetEndDragCallback(Callback_V callback);
		void SetAssetOperateCallback(Callback_IS callback);
		void SetRecordProfileCallback(Callback_V callback);
		void SetGetConsoleNamesCallback(Callback_SAI callback);
		void SetDisplayConsoleCallback(Callback_S callback);

		void UpdateFlagAndName(EUpdateFlag flag, const string& name);
		void UpdateFlagAnd32Bit(EUpdateFlag flag, uint32 val);
		void UpdatePosAndRot(const FFloat3& pos, const FFloat3& rot);
	};

}

EXPORT_WRAP_0_DCL(InitializeProcessUnique);
EXPORT_WRAP_0_DCL(DestroyProcessUnique);
EXPORT_WRAP_1_DCL(SetProcessUnique, void*);
EXPORT_WRAP_1_DCL(SetDisplayNormalLength, float);
EXPORT_WRAP_3_DCL(MoveCamera, float, float, float); // x, y, z
EXPORT_WRAP_3_DCL(RotateCamera, float, float, float); // pitch, yaw, roll
EXPORT_WRAP_1_DCL(SetAnimateRate, float);
EXPORT_WRAP_1_DCL(SetDisplayFlags, uint32);
EXPORT_WRAP_1_DCL(SetUpdateFlagAndString, LostCore::PFN_UpdateFlagAndString);
EXPORT_WRAP_1_DCL(SetUpdateFlagAnd32Bit, LostCore::PFN_UpdateFlagAnd32Bit);
EXPORT_WRAP_1_DCL(PlayAnimation, const char*);
EXPORT_WRAP_1_DCL(SetLogger, LostCore::PFN_Logger);
EXPORT_WRAP_4_DCL(InitializeWindow, HWND, bool, int32, int32);
EXPORT_WRAP_14_DCL(LoadFBX, const char*, const char*, const char*, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
EXPORT_WRAP_0_DCL(ClearScene);
EXPORT_WRAP_0_DCL(Shutdown);
EXPORT_WRAP_1_DCL(SetUpdatePosAndRot, LostCore::PFN_UpdatePosAndRot);
EXPORT_WRAP_2_DCL(OnHover, int32, int32);
EXPORT_WRAP_2_DCL(OnClick, int32, int32);
EXPORT_WRAP_2_DCL(OnDragging, int32, int32);
EXPORT_WRAP_0_DCL(OnEndDrag);
EXPORT_WRAP_2_DCL(AssetOperate, int32, const char*);
EXPORT_WRAP_0_DCL(RecordProfile);
EXPORT_WRAP_2_DCL(DeallocateStringArray, FStrArr, int32);
EXPORT_WRAP_3_DCL(GetConsoleNames, FStrArr, int32, int32*);
EXPORT_WRAP_1_DCL(DisplayConsole, const char*);