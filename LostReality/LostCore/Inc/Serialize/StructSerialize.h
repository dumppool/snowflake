/*
* file StructSerialize.h
*
* author luoxw
* date 2017/05/26
*
*
*/

#pragma once

namespace LostCore
{
	inline FVertexTypes::Details GetVertexDetails(int flag)
	{
		return LostCore::FVertexTypes::GetVertexDetail3D(
			(flag & EVertexElement::UV) == EVertexElement::UV,
			(flag & EVertexElement::Normal) == EVertexElement::Normal,
			(flag & EVertexElement::Tangent) == EVertexElement::Tangent,
			(flag & EVertexElement::VertexColor) == EVertexElement::VertexColor,
			(flag & EVertexElement::Skin) == EVertexElement::Skin);
	}

	inline uint32 GetAlignedSize(uint32 sz, uint32 alignment)
	{
		return (uint32)ceil(sz / (float)alignment)*alignment;
	}

	inline uint32 GetPaddingSize(uint32 sz, uint32 alignment)
	{
		return GetAlignedSize(sz, alignment) - sz;
	}

	// 树节点模板类
	template<typename T>
	struct TTreeNode
	{
		TTreeNode() 
		{
		}

		void SetData(const T& data)
		{
			Data = data;
		}

		void AddChild(const TTreeNode<T>& node)
		{
			Children.push_back(node);
		}

		T Data;
		vector<TTreeNode<T>> Children;
	};

	template<typename T>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const TTreeNode<T>& data)
	{
		stream << data.Data << data.Children.size();
		for (const auto& child : data.Children)
		{
			stream << child;
		}

		return stream;
	}

	template<typename T>
	inline FBinaryIO& operator >> (FBinaryIO& stream, TTreeNode<T>& data)
	{
		uint32 childNum;
		stream >> data.Data >> childNum;
		for (uint32 i = 0; i < childNum; ++i)
		{
			data.Children.push_back(TTreeNode<T>());
			stream >> *(data.Children.end() - 1);
		}

		return stream;
	}

	typedef TTreeNode<string> FBone;

	struct FMatrixNode
	{
		string Name;
		FMatrix Matrix;
	};

	typedef map<string, FMatrix> FPoseMap;
	typedef TTreeNode<FMatrixNode> FPoseTree;


	// 提供面向图形API的数据格式
	struct FMeshDataGPU
	{
		string Name;

		uint32 IndexCount;
		uint32 VertexCount;
		uint32 VertexFlags;

		vector<uint8> Indices;
		vector<uint8> Vertices;

		map<string, FPoseMap> Poses;
		FBone Skeleton;
		map<string, int32> SkeletonIndexMap;

		uint32 VertexMagic;

		FMeshDataGPU() 
		{
			Indices.clear();
			Vertices.clear();
			Poses.clear();
			SkeletonIndexMap.clear();
		}

		~FMeshDataGPU() 
		{
			Indices.clear();
			Vertices.clear();
			Poses.clear();
			SkeletonIndexMap.clear();
		}
	};

	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const FMeshDataGPU& data)
	{
		stream << data.Name << data.IndexCount << data.VertexCount << data.VertexFlags;

		if (data.IndexCount > 0)
		{
			uint32 ibSz = data.IndexCount * (data.VertexCount < (1 << 16) ? 2 : 4);
			Serialize(stream, &(data.Indices[0]), ibSz);
		}

		if (data.VertexCount > 0)
		{
			stream << data.Vertices.size();
			Serialize(stream, &(data.Vertices[0]), data.Vertices.size());
		}

		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			stream << data.Poses;
			stream << data.Skeleton;
			stream << data.SkeletonIndexMap;
		}

		stream << (uint32)MAGIC_VERTEX;

		return stream;
	}

	template<>
	inline FBinaryIO& operator >> (FBinaryIO& stream, FMeshDataGPU& data)
	{
		stream >> data.Name >> data.IndexCount >> data.VertexCount >> data.VertexFlags;

		if (data.IndexCount > 0)
		{
			uint32 ibSz = data.IndexCount * (data.VertexCount < (1 << 16) ? 2 : 4);
			data.Indices.resize(ibSz);
			Deserialize(stream, &(data.Indices[0]), ibSz);
		}

		if (data.VertexCount > 0)
		{
			uint32 sz;
			stream >> sz;
			data.Vertices.resize(sz);
			Deserialize(stream, &(data.Vertices[0]), sz);
		}

		if ((data.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			data.Poses.clear();
			data.SkeletonIndexMap.clear();
			stream >> data.Poses;
			stream >> data.Skeleton;
			stream >> data.SkeletonIndexMap;
		}

		stream >> data.VertexMagic;
		assert(data.VertexMagic == MAGIC_VERTEX && "vertex data is incorrect");

		return stream;
	}
}