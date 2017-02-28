/*
* file ShaderParameterInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

namespace LostCore
{
	class IShaderParameter : public IDrawable
	{
	public:
		virtual ~IShaderParameter() {}
		virtual bool Construct(IRenderContext* rc, uint32 bytes, bool bDynamic) = 0;
		virtual void UpdateBuffer(IRenderContext* rc, const void* buf, uint32 bytes) = 0;
	};
}