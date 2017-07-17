/*
* file MeshData.h
*
* author luoxw
* date 2017/07/17
*
*
*/

#pragma once

#include "StructSerialize.h"

namespace LostCore
{

	// 如果没有加载json数据需求，完全可以用FTreeNode<FMatrixNode>取代
	struct FMatrixTreeNode : public FTreeNode<FMatrixNode>
	{
		FMatrixTreeNode() {}
		FMatrixTreeNode(const FJson& j)
		{
			Load(j);
		}

		void Load(const FJson& j)
		{
			assert(j.find(K_NAME) != j.end());
			Data.Name = j[K_NAME];

			Children.clear();
			if (j.find(K_LAYER) != j.end())
			{
				for (auto it = j[K_LAYER].begin(); it != j[K_LAYER].end(); ++it)
				{
					Children.push_back(FMatrixTreeNode(it.value()));
				}
			}
		}

		FMatrixTreeNode& operator=(const FTreeNode<FMatrixNode>& node)
		{

			return *this;
		}
	};

	struct FMeshData
	{
		uint32 IndexCount;
		uint32 VertexCount;
		uint32 VertexFlags;

		vector<string> Bones;

		vector<uint16> Indices16;
		vector<uint32> Indices32;

		uint32 VertexMagic;

		vector<FVec3> Coordinates;
		vector<FVec2> UV;
		vector<FVec3> Normal;
		vector<FVec3> Tangent;
		vector<FVec3> Binormal;
		vector<FVec3> VertexColor;

		vector<FVec4> BlendWeights;
		vector<FVec4> BlendIndices;

		FPose DefaultPose;

		//FTreeNode<FMatrixNode> RootNode;
		FMatrixTreeNode RootNode;

		FMeshData() {}
		~FMeshData() {}

		void FromJson(const FJson& j)
		{
			IndexCount = j[K_INDEX].size();
			VertexCount = j[K_COORDINATE].size();
			VertexFlags = j[K_VERTEX_ELEMENT];
			Bones.reserve(j[K_SKIN].size());
			for (uint32 i = 0; i < j[K_SKIN].size(); ++i)
			{
				Bones.push_back(j[K_SKIN][i]);
			}

			// fill FMeshData(src) first
			if (VertexCount < (1 << 16))
			{
				Indices16.resize(IndexCount);
			}
			else
			{
				Indices32.resize(IndexCount);
			}

			Coordinates.resize(VertexCount);
			UV.resize(VertexCount);
			Normal.resize(VertexCount);
			Tangent.resize(VertexCount);
			Binormal.resize(VertexCount);
			VertexColor.resize(VertexCount);

			if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
			{
				BlendWeights.resize(VertexCount);
				BlendIndices.resize(VertexCount);
			}

			for (uint32 i = 0; i < VertexCount; ++i)
			{
				if (VertexCount < (1 << 16))
				{
					Indices16[i] = j[K_INDEX][i];
				}
				else
				{
					Indices32[i] = j[K_INDEX][i];
				}

				Coordinates[i] = j[K_COORDINATE][i];
				
				if ((VertexFlags & EVertexElement::UV) == EVertexElement::UV)
				{
					UV[i] = j[K_UV][i];
				}
				
				if ((VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
				{
					Normal[i] = j[K_NORMAL][i];
				}
				
				if ((VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
				{
					Tangent[i] = j[K_TANGENT][i];
					Binormal[i] = j[K_BINORMAL][i];
				}
				
				if ((VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
				{
					VertexColor[i] = j[K_VERTEXCOLOR][i];
				}

				if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
				{
					BlendWeights[i] = j[K_BLEND_WEIGHTS][i];
					BlendIndices[i] = j[K_BLEND_INDICES][i];
				}
			}

			auto it = j.find(K_DEFAULT_POSE);
			if (it != j.end())
			{
				const FJson& matrices = it.value()[K_POSE];
				for (const auto& bone : Bones)
				{
					auto matrixNode = matrices.find(bone);
					if (matrixNode != matrices.end())
					{
						DefaultPose[bone] = matrixNode.value();
					}
				}
			}

			auto itStack = j.find(K_STACK);
			if (itStack != j.end())
			{
				RootNode.Load(itStack.value());
			}
		}

		void FromSkinMeshData(const FSkinMeshData& data)
		{
			IndexCount = data.IndexCount;
			VertexCount = data.VertexCount;
			VertexFlags = data.VertexFlags;
			VertexMagic = data.VertexMagic;
			Bones = data.Bones;

			if (VertexCount < (1 << 16))
			{
				Indices16.resize(data.Indices.size() / sizeof(uint16));
				memcpy(&Indices16[0], &data.Indices[0], data.Indices.size());
			}
			else
			{
				Indices32.resize(data.Indices.size() / sizeof(uint32));
				memcpy(&Indices32[0], &data.Indices[0], data.Indices.size());
			}

			uint32 stride = GetVertexDetails(VertexFlags).Stride;
			uint32 paddingBytes = GetPaddingSize(stride, 16);
			uint32 alignedSz = GetAlignedSize(stride, 16);
			bool allocating = true;
			for (uint32 i = 0; i < VertexCount; ++i)
			{
				FBinaryIO stream(const_cast<uint8*>(&data.Vertices[i * alignedSz]), alignedSz);

				if (allocating)
				{
					Coordinates.resize(VertexCount);
				}

				stream >> Coordinates[i];

				if ((VertexFlags & EVertexElement::UV) == EVertexElement::UV)
				{
					if (allocating)
					{
						UV.resize(VertexCount);
					}
					stream >> UV[i];
				}

				if ((VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
				{
					if (allocating)
					{
						Normal.resize(VertexCount);
					}
					stream >> Normal[i];
				}

				if ((VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
				{
					if (allocating)
					{
						Tangent.resize(VertexCount);
						Binormal.resize(VertexCount);
					}
					stream >> Tangent[i] >> Binormal[i];
				}

				if ((VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
				{
					if (allocating)
					{
						VertexColor.resize(VertexCount);
					}
					stream >> VertexColor[i];
				}

				if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
				{
					if (allocating)
					{
						BlendWeights.resize(VertexCount);
						BlendIndices.resize(VertexCount);
					}

					stream >> BlendWeights[i] >> BlendIndices[i];
				}

				allocating = false;
			}

			DefaultPose = data.DefaultPose;
			RootNode = *(FMatrixNode*)&(data.RootNode);
		}
	};

	// serialize
	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const FMeshData& data)
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
	inline FBinaryIO& operator>>(FBinaryIO& stream, FMeshData& data)
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
			FMatrix boneMatrix;
			stream >> boneName >> boneMatrix;
			data.DefaultPose[boneName] = boneMatrix;
		}

		stream >> data.RootNode;

		delete[] padding;
		return stream;
	}
}