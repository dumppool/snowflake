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

LOSTCORE_API void LostCore::SetDisplayNormal(bool enable)
{
	FGlobalHandler::Get()->SetDisplayNormal(enable);
}

LOSTCORE_API void LostCore::SetDisplayTangent(bool enable)
{
	FGlobalHandler::Get()->SetDisplayTangent(enable);
}

LOSTCORE_API void LostCore::SetDisplayNormalLength(float value)
{
	FGlobalHandler::Get()->SetDisplayNormalLength(value);
}

LOSTCORE_API void LostCore::MoveCamera(float x, float y, float z)
{
	FGlobalHandler::Get()->MoveCamera(x, y, z);
}

LOSTCORE_API void LostCore::RotateCamera(float p, float y, float r)
{
	FGlobalHandler::Get()->RotateCamera(p, y, r);
}

LOSTCORE_API void LostCore::SetAnimateRate(float rate)
{
	FGlobalHandler::Get()->SetAnimateRate(rate);
}

LOSTCORE_API void LostCore::SetDisplaySkeleton(bool enable)
{
	FGlobalHandler::Get()->SetDisplaySkeleton(enable);
}

LOSTCORE_API void LostCore::SetAnimUpdater(PFN_AnimUpdate animUpdate)
{
	FGlobalHandler::Get()->SetAnimUpdater(animUpdate);
}

LOSTCORE_API void LostCore::PlayAnimation(const char * anim)
{
	FGlobalHandler::Get()->PlayAnimation(anim);
}
