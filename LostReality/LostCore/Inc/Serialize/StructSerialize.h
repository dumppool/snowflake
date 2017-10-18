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

	FORCEINLINE uint32 GetAlignedSize(uint32 sz, uint32 alignment)
	{
		return (uint32)ceil(sz / (float)alignment)*alignment;
	}

	FORCEINLINE uint32 GetPaddingSize(uint32 sz, uint32 alignment)
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

		int32 GetDescendantCount() const
		{
			int32 count = 0;
			auto numChildren = Children.size();
			count += numChildren;
			for (int32 childIndex = 0; childIndex < numChildren; ++childIndex)
			{
				count += Children[childIndex].GetChildCount();
			}

			return count;
		}

		int32 GetChildCount() const
		{
			return Children.size();
		}

		const TTreeNode<T>& GetChild(int32 index) const
		{
			assert(index < Children.size());
			return Children[index];
		}

		bool operator==(const TTreeNode<T>& rhs) const
		{
			bool equal = (Data == rhs.Data && Children.size() == rhs.Children.size());

			for (uint32 i=0; i<Children.size(); ++i)
			{
				if (!equal)
				{
					return false;
				}

				equal &= (Children[i] == rhs.Children[i]);
			}

			return equal;
		}

		T Data;
		vector<TTreeNode<T>> Children;
	};

	template<typename T>
	FORCEINLINE FBinaryIO& operator<<(FBinaryIO& stream, const TTreeNode<T>& data)
	{
		stream << data.Data << data.Children.size();
		for (const auto& child : data.Children)
		{
			stream << child;
		}

		return stream;
	}

	template<typename T>
	FORCEINLINE FBinaryIO& operator >> (FBinaryIO& stream, TTreeNode<T>& data)
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

	struct FMatrixNode
	{
		string Name;
		FFloat4x4 Matrix;

		FMatrixNode& operator=(const FMatrixNode& rhs)
		{
			Name = rhs.Name;
			Matrix = rhs.Matrix;
		}

		bool operator==(const FMatrixNode& rhs) const
		{
			return Name.compare(rhs.Name) == 0;
		}
	};


	FORCEINLINE FBinaryIO& operator<<(FBinaryIO& stream, const FMatrixNode& data)
	{
		stream << data.Name << data.Matrix;
		return stream;
	}

	FORCEINLINE FBinaryIO& operator >> (FBinaryIO& stream, FMatrixNode& data)
	{
		stream >> data.Name >> data.Matrix;
		return stream;
	}

	typedef map<string, FFloat4x4> FPoseMap;
	typedef TTreeNode<FMatrixNode> FPoseTree;

	struct FMeshData
	{
		string Name;

		// 测试
		FPoseTree PoseT;

		// 顶点索引数量
		uint32 IndexCount;

		// 顶点数量
		uint32 VertexCount;

		// 顶点结构
		uint32 VertexFlags;

		// 顶点坐标
		vector<FFloat3> Coordinates;

		// 纹理坐标
		string TexCoordName;
		vector<FFloat2> TexCoords;

		// 法线，切线，副法线
		vector<FFloat3> Normals;
		vector<FFloat3> Tangents;
		vector<FFloat3> Binormals;

		// 顶点色
		vector<FColor128> VertexColors;

		// 骨骼索引
		vector<FFloat4>	BlendWeights;

		// 骨骼混合权重
		vector<FSInt4>	BlendIndices;

		// 骨骼
		FPoseTree Skeleton;

		// 骨骼索引表
		map<string, int32> SkeletonIndexMap;

		// 顶点索引到三角面顶点索引的映射
		map<uint32, map<uint32, uint32>> VertexPolygonMap;

		uint32 VertexMagic;

		struct FVertex
		{
			uint32				Index;
			FFloat2				TexCoord;
			FFloat3				Normal;
			FFloat3				Tangent;
			FFloat3				Binormal;
			FColor24			Color;

			bool operator==(const FVertex& rhs) const
			{
				return (Index == rhs.Index
					&& TexCoord == rhs.TexCoord
					&& Normal == rhs.Normal
					&& Tangent == rhs.Tangent
					&& Binormal == rhs.Binormal
					&& Color == rhs.Color);
			}
		};

		struct FTriangle
		{
			FVertex				Vertices[3];

			bool operator==(const FTriangle& rhs) const
			{
				return (Vertices[0] == rhs.Vertices[0]
					&& Vertices[1] == rhs.Vertices[1]
					&& Vertices[2] == rhs.Vertices[2]);
			}
		};

		// 三角面
		vector<FTriangle>		Triangles;

		// 非序列化数据
		vector<uint8> Indices;
		vector<uint8> Vertices;

		FMeshData();

		~FMeshData();

		bool operator==(const FMeshData& rhs) const;

		string Save(const string& outputDir) const;
		void Load(const string& inputDir);

		// 暂时索引没有意义，只转换顶点数据流
		void BuildGPUData(uint32 flags);
	};

	FORCEINLINE FBinaryIO& operator<<(FBinaryIO& stream, const FMeshData& data)
	{
		uint32 sz = 0;
		uint32 dsz = 0;
		stream << data.Name << data.IndexCount << data.VertexCount
			<< data.VertexFlags << data.Coordinates
			<< data.TexCoordName << data.TexCoords
			<< data.Normals << data.Tangents << data.Binormals
			<< data.VertexColors
			<< data.BlendWeights << data.BlendIndices
			<< data.Skeleton
			<< data.SkeletonIndexMap
			<< data.VertexPolygonMap
			<< data.Triangles	// TODO: Triangles的序列化/反序列化应该根据实际数据而定
			;

		stream << data.PoseT;
		stream << (uint32)MAGIC_VERTEX;

		return stream;
	}

	FORCEINLINE FBinaryIO& operator >> (FBinaryIO& stream, FMeshData& data)
	{
		stream >> data.Name >> data.IndexCount >> data.VertexCount
			>> data.VertexFlags >> data.Coordinates 
			>> data.TexCoordName >> data.TexCoords
			>> data.Normals >> data.Tangents >> data.Binormals
			>> data.VertexColors
			>> data.BlendWeights >> data.BlendIndices
			>> data.Skeleton 
			>> data.SkeletonIndexMap
			>> data.VertexPolygonMap
			>> data.Triangles
			;

		stream >> data.PoseT;
		stream >> data.VertexMagic;
		assert(data.VertexMagic == MAGIC_VERTEX && "vertex data is corrupt");

		return stream;
	}

	struct FAnimCurveData
	{
		// Animation name.
		string Name;
		float SampleRate;
		float Length;
		int32 NumKeys;

		// <SkeletonName, <ComponentName, Curve>> map.
		map<string, map<string, FRealCurve>> CurveMap;

		string Save(const string& outputDir) const;
		void Load(const string& inputDir);
	};

	FORCEINLINE FBinaryIO& operator<<(FBinaryIO& stream, const FAnimCurveData& data)
	{
		stream << data.Name << data.SampleRate << data.Length << data.NumKeys << data.CurveMap;
		return stream;
	}

	FORCEINLINE FBinaryIO& operator >> (FBinaryIO& stream, FAnimCurveData& data)
	{
		stream >> data.Name >> data.SampleRate >> data.Length >> data.NumKeys >> data.CurveMap;
		return stream;
	}

	struct FAnimKeyFrameData
	{
		string Name;
		float SampleRate;
		float Length;
		int32 NumKeys;

		map<string, FMatrixCurve> KeyFrameMap;

		string Save(const string& outputDir) const;
		void Load(const string& inputDir);
	};

	FORCEINLINE FBinaryIO& operator<<(FBinaryIO& stream, const FAnimKeyFrameData& data)
	{
		stream << data.Name << data.SampleRate << data.Length << data.NumKeys << data.KeyFrameMap;
		return stream;
	}

	FORCEINLINE FBinaryIO& operator >> (FBinaryIO& stream, FAnimKeyFrameData& data)
	{
		stream >> data.Name >> data.SampleRate >> data.Length >> data.NumKeys >> data.KeyFrameMap;
		return stream;
	}
}

