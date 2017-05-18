/*
* file Pose.h
*
* author luoxw
* date 2017/05/16
*
*
*/

#pragma once

namespace LostCore
{
	class FBone
	{
	public:
		std::string Identifier;
		FTransform Transform;
		std::vector<FBone*> Children;
	};

	class FPose
	{
	public:
	};
}

