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
