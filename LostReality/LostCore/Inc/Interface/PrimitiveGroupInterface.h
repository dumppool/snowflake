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

		// UpdateVBֻ����ConstructVB֮����ã�����Ҫ��Vertex��dynamic��.
		// vertex stride�����Ըı�.
		// ���bytes����VertexCount*VertexStride��UpdateVB�ڻ����´���VB.
		// ����UpdateVB��ǿ������IndexBuffer
		virtual void UpdateVB(const void* buf, uint32 bytes) = 0;
	};
}