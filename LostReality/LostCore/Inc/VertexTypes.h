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

	FORCEINLINE FVertexDetails GetVertexDetails(uint32 flags, bool vertexOnly = true)
	{
		int32 offset = 0;
		string name("");
		int32 bit32 = sizeof(float);
		if (vertexOnly)
		{
			if (HAS_FLAGS(VERTEX_COORDINATE2D, flags))
			{
				offset += bit32 * 2;
				name.append("xy");
			}
			else
			if (HAS_FLAGS(VERTEX_COORDINATE3D, flags))
			{
				offset += bit32 * 3;
				name.append("xyz");
			}

			if (HAS_FLAGS(VERTEX_TEXCOORD0, flags))
			{
				offset += bit32 * 2;
				name.append("_uv0");
			}

			if (HAS_FLAGS(VERTEX_TEXCOORD1, flags))
			{
				offset += bit32 * 2;
				name.append("_uv1");
			}

			if (HAS_FLAGS(VERTEX_NORMAL, flags))
			{
				offset += bit32 * 3;
				name.append("_n");
			}

			if (HAS_FLAGS(VERTEX_TANGENT, flags))
			{
				offset += bit32 * 6;
				name.append("_tb");
			}

			if (HAS_FLAGS(VERTEX_COLOR, flags))
			{
				offset += bit32 * 4;
				name.append("_color");
			}

			if (HAS_FLAGS(VERTEX_SKIN, flags))
			{
				offset += bit32 * 8;
				name.append("_skinned");
			}
		}
		else
		{
			if (HAS_FLAGS(INSTANCE_TRANSFORM2D, flags))
			{
				offset += bit32 * 2 * 3;
				name.append("_instancing2D");
			}
			else
			if (HAS_FLAGS(INSTANCE_TRANSFORM3D, flags))
			{
				offset += bit32 * 4 * 4;
				name.append("_instancing3D");
			}

			if (HAS_FLAGS(INSTANCE_TEXTILE, flags))
			{
				offset += bit32 * 2 * 2;
				name.append("_textile");
			}
		}

		return FVertexDetails(name, offset);
	}
}