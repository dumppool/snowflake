/*
* file ShaderInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "Drawable.h"
#include "ShaderParameterInterface.h"
#include "TextureInterface.h"

namespace LostCore
{
	class IShader : public IDrawable
	{
	public:
		virtual void SetShaderParameter(uint32 slot, IShaderParameter* sp) = 0;
		virtual void SetShaderResource(uint32 slot, ITexture* tex) = 0;
	};
}