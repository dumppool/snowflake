/*
* file CameraFactory.cpp
*
* author luoxw
* date 2017/09/21
*
*
*/

#include "stdafx.h"
using namespace LostCore;

LostCore::FBasicCamera* LostCore::FCameraFactory::NewCameraDefault()
{
	auto cam = new FBasicCamera;
	cam->GetViewPosition() = FFloat3(0.0f, 10.0f, 0.0f);
	cam->GetViewEuler() = FFloat3(0.0f, 0.0f, 0.0f);
	cam->SetNearPlane(0.1f);
	cam->SetFarPlane(10000.0f);
	cam->SetFov(90.0f);
	return cam;
}

FBasicCamera * LostCore::FCameraFactory::NewCamera(const FJson & config)
{
	if (config[K_SUBTYPE] == (uint8)ECameraType::Default)
	{
		auto cam = new FBasicCamera;
		cam->Config(config);
		return cam;
	}

	return nullptr;
}