FORCEINLINE LostCore::FMeshData::FMeshData()
	: Name("")
	, IndexCount(0)
	, VertexCount(0)
	, VertexFlags(0)
	, VertexMagic(0)
{
	Coordinates.clear();
	TexCoords.clear();
	Normals.clear();
	Tangents.clear();
	Binormals.clear();
	VertexColors.clear();
	BlendWeights.clear();
	BlendIndices.clear();
	SkeletonIndexMap.clear();
	Triangles.clear();
	Indices.clear();
	Vertices.clear();
}

FORCEINLINE LostCore::FMeshData::~FMeshData()
{
	Coordinates.clear();
	TexCoords.clear();
	Normals.clear();
	Tangents.clear();
	Binormals.clear();
	VertexColors.clear();
	BlendWeights.clear();
	BlendIndices.clear();
	SkeletonIndexMap.clear();
	Triangles.clear();
	Indices.clear();
	Vertices.clear();
}

FORCEINLINE bool LostCore::FMeshData::operator==(const FMeshData& rhs) const
{
	return (Name == rhs.Name
		&& IndexCount == rhs.IndexCount
		&& VertexCount == rhs.VertexCount
		&& VertexFlags == rhs.VertexFlags
		&& Coordinates == rhs.Coordinates
		&& TexCoords == rhs.TexCoords
		&& Normals == rhs.Normals
		&& Tangents == rhs.Tangents
		&& Binormals == rhs.Binormals
		&& VertexColors == rhs.VertexColors
		&& BlendIndices == rhs.BlendIndices
		&& BlendWeights == rhs.BlendWeights
		&& SkeletonIndexMap == rhs.SkeletonIndexMap
		&& Skeleton == rhs.Skeleton
		&& Triangles == rhs.Triangles);
}

