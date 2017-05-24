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

		static Details GetVertexType2DString(
			bool bHasTexcoord,
			bool bHasVertexRGBA
		)
		{
			std::string typeName("XY");
			int stride = sizeof(float) * 2;

			if (bHasTexcoord)
			{
				typeName += "UV";
				stride += sizeof(float) * 2;
			}

			if (bHasVertexRGBA)
			{
				typeName += "RGBA";
				stride += sizeof(float) * 4;
			}

			return Details(typeName, stride);
		}

		static Details GetVertexType3DString(
			bool bHasTexcoord,
			bool bHasNormal,
			bool bHasTangent,
			bool bHasBinormal,
			bool bHasVertexRGB
		)
		{
			std::string typeName("XYZ");
			int stride = sizeof(float) * 3;

			if (bHasTexcoord)
			{
				typeName += "UV";
				stride += sizeof(float) * 2;
			}

			if (bHasNormal)
			{
				typeName += "N";
				stride += sizeof(float) * 3;
			}

			if (bHasTangent)
			{
				typeName += "T";
				stride += sizeof(float) * 3;
			}

			if (bHasBinormal)
			{
				typeName += "B";
				stride += sizeof(float) * 3;
			}

			if (bHasVertexRGB)
			{
				typeName += "RGB";
				stride += sizeof(float) * 3;
			}

			return Details(typeName, stride);
		}
	};
}