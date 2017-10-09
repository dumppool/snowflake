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
		virtual void Commit() = 0;

		virtual void SetShaderSlot(int32 slot) = 0;
		virtual int32 GetShaderSlot() const = 0;

		virtual void SetShaderFlags(int32 flags) = 0;
		virtual int32 GetShaderFlags() const = 0;
	};
}