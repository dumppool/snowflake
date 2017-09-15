// LostCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GlobalHandler.h"

using namespace LostCore;

LostCore::FGlobalHandler::FGlobalHandler()
	: RenderContextPP(nullptr)
	, DisplayNormalLength(5.0f)
	, MoveCameraCallback(nullptr)
	, RotateCameraCallback(nullptr)
	, AnimateRate(1.0f)
	, LoadModelCallback(nullptr)
	, LoadAnimationCallback(nullptr)
	, LoggingFunc(nullptr)
	, InitializeWindowCallback(nullptr)
	, LoadFBXCallback(nullptr)
	, ClearSceneCallback(nullptr)
	, PickingCallback(nullptr)
	, ShutdownCallback(nullptr)
{
}

EReturnCode LostCore::FGlobalHandler::SetDisplayNormalLength(float value)
{
	DisplayNormalLength = value;
	return SSuccess;
}

float LostCore::FGlobalHandler::GetDisplayNormalLength() const
{
	return DisplayNormalLength;
}

EReturnCode LostCore::FGlobalHandler::MoveCamera(float x, float y, float z)
{
	if (MoveCameraCallback != nullptr)
	{
		MoveCameraCallback(x, y, z);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::RotateCamera(float p, float y, float r)
{
	if (RotateCameraCallback != nullptr)
	{
		RotateCameraCallback(p, y, r);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::SetAnimateRate(float rate)
{
	AnimateRate = rate;

	return SSuccess;
}

float LostCore::FGlobalHandler::GetAnimateRate() const
{
	return AnimateRate;
}

EReturnCode LostCore::FGlobalHandler::SetDisplayFlags(uint32 flags)
{
	DisplayFlags = flags;

	return SSuccess;
}

uint32 LostCore::FGlobalHandler::GetDisplayFlags() const
{
	return DisplayFlags;
}

EReturnCode LostCore::FGlobalHandler::SetAnimUpdater(PFN_AnimUpdate animUpdate)
{
	AnimUpdateFunc = animUpdate;

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::PlayAnimation(const char * anim)
{
	if (PlayAnimationCallback != nullptr)
	{
		PlayAnimationCallback(anim);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::LoadAsset(uint32 flag, const char * url)
{
	if ((EAssetFlag)flag == EAssetFlag::AssetModel && LoadModelCallback != nullptr)
	{
		LoadModelCallback(url);
	}
	else if ((EAssetFlag)flag == EAssetFlag::AssetAnimation && LoadAnimationCallback != nullptr)
	{
		LoadAnimationCallback(url);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::SetLogger(PFN_Logger logger)
{
	LoggingFunc = logger;

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	if (InitializeWindowCallback != nullptr)
	{
		InitializeWindowCallback(wnd, windowed, width, height);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::LoadFBX(const char * url,
	const char * primitiveOutput,
	const char * animationOutput,
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
	bool generateTangentIfNotFound)
{
	if (LoadFBXCallback != nullptr)
	{
		LoadFBXCallback(url,
			primitiveOutput,
			animationOutput,
			clearScene,
			importTexCoord,
			importAnimation,
			importVertexColor,
			mergeNormalTangentAll,
			importNormal,
			forceRegenerateNormal,
			generateNormalIfNotFound,
			importTangent,
			forceRegenerateTangent,
			generateTangentIfNotFound);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::ClearScene()
{
	if (ClearSceneCallback != nullptr)
	{
		ClearSceneCallback();
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::Picking(int32 x, int32 y, bool clicked)
{
	if (PickingCallback != nullptr)
	{
		PickingCallback(x, y, clicked);
	}

	return SSuccess;
}

EReturnCode LostCore::FGlobalHandler::Shutdown()
{
	if (ShutdownCallback != nullptr)
	{
		ShutdownCallback();
	}

	return SSuccess;
}

void LostCore::FGlobalHandler::SetRenderContextPP(IRenderContext ** rc)
{
	RenderContextPP = rc;
}

IRenderContext * LostCore::FGlobalHandler::GetRenderContext() const
{
	if (RenderContextPP != nullptr)
	{
		return *RenderContextPP;
	}
	else
	{
		return nullptr;
	}
}

void LostCore::FGlobalHandler::SetFrameTime(float sec)
{
	FrameTime = sec;
}

float LostCore::FGlobalHandler::GetFrameTime() const
{
	return FrameTime;
}

void LostCore::FGlobalHandler::SetMoveCameraCallback(Callback_FFF callback)
{
	MoveCameraCallback = callback;
}

void LostCore::FGlobalHandler::SetRotateCameraCallback(Callback_FFF callback)
{
	RotateCameraCallback = callback;
}

bool LostCore::FGlobalHandler::IsDisplay(uint32 flag) const
{
	return (DisplayFlags & flag) == flag;
}

void LostCore::FGlobalHandler::ClearAnimationList()
{
	if (AnimUpdateFunc != nullptr)
	{
		AnimUpdateFunc((int32)EUpdateFlag::UpdateAnimClear, nullptr);
	}
}

void LostCore::FGlobalHandler::AddAnimation(const string& anim)
{
	if (AnimUpdateFunc != nullptr)
	{
		AnimUpdateFunc((int32)EUpdateFlag::UpdateAnimAdd, anim.c_str());
	}
}

void LostCore::FGlobalHandler::SetCallbackPlayAnimation(Callback_S playAnim)
{
	PlayAnimationCallback = playAnim;
}

void LostCore::FGlobalHandler::SetCallbackLoadModel(Callback_S loadModel)
{
	LoadModelCallback = loadModel;
}

void LostCore::FGlobalHandler::SetCallbackLoadAnimation(Callback_S loadAnimation)
{
	LoadAnimationCallback = loadAnimation;
}

void LostCore::FGlobalHandler::Logging(int32 level, const string & msg)
{
	if (LoggingFunc != nullptr)
	{
		LoggingFunc(level, msg.c_str());
	}
}

void LostCore::FGlobalHandler::SetCallbackInitializeWindow(Callback_HBII initWin)
{
	InitializeWindowCallback = initWin;
}

void LostCore::FGlobalHandler::SetCallbackLoadFBX(Callback_X loadFBX)
{
	LoadFBXCallback = loadFBX;
}

void LostCore::FGlobalHandler::SetCallbackClearScene(Callback_V clearScene)
{
	ClearSceneCallback = clearScene;
}

void LostCore::FGlobalHandler::SetCallbackPicking(Callback_IIB picking)
{
	PickingCallback = picking;
}

void LostCore::FGlobalHandler::SetCallbackShutdown(Callback_V shutdown)
{
	ShutdownCallback = shutdown;
}

#define EXPORTER_PTR (LostCore::FGlobalHandler::Get())

EXPORT_WRAP_1_DEF(SetDisplayNormalLength, float);
EXPORT_WRAP_3_DEF(MoveCamera, float, float, float); // x, y, z
EXPORT_WRAP_3_DEF(RotateCamera, float, float, float); // pitch, yaw, roll
EXPORT_WRAP_1_DEF(SetAnimateRate, float);
EXPORT_WRAP_1_DEF(SetDisplayFlags, uint32);
EXPORT_WRAP_1_DEF(SetAnimUpdater, LostCore::PFN_AnimUpdate);
EXPORT_WRAP_1_DEF(PlayAnimation, const char*);
EXPORT_WRAP_2_DEF(LoadAsset, uint32, const char*);
EXPORT_WRAP_1_DEF(SetLogger, LostCore::PFN_Logger);
EXPORT_WRAP_4_DEF(InitializeWindow, HWND, bool, int32, int32);
EXPORT_WRAP_14_DEF(LoadFBX, const char*, const char*, const char*, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
EXPORT_WRAP_0_DEF(ClearScene);
EXPORT_WRAP_3_DEF(Picking, int32, int32, bool);
EXPORT_WRAP_0_DEF(Shutdown);

#undef EXPORTER_PTR