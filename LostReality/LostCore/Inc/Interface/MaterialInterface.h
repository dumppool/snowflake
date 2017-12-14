/*
* file MaterialInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

namespace LostCore
{
	class IConstantBuffer;

	class IMaterial
	{
	public:
		virtual ~IMaterial() {}
		virtual void Bind(IRenderContext* rc) = 0;
		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) = 0;
		//virtual void UpdateTexture(IRenderContext* rc, ITextureSet* tex, int32 slot) = 0;
		virtual void SetDepthStencilState(const char* name) = 0;

		virtual void SetDepthStencil() = 0;
	};
}