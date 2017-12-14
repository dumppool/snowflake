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
	enum class EPrimitiveTopology : uint8
	{
		PointList,
		LineList,
		TriangleList,
		TriangleFan,
		TriangleStrip,
	};

	class IPrimitive
	{
	public:
		virtual ~IPrimitive() {}

		virtual void Commit() = 0;

		virtual void SetRenderOrder(ERenderOrder ro) = 0;
		virtual ERenderOrder GetRenderOrder() const = 0;

		virtual void SetVertexElement(uint32 flags) = 0;
		virtual uint32 GetFlags() const = 0;

		virtual void ConstructVB(const void* buf, uint32 sz, uint32 stride, bool bDynamic) = 0;
		virtual void ConstructIB(const FBuf& buf, uint32 stride, bool bDynamic) = 0;
		virtual void SetTopology(EPrimitiveTopology topo) = 0;

		// UpdateVBֻ����ConstructVB֮����ã�����Ҫ��Vertex��dynamic��.
		// vertex stride�����Ըı�.
		// ���bytes����VertexCount*VertexStride��UpdateVB�ڻ����´���VB.
		// ����UpdateVB���ͷ�IndexBuffer
		virtual void UpdateVB(const void* buf, uint32 sz, uint32 stride) = 0;
	};
}