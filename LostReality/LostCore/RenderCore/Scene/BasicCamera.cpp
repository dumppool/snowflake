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
#include "BasicScene.h"

using namespace LostCore;

LostCore::FBasicCamera::FBasicCamera()
	: NearPlane(0.1f)
	, FarPlane(1000000.f)
	, Fov(90.f)
	, ViewEuler()
	, ViewPosition(0.f, 100.f, -160.f)
{
}

LostCore::FBasicCamera::~FBasicCamera() 
{
}

void LostCore::FBasicCamera::Init(int32 width, int32 height)
{
	ScreenWidth = width;
	ScreenHeight = height;
	RcpScreenWidth = 1.0f / ScreenWidth;
	RcpScreenHeight = 1.0f / ScreenHeight;
	AspectRatio = (float)ScreenWidth / (float)ScreenHeight;
}

bool LostCore::FBasicCamera::Config(const FJson & config)
{
	GetViewPosition() = config[K_POSITION];
	GetViewEuler() = config[K_EULER];
	SetNearPlane(config[K_NEARPLANE]);
	SetFarPlane(config[K_FARPLANE]);
	SetFov(config[K_FOV]);
	return true;
}

FJson LostCore::FBasicCamera::Save()
{
	FJson config;
	config[K_TYPE] = (uint8)ESceneNodeType::Camera;
	config[K_SUBTYPE] = (uint8)ECameraType::Default;
	config[K_POSITION] = GetViewPosition();
	config[K_EULER] = GetViewEuler();
	config[K_NEARPLANE] = GetNearPlane();
	config[K_FARPLANE] = GetFarPlane();
	config[K_FOV] = GetFov();
	return config;
}

void LostCore::FBasicCamera::Tick()
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
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

FFloat3& LostCore::FBasicCamera::GetViewPosition()
{
	return ViewPosition;
}

FFloat3& LostCore::FBasicCamera::GetViewEuler()
{
	return ViewEuler;
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

FRay LostCore::FBasicCamera::ScreenCastRay(int32 x, int32 y, float depth)
{
	auto proj = GetProjectMatrix();
	auto invView = GetViewMatrix().Invert();
	FFloat3 result(
		(2 * x * RcpScreenWidth - 1.0f) / proj.M[0][0],
		(1.0f - 2 * y * RcpScreenHeight) / proj.M[1][1],
		depth);

	result = invView.ApplyVector(result);
	auto rayP0 = invView.GetOrigin();
	return FRay(rayP0, result);
}

FFloat3 LostCore::FBasicCamera::ScreenToWorld(int32 x, int32 y, float depth)
{
	auto proj = GetProjectMatrix();
	FFloat3 result(
		(2 * x * RcpScreenWidth - 1.0f) / proj.M[0][0],
		(1.0f - 2 * y * RcpScreenHeight) / proj.M[1][1], 
		depth);

	result = GetViewMatrix().Invert().ApplyPoint(result);
	return result;
}
