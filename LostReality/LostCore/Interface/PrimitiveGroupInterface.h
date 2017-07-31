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

	class IPrimitiveGroup  : public IDrawable
	{
	public:
		virtual bool ConstructVB(IRenderContext* rc, const void* buf, uint32 bytes, uint32 stride, bool bDynamic) = 0;
		virtual bool ConstructIB(IRenderContext* rc, const void* buf, uint32 bytes, uint32 stride, bool bDynamic) = 0;
		virtual void SetTopology(EPrimitiveTopology topo) = 0;

		virtual void SetMaterial(IMaterial* mat) = 0;
		virtual const IMaterial* GetMaterial() const = 0;

		// UpdateVB只能再ConstructVB之后调用，并且要求Vertex是dynamic的.
		// vertex stride不可以改变.
		// 如果bytes大于VertexCount*VertexStride，UpdateVB内会重新创建VB.
		// 调用UpdateVB会强行销毁IndexBuffer
		virtual void UpdateVB(IRenderContext* rc, const void* buf, uint32 bytes) = 0;
	};
}