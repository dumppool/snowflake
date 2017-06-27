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
	, ViewPosition(0.f, 0.f, -5.f)
{
}

LostCore::FBasicCamera::~FBasicCamera() 
{
}

bool LostCore::FBasicCamera::Load(IRenderContext * rc, const char* url)
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

void LostCore::FBasicCamera::AddPositionWorld(const FVec3& pos)
{
	FQuat orientation;
	orientation.FromEuler(ViewEuler);

	FTransform world(orientation, ViewPosition);
	ViewPosition = world.TransformPosition(pos);
}

void LostCore::FBasicCamera::AddEulerWorld(const FVec3& euler)
{
	ViewEuler += euler;
}

void LostCore::FBasicCamera::AddPositionLocal(const FVec3& pos)
{
	FQuat orientation;
	orientation.FromEuler(ViewEuler);

	FTransform world(orientation, ViewPosition);
	ViewPosition = world.TransformPosition(pos);
}

void LostCore::FBasicCamera::AddEulerLocal(const FVec3& euler)
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

	FVec3 formattedEuler = orientation.Euler();
	
	//FTransform world(orientation, ViewPosition);
	FMatrix world, world34;
	world.SetRotateAndOrigin(orientation, ViewPosition);
	world34.SetRotateAndOrigin(orientation, ViewPosition);
	//return world.GetInversed().ToMatrix();

	FMatrix matRot, matTrans;
	matRot.SetRotate(orientation);
	matTrans.SetTranslate(ViewPosition);
	FMatrix world2 = matRot * matTrans;

	FVec3 pt1 = world.ApplyPoint(FVec3(0.f, 0.f, 0.f));
	FVec3 pt2 = world2.ApplyPoint(FVec3(0.f, 0.f, 0.f));

	FVec3 right = orientation.GetRightVector();
	FVec3 up = orientation.GetUpVector();
	FVec3 direction = orientation.GetForwardVector();
	
	world.Invert();
	world34.Invert34();

	return world;
}

FMatrix LostCore::FBasicCamera::GetProjectMatrix() const
{
	float htan = std::tan(Fov * 0.5f);
	if (IsEqual(0.f, htan))
	{
		htan = 0.001f;
	}

	float h = 1 / htan;
	float w = h / AspectRatio;
	float q = FarPlane / (FarPlane - NearPlane);
	FMatrix result;
	memset(&result, 0, sizeof(result));
	result.M[0][0] = w;
	result.M[1][1] = h;
	result.M[2][2] = q;
	result.M[2][3] = 1.f;
	result.M[3][2] = -q * NearPlane;

	return result;
}

FMatrix LostCore::FBasicCamera::GetViewProjectMatrix() const
{
	return GetViewMatrix() * GetProjectMatrix();
}