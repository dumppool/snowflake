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

	class IPrimitiveGroup
	{
	public:
		virtual ~IPrimitiveGroup() {}

		virtual void Commit() = 0;

		virtual void SetRenderOrder(ERenderOrder ro) = 0;
		virtual ERenderOrder GetRenderOrder() const = 0;

		virtual void SetVertexElement(uint32 flags) = 0;
		virtual uint32 GetVertexElement() const = 0;

		virtual bool ConstructVB(const void* buf, uint32 bytes, uint32 stride, bool bDynamic) = 0;
		virtual bool ConstructIB(const void* buf, uint32 bytes, uint32 stride, bool bDynamic) = 0;
		virtual void SetTopology(EPrimitiveTopology topo) = 0;

		// UpdateVB只能再ConstructVB之后调用，并且要求Vertex是dynamic的.
		// vertex stride不可以改变.
		// 如果bytes大于VertexCount*VertexStride，UpdateVB内会重新创建VB.
		// 调用UpdateVB会强行销毁IndexBuffer
		virtual void UpdateVB(const void* buf, uint32 bytes) = 0;
	};
}