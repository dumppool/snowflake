/*
* file BasicCamera.cpp
*
* author luoxw
* date 2017/02/09
*
*
*/

#include "stdafx.h"
#include "BasicCamera.h"

using namespace LostCore;

LostCore::FBasicCamera::FBasicCamera()
	: NearPlane(0.1f)
	, FarPlane(1000.f)
	, Fov(90.f)
	, AspectRatio(1280.f/720.f)
	, ViewEuler()
	, ViewPosition()
{
}

LostCore::FBasicCamera::~FBasicCamera() 
{
}

bool FBasicCamera::Init(const char* name, IRenderContext * rc)
{
	return true;
}

void LostCore::FBasicCamera::Fini()
{

}

void LostCore::FBasicCamera::Tick(float sec)
{

}

void LostCore::FBasicCamera::Draw(IRenderContext * rc, float sec)
{

}

void LostCore::FBasicCamera::AddPosition(const FVec3& pos)
{
	ViewPosition += pos;
}

void LostCore::FBasicCamera::AddEuler(const FVec3& euler)
{
	ViewEuler += euler;
}

void LostCore::FBasicCamera::SetNearPlane(float value)
{
	NearPlane = value;
}

float LostCore::FBasicCamera::GetNearPlane() const
{
	return NearPlane;
}

void LostCore::FBasicCamera::SetFarPlane(float value)
{
	FarPlane = value;
}

float LostCore::FBasicCamera::GetFarPlane() const
{
	return FarPlane;
}

void LostCore::FBasicCamera::SetFov(float fov)
{
	Fov = fov;
}

float LostCore::FBasicCamera::GetFov() const
{
	return Fov;
}

void FBasicCamera::SetAspectRatio(float ratio)
{
	AspectRatio = ratio;
}

float FBasicCamera::GetAspectRatio() const
{
	return AspectRatio;
}

FMatrix LostCore::FBasicCamera::GetViewMatrix() const
{
	FQuat orientation;
	orientation.FromEuler(ViewEuler);
	
	FTransform world(orientation, ViewPosition);
	return world.GetInversed().ToMatrix();
}

FMatrix LostCore::FBasicCamera::GetProjectMatrix() const
{
	float wfov = Fov;
	float hfov = wfov / AspectRatio;
	float w = 1.f / std::tan(wfov * 0.5f);
	float h = 1.f / std::tan(hfov * 0.5f);
	float q = FarPlane / (FarPlane - NearPlane);
	FMatrix result;
	memset(&result, 0, sizeof(result));
	result.M[0][0] = w;
	result.M[1][1] = h;
	result.M[2][2] = q;
	result.M[3][2] = -q * NearPlane;
	result.M[2][3] = 1.f;
	return result;
}

FMatrix LostCore::FBasicCamera::GetViewProjectMatrix() const
{
	return GetViewMatrix() * GetProjectMatrix();
}