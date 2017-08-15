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

void LostCore::FGlobalHandler::SetMoveCameraCallback(FCameraCallback callback)
{
	MoveCameraCallback = callback;
}

void LostCore::FGlobalHandler::SetRotateCameraCallback(FCameraCallback callback)
{
	RotateCameraCallback = callback;
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
