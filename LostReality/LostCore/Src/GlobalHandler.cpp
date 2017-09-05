// LostCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "GlobalHandler.h"

using namespace LostCore;

LostCore::FGlobalHandler::FGlobalHandler()
	: bDisplayNormal(false)
	, bDisplayTangent(false)
	, DisplayNormalLength(5.0f)
	, MoveCameraCallback(nullptr)
	, RotateCameraCallback(nullptr)
	, AnimateRate(1.0f)
	, LoadModelCallback(nullptr)
	, LoadAnimationCallback(nullptr)
	, LoggingFunc(nullptr)
	, InitializeWindowCallback(nullptr)
	, LoadFBXCallback(nullptr)
{
}

void LostCore::FGlobalHandler::SetDisplayNormal(bool enable)
{
	bDisplayNormal = enable;
}

bool LostCore::FGlobalHandler::GetDisplayNormal() const
{
	return bDisplayNormal;
}

void LostCore::FGlobalHandler::SetDisplayTangent(bool enable)
{
	bDisplayTangent = enable;
}

bool LostCore::FGlobalHandler::GetDisplayTangent() const
{
	return bDisplayTangent;
}

void LostCore::FGlobalHandler::SetDisplayNormalLength(float value)
{
	DisplayNormalLength = value;
}

float LostCore::FGlobalHandler::GetDisplayNormalLength() const
{
	return DisplayNormalLength;
}

void LostCore::FGlobalHandler::MoveCamera(float x, float y, float z)
{
	if (MoveCameraCallback != nullptr)
	{
		MoveCameraCallback(x, y, z);
	}
}

void LostCore::FGlobalHandler::RotateCamera(float p, float y, float r)
{
	if (RotateCameraCallback != nullptr)
	{
		RotateCameraCallback(p, y, r);
	}
}

void LostCore::FGlobalHandler::SetAnimateRate(float rate)
{
	AnimateRate = rate;
}

float LostCore::FGlobalHandler::GetAnimateRate() const
{
	return AnimateRate;
}

void LostCore::FGlobalHandler::SetDisplaySkeleton(uint32 flag)
{
	FlagDisplaySkeleton = flag;
}

uint32 LostCore::FGlobalHandler::GetDisplaySkeleton() const
{
	return FlagDisplaySkeleton;
}

void LostCore::FGlobalHandler::SetAnimUpdater(PFN_AnimUpdate animUpdate)
{
	AnimUpdateFunc = animUpdate;
}

void LostCore::FGlobalHandler::PlayAnimation(const char * anim)
{
	if (PlayAnimationCallback != nullptr)
	{
		PlayAnimationCallback(anim);
	}
}

void LostCore::FGlobalHandler::LoadAsset(uint32 flag, const char * name)
{
	if (flag == FLAG_ASSET_MODEL && LoadModelCallback != nullptr)
	{
		LoadModelCallback(name);
	}
	else if (flag == FLAG_ASSET_ANIMATION && LoadAnimationCallback != nullptr)
	{
		LoadAnimationCallback(name);
	}
}

void LostCore::FGlobalHandler::SetLogger(PFN_Logger logger)
{
	LoggingFunc = logger;
}

void LostCore::FGlobalHandler::InitializeWindow(HWND wnd, bool windowed, int32 width, int32 height)
{
	if (InitializeWindowCallback != nullptr)
	{
		InitializeWindowCallback(wnd, windowed, width, height);
	}
}

void LostCore::FGlobalHandler::LoadFBX(const char * file,
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
		LoadFBXCallback(file,
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
}

bool LostCore::FGlobalHandler::IsDisplayNormal(uint32 flags) const
{
	bool displayTangent = (flags & EVertexElement::Tangent) == EVertexElement::Tangent
		&& FGlobalHandler::Get()->GetDisplayTangent();

	return (flags & EVertexElement::Normal) == EVertexElement::Normal
		&& !displayTangent && FGlobalHandler::Get()->GetDisplayNormal();
}

bool LostCore::FGlobalHandler::IsDisplayTangent(uint32 flags) const
{
	return (flags & EVertexElement::Tangent) == EVertexElement::Tangent
		&& FGlobalHandler::Get()->GetDisplayTangent();
}

void LostCore::FGlobalHandler::SetMoveCameraCallback(Callback_FFF callback)
{
	MoveCameraCallback = callback;
}

void LostCore::FGlobalHandler::SetRotateCameraCallback(Callback_FFF callback)
{
	RotateCameraCallback = callback;
}

bool LostCore::FGlobalHandler::IsDisplaySkeleton(uint32 flag) const
{
	return (FlagDisplaySkeleton & flag) == flag;
}

void LostCore::FGlobalHandler::ClearAnimationList()
{
	if (AnimUpdateFunc != nullptr)
	{
		AnimUpdateFunc(FLAG_ANIM_CLEAR, nullptr);
	}
}

void LostCore::FGlobalHandler::AddAnimation(const char * anim)
{
	if (AnimUpdateFunc != nullptr)
	{
		AnimUpdateFunc(FLAG_ANIM_ADD, anim);
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

EXPORT_WRAP_1_DEF(SetDisplayNormal, bool);
EXPORT_WRAP_1_DEF(SetDisplayTangent, bool);
EXPORT_WRAP_1_DEF(SetDisplayNormalLength, float);
EXPORT_WRAP_3_DEF(MoveCamera, float, float, float); // x, y, z
EXPORT_WRAP_3_DEF(RotateCamera, float, float, float); // pitch, yaw, roll
EXPORT_WRAP_1_DEF(SetAnimateRate, float);
EXPORT_WRAP_1_DEF(SetDisplaySkeleton, bool);
EXPORT_WRAP_1_DEF(SetAnimUpdater, LostCore::PFN_AnimUpdate);
EXPORT_WRAP_1_DEF(PlayAnimation, const char*);
EXPORT_WRAP_2_DEF(LoadAsset, uint32, const char*);
EXPORT_WRAP_1_DEF(SetLogger, LostCore::PFN_Logger);
EXPORT_WRAP_4_DEF(InitializeWindow, HWND, bool, int32, int32);
EXPORT_WRAP_14_DEF(LoadFBX, const char*, const char*, const char*, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool, bool);
