/*
* file CameraFactory.h
*
* author luoxw
* date 2017/09/21
*
*
*/

#pragma once

namespace LostCore
{
	class FBasicCamera;
	class FCameraFactory
	{
	public:
		static FBasicCamera* NewCameraDefault();
		static FBasicCamera* NewCamera(const FJson & config);
	};
}