FORCEINLINE string LostCore::FMeshData::Save(const string & outputDir) const
{
	if (outputDir.empty())
	{
		return string("");
	}

	auto outputFile = outputDir;
	LostCore::ReplaceChar(outputFile, "/", "\\");
	if (LostCore::IsDirectory(outputFile))
	{
		outputFile = outputDir + Name + "." + K_PRIMITIVE_EXT;
	}
	
	FBinaryIO stream;
	stream << *this;
	stream.WriteToFile(outputFile);

	LVMSG("FMeshData::Save", "Mesh[%s, %.1fKB, %s] is saved[%s]", Name.c_str(),
		stream.RemainingSize() / 1024.f, GetVertexDetails(VertexFlags).Name.c_str(), outputFile.c_str());
	
	return outputFile;
}

FORCEINLINE void LostCore::FMeshData::Load(const string & inputFile)
{
	if (inputFile.empty())
	{
		return;
	}

	FBinaryIO stream;
	stream.ReadFromFile(inputFile);
	uint32 sz = stream.RemainingSize();
	stream >> *this;

	LVMSG("FMeshData::Load", "Mesh[%s, %.1fKB, %s] is loaded[%s]", Name.c_str(),
		sz / 1024.f, GetVertexDetails(VertexFlags).Name.c_str(), inputFile.c_str());
}

