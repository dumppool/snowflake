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
	, FarPlane(1000000.f)
	, Fov(90.f)
	, AspectRatio(1280.f/720.f)
	, ViewEuler()
	, ViewPosition(0.f, 100.f, -160.f)
{
}

LostCore::FBasicCamera::~FBasicCamera() 
{
}

bool LostCore::FBasicCamera::Config(IRenderContext * rc, const FJson & config)
{
	return true;
}

bool LostCore::FBasicCamera::Load(IRenderContext * rc, const char* url)
{
	return true;
}

void LostCore::FBasicCamera::Tick(float sec)
{

}

void LostCore::FBasicCamera::Draw(IRenderContext * rc, float sec)
{
	if (rc != nullptr)
	{
		rc->SetViewProjectMatrix(GetViewProjectMatrix());
	}
}

void LostCore::FBasicCamera::AddPositionWorld(const FFloat3& pos)
{
	ViewPosition += pos;
}

void LostCore::FBasicCamera::AddEulerWorld(const FFloat3& euler)
{
	ViewEuler += euler;
}

void LostCore::FBasicCamera::AddPositionLocal(const FFloat3& pos)
{
	FQuat orientation;
	orientation.FromEuler(ViewEuler);

	FTransform world(orientation, ViewPosition);
	ViewPosition = world.TransformPosition(pos);
}

void LostCore::FBasicCamera::AddEulerLocal(const FFloat3& euler)
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

FFloat4x4 LostCore::FBasicCamera::GetViewMatrix() const
{
	FQuat orientation;
	orientation.FromEuler(ViewEuler);

	FFloat4x4 world;
	world.SetRotateAndOrigin(orientation, ViewPosition);
	world.Invert();
	return world;
}

FFloat4x4 LostCore::FBasicCamera::GetProjectMatrix() const
{
	float htan = std::tan(Fov * SD2RConstant * 0.5f);
	if (IsEqual(0.f, htan))
	{
		htan = 0.001f;
	}

	float h = 1 / htan;
	float w = h / AspectRatio;
	float q = FarPlane / (FarPlane - NearPlane);
	FFloat4x4 result;
	memset(&result, 0, sizeof(result));
	result.M[0][0] = w;
	result.M[1][1] = h;
	result.M[2][2] = q;
	result.M[2][3] = 1.f;
	result.M[3][2] = -q * NearPlane;

	return result;
}

FFloat4x4 LostCore::FBasicCamera::GetViewProjectMatrix() const
{
	return GetViewMatrix() * GetProjectMatrix();
}