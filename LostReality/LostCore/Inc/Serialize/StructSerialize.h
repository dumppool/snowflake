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

	// ���ڵ�ģ����
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
		FFloat4x4 Matrix;
	};

	typedef map<string, FFloat4x4> FPoseMap;
	typedef TTreeNode<FMatrixNode> FPoseTree;


	struct FMeshData
	{
		string Name;

		// ������������
		uint32 IndexCount;

		// ��������
		uint32 VertexCount;

		// ����ṹ
		uint32 VertexFlags;

		// ��������
		vector<FFloat3> Coordinates;

		// ��������
		vector<FFloat2> TexCoords;

		// ���ߣ����ߣ�������
		vector<FFloat3> Normals;
		vector<FFloat3> Tangents;
		vector<FFloat3> Binormals;

		// ����ɫ
		vector<FColor96> VertexColors;

		// ��������
		vector<FFloat4>	BlendWeights;

		// �������Ȩ��
		vector<FSInt4>	BlendIndices;

		map<string, FPoseMap> Poses;

		// ����
		FBone Skeleton;

		// ����������
		map<string, int32> SkeletonIndexMap;

		uint32 VertexMagic;

		struct FVertex
		{
			uint32				Index;
			FFloat2				TexCoord;
			FFloat3				Normal;
			FFloat3				Tangent;
			FFloat3				Binormal;
			FColor96			Color;

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

		// ������
		vector<FTriangle>		Triangles;

		// �����л�����
		vector<uint8> Indices;
		vector<uint8> Vertices;

		FMeshData();

		~FMeshData();

		bool operator==(const FMeshData& rhs) const;

		void Save(const string& outputDir) const;
		void Load(const string& inputDir);

		// ��ʱ����û�����壬ֻת������������
		void BuildGPUData();
	};

	template<>
	inline FBinaryIO& operator<<(FBinaryIO& stream, const FMeshData& data)
	{
		uint32 sz = 0;
		uint32 dsz = 0;
		stream << data.Name << data.IndexCount << data.VertexCount 
			<< data.VertexFlags << data.Coordinates << data.TexCoords
			<< data.Normals << data.Tangents << data.Binormals
			<< data.VertexColors
			<< data.BlendWeights << data.BlendIndices
			<< data.Poses
			<< data.Skeleton
			<< data.SkeletonIndexMap
			<< data.Triangles	// TODO: Triangles�����л�/�����л�Ӧ�ø���ʵ�����ݶ���
			;

		stream << (uint32)MAGIC_VERTEX;

		return stream;
	}

	template<>
	inline FBinaryIO& operator >> (FBinaryIO& stream, FMeshData& data)
	{
		stream >> data.Name >> data.IndexCount >> data.VertexCount
			>> data.VertexFlags >> data.Coordinates >> data.TexCoords
			>> data.Normals >> data.Tangents >> data.Binormals
			>> data.VertexColors
			>> data.BlendWeights >> data.BlendIndices
			>> data.Poses >> data.Skeleton >> data.SkeletonIndexMap
			>> data.Triangles
			;

		stream >> data.VertexMagic;
		assert(data.VertexMagic == MAGIC_VERTEX && "vertex data is corrupt");

		return stream;
	}
}

inline LostCore::FMeshData::FMeshData()
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
	Poses.clear();
	SkeletonIndexMap.clear();
	Triangles.clear();
	Indices.clear();
	Vertices.clear();
}

inline LostCore::FMeshData::~FMeshData()
{
	Coordinates.clear();
	TexCoords.clear();
	Normals.clear();
	Tangents.clear();
	Binormals.clear();
	VertexColors.clear();
	BlendWeights.clear();
	BlendIndices.clear();
	Poses.clear();
	SkeletonIndexMap.clear();
	Triangles.clear();
	Indices.clear();
	Vertices.clear();
}

inline bool LostCore::FMeshData::operator==(const FMeshData& rhs) const
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
		&& Poses == rhs.Poses
		&& SkeletonIndexMap == rhs.SkeletonIndexMap
		&& Skeleton == rhs.Skeleton
		&& Triangles == rhs.Triangles);
}

inline void LostCore::FMeshData::Save(const string & outputDir) const
{
	if (outputDir.empty())
	{
		return;
	}

	auto outputFile = outputDir + Name + "." + K_PRIMITIVE;
	FBinaryIO stream;
	stream << *this;
	stream.WriteToFile(outputFile);

	LVMSG("FMeshData::Save", "Mesh[%s, %.1fKB, %s] is saved[%s]", Name.c_str(),
		stream.RemainingSize() / 1024.f, GetVertexDetails(VertexFlags).Name.c_str(), outputFile.c_str());
}

inline void LostCore::FMeshData::Load(const string & inputFile)
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

// ��ʱ����û�����壬ֻ���춥��������
inline void LostCore::FMeshData::BuildGPUData()
{
	// �������split��Ϊ�գ����Կ��ǹ�����������
	bool splitUV = TexCoords.size() == 0;
	bool splitNormal = Normals.size() == 0;
	bool splitVertexColor = VertexColors.size() == 0;

	Vertices.clear();
	FBinaryIO stream;
	vector<uint8> padding;
	uint32 paddingSz = GetPaddingSize(GetVertexDetails(VertexFlags).Stride, 16);
	padding.resize(paddingSz);
	for (const auto& tri : Triangles)
	{
		for (const auto& vert : tri.Vertices)
		{
			stream << Coordinates[vert.Index];

			if ((VertexFlags & EVertexElement::UV) == EVertexElement::UV)
			{
				stream << (splitUV ? vert.TexCoord : TexCoords[vert.Index]);
			}

			if ((VertexFlags & EVertexElement::Normal) == EVertexElement::Normal)
			{
				stream << (splitNormal ? vert.Normal : Normals[vert.Index]);
			}

			if ((VertexFlags & EVertexElement::Tangent) == EVertexElement::Tangent)
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

			if ((VertexFlags & EVertexElement::VertexColor) == EVertexElement::VertexColor)
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

			if ((VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
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
