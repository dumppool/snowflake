/*
* file ConstantBufferInterface.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

namespace LostCore
{
	class IConstantBuffer
	{
	public:
		virtual ~IConstantBuffer() {}
		virtual bool Initialize(LostCore::IRenderContext * rc, int32 sz, bool dynamic) = 0;
		virtual void UpdateBuffer(IRenderContext* rc, const void* buf, int32 sz) = 0;
		virtual void Bind(IRenderContext* rc, int32 slot) = 0;
	};
}