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
	enum class EMaterialID : uint8
	{
		Undefined,

	};

	class IMaterial : public IDrawable
	{
	public:
		virtual void SetVertexShader(IShader* shader) = 0;
		virtual const IShader* GetVertexShader() const = 0;

		virtual void SetPixelShader(IShader* shader) = 0;
		virtual const IShader* GetPixelShader() const = 0;
	};
}