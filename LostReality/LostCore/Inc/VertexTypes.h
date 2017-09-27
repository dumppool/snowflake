/*
* file VertexTypes.h
*
* author luoxw
* date 2017/05/23
*
*
*/

#pragma once

namespace LostCore
{
	struct FVertexDetails
	{
		std::string Name;
		int Stride;

		FVertexDetails() : Name("unknown"), Stride(-1) {}

		FVertexDetails(const std::string& name, int stride)
			: Name(name)
			, Stride(stride)
		{
		}
	};

	FORCEINLINE FVertexDetails GetVertexDetails(uint32 flags)
	{
		int32 offset = 0;
		string name("");
		if (HAS_FLAGS(VERTEX_COORDINATE2D, flags))
		{
			offset += sizeof(float) * 2;
			name.append("xy");
		}
		else
		{
			offset += sizeof(float) * 3;
			name.append("xyz");
		}

		if (HAS_FLAGS(VERTEX_TEXCOORD0, flags))
		{
			offset += sizeof(float) * 2;
			name.append("_uv0");
		}

		if (HAS_FLAGS(VERTEX_TEXCOORD1, flags))
		{
			offset += sizeof(float) * 2;
			name.append("_uv1");
		}

		if (HAS_FLAGS(VERTEX_NORMAL, flags))
		{
			offset += sizeof(float) * 3;
			name.append("_n");
		}

		if (HAS_FLAGS(VERTEX_TANGENT, flags))
		{
			offset += sizeof(float) * 6;
			name.append("_tb");
		}

		if (HAS_FLAGS(VERTEX_COLOR, flags))
		{
			offset += sizeof(float) * 3;
			name.append("_color");
		}

		if (HAS_FLAGS(VERTEX_SKIN, flags))
		{
			offset += sizeof(float) * 8;
			name.append("_skinned");
		}

		return FVertexDetails(name, offset);
	}
}