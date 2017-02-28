/*
* file MaterialInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "Drawable.h"
#include "ShaderInterface.h"

namespace LostCore
{

	class IMaterial : public IDrawable
	{
	public:
		virtual bool LoadShader(const char* path) = 0;
		virtual void UpdateMatrix_World(const FMatrix& mat) = 0;
	};
}