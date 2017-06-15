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
	struct FVertexTypes
	{
		struct Details
		{
			std::string Name;
			int Stride;

			Details() : Name("unknown"), Stride(-1) {}

			Details(const std::string& name, int stride)
				: Name(name)
				, Stride(stride)
			{
			}
		};

		static Details GetVertexDetail2D(
			bool bHasTexcoord,
			bool bHasVertexRGBA
		)
		{
			std::string typeName("xy");
			int stride = sizeof(float) * 2;

			if (bHasTexcoord)
			{
				typeName += "uv";
				stride += sizeof(float) * 2;
			}

			if (bHasVertexRGBA)
			{
				typeName += "rgba";
				stride += sizeof(float) * 4;
			}

			return Details(typeName, stride);
		}

		static Details GetVertexDetail3D(
			bool bHasTexcoord,
			bool bHasNormal,
			bool bHasTangent,
			bool bHasBinormal,
			bool bHasVertexRGB,
			bool bHasSkin
		)
		{
			std::string typeName("xyz");
			int stride = sizeof(float) * 3;

			if (bHasTexcoord)
			{
				typeName += "uv";
				stride += sizeof(float) * 2;
			}

			if (bHasNormal)
			{
				typeName += "n";
				stride += sizeof(float) * 3;
			}

			if (bHasTangent)
			{
				typeName += "t";
				stride += sizeof(float) * 3;
			}

			if (bHasBinormal)
			{
				typeName += "b";
				stride += sizeof(float) * 3;
			}

			if (bHasVertexRGB)
			{
				typeName += "rgb";
				stride += sizeof(float) * 3;
			}

			if (bHasSkin)
			{
				typeName += "_skinned";
				stride += sizeof(float) * 8;
			}

			return Details(typeName, stride);
		}
	};
}