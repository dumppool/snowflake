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
		virtual bool Initialize(int32 sz, bool dynamic) = 0;
		virtual void UpdateBuffer(const void* buf, int32 sz) = 0;
		virtual void Bind(int32 slot) = 0;
	};
}