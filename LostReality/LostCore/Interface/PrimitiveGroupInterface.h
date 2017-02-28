/*
* file PrimitiveGroupInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "Drawable.h"
#include "MaterialInterface.h"

namespace LostCore
{
	class IPrimitiveGroup  : public IDrawable
	{
	public:
		virtual bool LoadPrimitive(const char* path) = 0;
		virtual bool ConstructVB(IRenderContext* rc, const void* buf, uint32 bytes, uint32 stride, bool bDynamic) = 0;
		virtual bool ConstructIB(IRenderContext* rc, const void* buf, uint32 bytes, uint32 stride, bool bDynamic) = 0;

		virtual void SetMaterial(IMaterial* mat) = 0;
		virtual const IMaterial* GetMaterial() const = 0;
	};
}