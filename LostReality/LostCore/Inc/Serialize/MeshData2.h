/*
* file StructSerialize.h
*
* author luoxw
* date 2017/05/26
*
*
*/

#pragma once

#include "Serialize/StructSerialize.h"

namespace LostCore
{

	struct FMeshData2
	{
		uint32 IndexCount;
		uint32 VertexCount;
		uint32 VertexFlags;

		vector<string> Bones;

		vector<uint16> Indices16;
		vector<uint32> Indices32;

		uint32 VertexMagic;

		vector<FFloat3> Coordinates;
		vector<FFloat2> UV;
		vector<FFloat3> Normal;
		vector<FFloat3> Tangent;
		vector<FFloat3> Binormal;
		vector<FFloat3> VertexColor;

		vector<FFloat4> BlendWeights;
		vector<FSInt4> BlendIndices;

		FPoseMap DefaultPose;

		TTreeNode<FMatrixNode> RootNode;

		FMeshData2() {}
		~FMeshData2() {}
	};

	// serialize
	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const FMeshData2& data)
	{
		stream << data.IndexCount << data.VertexCount << data.VertexFlags;
		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			stream << data.Bones.size();
			for (size_t i = 0; i < data.Bones.size(); ++i)
			{
				auto it = data.Bones.begin() + i;
				stream << *it;
			}
		}

		if (data.VertexCount < (1 << 16))
		{
			Serialize(stream, (const uint8*)&data.Indices16[0], data.IndexCount * sizeof(uint16));
		}
		else
		{
			Serialize(stream, (const uint8*)&data.Indices32[0], data.IndexCount * sizeof(uint32));
		}

		uint32 stride = GetVertexDetails(data.VertexFlags).Stride;
		uint32 paddingBytes = GetPaddingSize(stride, 16);
		uint8* padding = new uint8[paddingBytes];
		stream << (uint32)MAGIC_VERTEX;
		for (uint32 i = 0; i < data.VertexCount; ++i)
		{
			stream << data.Coordinates[i];

			if ((data.VertexFlags & EVertexElement::UV) == EVertexElement::UV)
			{
				stream << data.UV[i];
			}

			if ((data.VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
			{
				stream << data.Normal[i];
			}

			if ((data.VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
			{
				stream << data.Tangent[i];
				stream << data.Binormal[i];
			}

			if ((data.VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
			{
				stream << data.VertexColor[i];
			}

			if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				stream << data.BlendWeights[i] << data.BlendIndices[i];
			}

			if (paddingBytes != 0)
			{
				Serialize(stream, padding, paddingBytes);
			}
		}

		stream << data.DefaultPose.size();
		for (auto it = data.DefaultPose.begin(); it != data.DefaultPose.end(); ++it)
		{
			stream << it->first << it->second;
		}

		stream << data.RootNode;

		delete[] padding;
		return stream;
	}

	// deserialize
	template<>
	inline FBinaryIO& operator>>(FBinaryIO& stream, FMeshData2& data)
	{
		stream >> data.IndexCount >> data.VertexCount >> data.VertexFlags;
		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			data.Bones.clear();

			size_t boneSize;
			stream >> boneSize;
			for (size_t i = 0; i < boneSize; ++i)
			{
				std::string boneName;
				stream >> boneName;
				data.Bones.push_back(boneName);
			}
		}

		if (data.VertexCount < (1 << 16))
		{
			data.Indices16.resize(data.IndexCount);
			Deserialize(stream, (uint8*)&data.Indices16[0], data.IndexCount * sizeof(uint16));
		}
		else
		{
			data.Indices32.resize(data.IndexCount);
			Deserialize(stream, (uint8*)&data.Indices32[0], data.IndexCount * sizeof(uint32));
		}

		uint32 stride = GetVertexDetails(data.VertexFlags).Stride;
		uint32 paddingBytes = GetPaddingSize(stride, 16);
		uint8* padding = new uint8[paddingBytes];
		bool allocating = true;
		stream >> data.VertexMagic;
		assert(data.VertexMagic == MAGIC_VERTEX && "vertex data is incorrect");
		for (uint32 i = 0; i < data.VertexCount; ++i)
		{
			if (allocating)
			{
				data.Coordinates.resize(data.VertexCount);
			}

			stream >> *(data.Coordinates.begin() + i);
			if ((data.VertexFlags & EVertexElement::UV) == EVertexElement::UV)
			{
				if (allocating)
				{
					data.UV.resize(data.VertexCount);
				}

				stream >> *(data.UV.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
			{
				if (allocating)
				{
					data.Normal.resize(data.VertexCount);
				}

				stream >> *(data.Normal.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
			{
				if (allocating)
				{
					data.Tangent.resize(data.VertexCount);
					data.Binormal.resize(data.VertexCount);
				}

				stream >> *(data.Tangent.begin() + i);
				stream >> *(data.Binormal.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
			{
				if (allocating)
				{
					data.VertexColor.resize(data.VertexCount);
				}

				stream >> *(data.VertexColor.begin() + i);
			}

			if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				if (allocating)
				{
					data.BlendWeights.resize(data.VertexCount);
					data.BlendIndices.resize(data.VertexCount);
				}

				stream >> *(data.BlendWeights.begin() + i)
					>> *(data.BlendIndices.begin() + i);
			}

			allocating = false;

			if (paddingBytes != 0)
			{
				Deserialize(stream, padding, paddingBytes);
			}
		}

		uint32 defaultPoseSz;
		stream >> defaultPoseSz;
		for (uint32 i = 0; i < defaultPoseSz; ++i)
		{
			string boneName;
			FFloat4x4 boneMatrix;
			stream >> boneName >> boneMatrix;
			data.DefaultPose[boneName] = boneMatrix;
		}

		stream >> data.RootNode;

		delete[] padding;
		return stream;
	}
}