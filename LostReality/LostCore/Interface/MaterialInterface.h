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

namespace LostCore
{
	class ITexture;

	class IMaterial : public IDrawable
	{
	public:
		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) = 0;
		virtual void UpdateConstantBuffer(IRenderContext* rc, const void* buf, int32 sz) = 0;
		virtual void UpdateTexture(IRenderContext* rc, ITexture* tex, int32 slot) = 0;
		virtual void SetDepthStencilState(const char* name) = 0;
	};
}