// 暂时索引没有意义，只构造顶点数据流
FORCEINLINE void LostCore::FMeshData::BuildGPUData(uint32 flags = 0)
{
	uint32 buildFlags = flags;
	if (flags == 0)
	{
		flags = VertexFlags;
	}
	else if ((flags & ~VertexFlags) != 0)
	{
		LVWARN("FMeshData::BuildGPUData", "Invalid override flags[%s], original flags[%s]",
			GetVertexDetails(flags).Name.c_str(), GetVertexDetails(VertexFlags).Name.c_str());

		flags = VertexFlags;
	}

	// 如果下面split都为空，可以考虑构造索引缓存
	bool splitUV = TexCoords.size() == 0;
	bool splitNormal = Normals.size() == 0;
	bool splitVertexColor = VertexColors.size() == 0;

	Vertices.clear();
	FBinaryIO stream;
	vector<uint8> padding;
	uint32 paddingSz = GetPaddingSize(GetVertexDetails(flags).Stride, 16);
	padding.resize(paddingSz);
	for (const auto& tri : Triangles)
	{
		for (const auto& vert : tri.Vertices)
		{
			stream << Coordinates[vert.Index];

			if (HAS_FLAGS(VERTEX_TEXCOORD0, flags))
			{
				stream << (splitUV ? vert.TexCoord : TexCoords[vert.Index]);
			}

			if (HAS_FLAGS(VERTEX_NORMAL, flags))
			{
				stream << (splitNormal ? vert.Normal : Normals[vert.Index]);
			}

			if (HAS_FLAGS(VERTEX_TANGENT, flags))
			{
				if (splitNormal)
				{
					stream << vert.Tangent << vert.Binormal;
				}
				else
				{
					stream << Tangents[vert.Index] << Binormals[vert.Index];
				}
			}

			if (HAS_FLAGS(VERTEX_COLOR, flags))
			{
				if (splitVertexColor)
				{
					stream << vert.Color;
				}
				else
				{
					stream << VertexColors[vert.Index];
				}
			}

			if (HAS_FLAGS(VERTEX_SKIN, flags))
			{
				stream << BlendWeights[vert.Index] << BlendIndices[vert.Index];
			}

			if (padding.size() > 0)
			{
				Serialize(stream, &padding[0], padding.size());
			}
		}
	}

	Vertices.resize(stream.RemainingSize());
	Deserialize(stream, &Vertices[0], stream.RemainingSize());
}

FORCEINLINE string LostCore::FAnimCurveData::Save(const string& outputDir) const
{
	if (outputDir.empty())
	{
		return "";
	}

	string outputFile = outputDir;
	LostCore::ReplaceChar(outputFile, "/", "\\");
	if (LostCore::IsDirectory(outputFile))
	{
		outputFile += Name + "." + K_ANIM_EXT_CURVE;
	}

	FBinaryIO stream;
	stream << *this;
	stream.WriteToFile(outputFile);

	LVMSG("FAnimCurveData::Save", "Animation is saved: %s, %.1f KB, %s",
		Name.c_str(), stream.RemainingSize() / 1000.0f, outputFile.c_str());

	return outputFile;
}

FORCEINLINE void LostCore::FAnimCurveData::Load(const string& inputDir)
{
	if (inputDir.empty())
	{
		return;
	}

	FBinaryIO stream;
	stream.ReadFromFile(inputDir);
	auto sz = stream.RemainingSize();
	stream >> *this;

	LVMSG("FAnimCurveData::Load", "Animation is loaded: %s, %.1f KB, %s",
		Name.c_str(), sz / 1000.0f, inputDir.c_str());
}


FORCEINLINE string LostCore::FAnimKeyFrameData::Save(const string& outputDir) const
{
	if (outputDir.empty())
	{
		return "";
	}

	string outputFile = outputDir;
	LostCore::ReplaceChar(outputFile, "/", "\\");
	if (LostCore::IsDirectory(outputFile))
	{
		outputFile += Name + "." + K_ANIM_EXT_KEYFRAME;
	}

	FBinaryIO stream;
	stream << *this;
	stream.WriteToFile(outputFile);

	LVMSG("FAnimKeyFrameData::Save", "Animation is saved: %s, %.1f KB, %s",
		Name.c_str(), stream.RemainingSize() / 1000.0f, outputFile.c_str());

	return outputFile;
}

FORCEINLINE void LostCore::FAnimKeyFrameData::Load(const string& inputDir)
{
	if (inputDir.empty())
	{
		return;
	}

	FBinaryIO stream;
	stream.ReadFromFile(inputDir);
	auto sz = stream.RemainingSize();
	stream >> *this;

	LVMSG("FAnimKeyFrameData::Load", "Animation is loaded: %s, %.1f KB, %s",
		Name.c_str(), sz / 1000.0f, inputDir.c_str());
}