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
	FQuat orientation;
	orientation.FromEuler(ViewEuler);

	FTransform world(orientation, ViewPosition);
	ViewPosition = world.TransformPosition(pos);
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

	FFloat3 formattedEuler = orientation.Euler();
	
	//FTransform world(orientation, ViewPosition);
	FFloat4x4 world, world34;
	world.SetRotateAndOrigin(orientation, ViewPosition);
	world34.SetRotateAndOrigin(orientation, ViewPosition);
	//return world.GetInversed().ToMatrix();

	FFloat4x4 matRot, matTrans;
	matRot.SetRotate(orientation);
	matTrans.SetTranslate(ViewPosition);
	FFloat4x4 world2 = matRot * matTrans;

	FFloat3 pt1 = world.ApplyPoint(FFloat3(0.f, 0.f, 0.f));
	FFloat3 pt2 = world2.ApplyPoint(FFloat3(0.f, 0.f, 0.f));

	FFloat3 right = orientation.GetRightVector();
	FFloat3 up = orientation.GetUpVector();
	FFloat3 direction = orientation.GetForwardVector();
	
	world.Invert();
	world34.Invert34();

	/*************************************************/
	const float s2 = Sqrt(2.f)*0.5;
	FFloat3 src(s2, 0.f, s2);
	FQuat rot0; rot0.FromEuler(FFloat3(0.f, 45.f, 0.f));
	FFloat3 euler0 = rot0.Euler();
	FFloat4x4 mat0; mat0.SetRotate(rot0);
	FFloat3 dst = mat0.ApplyPoint(src);

	FFloat4x4 mat1; mat1.SetTranslate(FFloat3(0.f, 0.f, 1.f));
	mat0 = mat0 * mat1;
	dst = mat0.ApplyPoint(src);

	FFloat4x4 mat2; mat2.M[0][0] = mat2.M[2][0] = mat2.M[2][2] = s2; mat2.M[0][2] = mat2.M[3][0] = mat2.M[3][2] = -s2;
	dst = mat2.ApplyPoint(FFloat3(0.f, 0.f, 0.f));
	dst = mat2.ApplyPoint(src);
	mat2.Invert();
	dst = mat2.ApplyPoint(src);
	/*************************************************/

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