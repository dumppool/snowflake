/*
* file DirectionalLight.h
*
* author luoxw
* date 2017/10/20
*
*
*/

#pragma once

namespace LostCore
{
	class FDirectionalLight
	{
		FColor128 Color;
		FFloat3 Direction;

	public:
		FDirectionalLight();
		FDirectionalLight(const FColor128& col, const FFloat3& dir);
		~FDirectionalLight();
	};
}