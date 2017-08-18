/*
* file BasicStaticMesh.cpp
*
* author luoxw
* date 2017/02/10
*
*
*/

#include "stdafx.h"
#include "BasicModel.h"
#include "PrimitiveGroupInterface.h"
#include "Resource/MeshLoader.h"

using namespace LostCore;


bool LostCore::FBasicModel::Load(IRenderContext * rc, const char* url)
{
	FJson config;
	if (!FDirectoryHelper::Get()->GetModelJson(url, config))
	{
		return false;
	}
	else
	{
		return Config(rc, config);
	}
}

LostCore::FStaticModel::FStaticModel()
	: Primitive(nullptr)
	, Material(nullptr)
{
}

LostCore::FStaticModel::~FStaticModel()
{
	Fini();

	assert(Primitive == nullptr);
	assert(Material == nullptr);
}

bool LostCore::FStaticModel::Config(IRenderContext * rc, const FJson & config)
{
	const char* head = "LostCore::FStaticModel::Config";

	assert(Primitive == nullptr && Material == nullptr && "model should be clear before load anything");
	assert(config.find("type") != config.end() && "model needs [type] section");
	assert((config.find("material") != config.end() || config.find("material_prefix") != config.end()) && "model needs [material] section");

	uint32 vertexFlags = 0;
	auto itType = config.find("type");
	if (itType.value() == (int)EPrimitiveType::PrimitiveFile)
	{
		assert(config.find("primitive") != config.end() && "model needs [primitive] section");

		bool isAbsUrl = config.find("abs_url") != config.end();
		string primitivePath;
		if (!FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(config.find("primitive").value(), primitivePath))
		{
			string path = config.find("primitive").value();
			LVERR(head, "failed to find primitive json: %s", path.c_str());
			return false;
		}

		FBinaryIO stream;
		stream.ReadFromFile(primitivePath.c_str());

		stream >> MeshData;
		vertexFlags = VertexFlags == 0 ? MeshData.VertexFlags : VertexFlags;

		MeshData.BuildGPUData(vertexFlags);

		assert(MeshData.Poses.find(K_INITIAL_POSE) != MeshData.Poses.end());

		if (D3D11::WrappedCreatePrimitiveGroup(&Primitive) == SSuccess)
		{
			if (MeshData.IndexCount > 0)
			{
				uint32 ibStride = MeshData.VertexCount < (1 << 16) ? 2 : 4;
				assert(Primitive->ConstructIB(rc, &(MeshData.Indices[0]), MeshData.Indices.size(), ibStride, false));
			}

			uint32 vbStride = GetAlignedSize(GetVertexDetails(vertexFlags).Stride, 16);
			assert(GetPaddingSize(vbStride, 16) == 0);
			assert(Primitive->ConstructVB(rc, &(MeshData.Vertices[0]), MeshData.Vertices.size(), vbStride, false));
		}
	}
	else
	{
		assert(0 && "Unknown primitive type");
		return false;
	}

	if (D3D11::WrappedCreateMaterial_SceneObject(&Material) == SSuccess)
	{
		if (config.find("material") != config.end())
		{
			string materialPath = config.find("material").value();
			return Material->Initialize(rc, materialPath.c_str());
		}
		else
		{
			string materialPath = config.find("material_prefix").value();
			string vertexName = GetVertexDetails(vertexFlags).Name;
			materialPath = materialPath + "_" + vertexName + ".json";
			return Material->Initialize(rc, materialPath.c_str());
		}
	}
	else
	{
		assert(0 && "create material failed");
		return false;
	}

	return true;
}

void LostCore::FStaticModel::Tick(float sec)
{
}

void LostCore::FStaticModel::Draw(IRenderContext * rc, float sec)
{
	//if (Material != nullptr)
	//{
	//	Material->UpdateConstantBuffer(rc, (const void*)&Matrices, sizeof(Matrices));
	//	Material->Draw(rc, sec);
	//}

	if (Primitive != nullptr)
	{
		Primitive->Draw(rc, sec);
	}

	bool bDisplayNormal = true;
	if (bDisplayNormal)
	{
		SegmentRenderer.Draw(rc, sec);
	}
}

void LostCore::FStaticModel::SetWorldMatrix(const FFloat4x4 & world)
{
}

void LostCore::FStaticModel::SetPrimitiveVertexFlags(uint32 flags)
{
	VertexFlags = flags;
}

void LostCore::FStaticModel::Fini()
{
	if (Primitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(Primitive));
		Primitive = nullptr;
	}

	if (Material != nullptr)
	{
		D3D11::WrappedDestroyMaterial_SceneObject(std::forward<IMaterial*>(Material));
		Material = nullptr;
	}
}

LostCore::FSkeletalModel::FSkeletalModel()
	: Primitive(nullptr)
	, Material(nullptr)
	, VertexFlags(0)
{
}

LostCore::FSkeletalModel::~FSkeletalModel()
{
	Fini();

	assert(Primitive == nullptr);
	assert(Material == nullptr);
}

bool LostCore::FSkeletalModel::Config(IRenderContext * rc, const FJson & config)
{
	const char* head = "LostCore::FSkeletalModel::Config";

	assert(Primitive == nullptr && Material == nullptr && "model should be clear before load anything");
	assert(config.find("type") != config.end() && "model needs [type] section");
	assert((config.find("material") != config.end() || config.find("material_prefix") != config.end()) && "model needs [material] section");

	uint32 vertexFlags = 0;
	auto itType = config.find("type");
	if (itType.value() == (int)EPrimitiveType::PrimitiveFile)
	{
		assert(config.find("primitive") != config.end() && "model needs [primitive] section");

		bool isAbsUrl = config.find("abs_url") != config.end();
		string primitivePath;
		if (!FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(config.find("primitive").value(), primitivePath))
		{
			string path = config.find("primitive").value();
			LVERR(head, "failed to find primitive json: %s", path.c_str());
			return false;
		}

		FBinaryIO stream;
		stream.ReadFromFile(primitivePath.c_str());

		stream >> MeshData;
		vertexFlags = VertexFlags == 0 ? MeshData.VertexFlags : VertexFlags;

		MeshData.BuildGPUData(vertexFlags);

		Root.LoadSkeleton(MeshData.Skeleton);

		assert(MeshData.Poses.find(K_INITIAL_POSE) != MeshData.Poses.end());
		Root.LoadLocalPose(MeshData.Poses[K_INITIAL_POSE]);

		if (D3D11::WrappedCreatePrimitiveGroup(&Primitive) == SSuccess)
		{
			if (MeshData.IndexCount > 0)
			{
				uint32 ibStride = MeshData.VertexCount < (1 << 16) ? 2 : 4;
				assert(Primitive->ConstructIB(rc, &(MeshData.Indices[0]), MeshData.Indices.size(), ibStride, false));
			}

			uint32 vbStride = GetAlignedSize(GetVertexDetails(vertexFlags).Stride, 16);
			assert(GetPaddingSize(vbStride, 16) == 0);
			assert(Primitive->ConstructVB(rc, &(MeshData.Vertices[0]), MeshData.Vertices.size(), vbStride, false));
		}
	}
	else
	{
		assert(0 && "Unknown primitive type");
		return false;
	}

	if (D3D11::WrappedCreateMaterial_SceneObjectSkinned(&Material) == SSuccess)
	{
		if (config.find("material") != config.end())
		{
			string materialPath = config.find("material").value();
			return Material->Initialize(rc, materialPath.c_str());
		}
		else
		{
			string materialPath = config.find("material_prefix").value();
			string vertexName = GetVertexDetails(vertexFlags).Name;
			materialPath = materialPath + "_" + vertexName + ".json";
			return Material->Initialize(rc, materialPath.c_str());
		}
	}
	else
	{
		assert(0 && "create material failed");
		return false;
	}

	return true;
}

void LostCore::FSkeletalModel::Fini()
{
	if (Primitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(Primitive));
		Primitive = nullptr;
	}

	if (Material != nullptr)
	{
		D3D11::WrappedDestroyMaterial_SceneObjectSkinned(std::forward<IMaterial*>(Material));
		Material = nullptr;
	}
}

void LostCore::FSkeletalModel::Tick(float sec)
{
	Root.UpdateWorldMatrix(Matrices.World, sec);

	// 是时候分开Skeleton和非Skeleton了
	LostCore::FPoseMap pose;
	Root.GetWorldPose(pose);
	for (const auto& it : pose)
	{
		if (MeshData.SkeletonIndexMap.find(it.first) != MeshData.SkeletonIndexMap.end())
		{
			Matrices.Bones[MeshData.SkeletonIndexMap[it.first]] = it.second;
		}
	}

	const FColor96 constSegColor((uint32)0x0000ff);
	const float segLen = FGlobalHandler::Get()->GetDisplayNormalLength();
	bool displayTangent = FGlobalHandler::Get()->IsDisplayTangent(MeshData.VertexFlags);
	bool displayNormal = FGlobalHandler::Get()->IsDisplayNormal(MeshData.VertexFlags);
	if (displayNormal || displayTangent)
	{
		for (uint32 i = 0; i < MeshData.Coordinates.size(); ++i)
		{
			FFloat4x4 localToWorld;
			localToWorld.SetZero();
			for (uint32 j = 0; j < 4; ++j)
			{
				if (MeshData.BlendIndices[i][j] >= 0)
				{
					localToWorld = localToWorld + Matrices.Bones[MeshData.BlendIndices[i][j]] * MeshData.BlendWeights[i][j];
				}
				else
				{
					if (j == 0)
					{
						assert(0);
					}
				}
			}

			if (displayNormal)
			{
				FSegmentData seg;
				seg.StartPt = localToWorld.ApplyPoint(MeshData.Coordinates[i]);

				if (MeshData.Normals.size() > 0)
				{
					FFloat3 normal = localToWorld.ApplyVector(MeshData.Normals[i]);
					seg.StopPt = seg.StartPt + normal * segLen;
					seg.Color = constSegColor;
					SegmentRenderer.AddSegment(seg);
				}
				else
				{
					for (auto& elem : MeshData.VertexPolygonMap[i])
					{
						FFloat3 normal = localToWorld.ApplyVector(MeshData.Triangles[elem.first].Vertices[elem.second].Normal);
						seg.StopPt = seg.StartPt + normal * segLen;
						seg.Color = constSegColor;
						SegmentRenderer.AddSegment(seg);
					}
				}
			}

			if (displayTangent)
			{
				FAxisData axis;
				axis.Origin = localToWorld.ApplyPoint(MeshData.Coordinates[i]);

				if (MeshData.Normals.size() > 0)
				{
					FFloat3 normal = localToWorld.ApplyVector(MeshData.Normals[i]);
					FFloat3 tangent = localToWorld.ApplyVector(MeshData.Tangents[i]);
					FFloat3 binormal = localToWorld.ApplyVector(MeshData.Binormals[i]);
					axis.DirX = binormal;
					axis.DirY = normal;
					axis.DirZ = tangent;
					axis.Length = segLen;
					AxisRenderer.AddAxis(axis);
				}
				else
				{
					for (auto& elem : MeshData.VertexPolygonMap[i])
					{
						FFloat3 normal = localToWorld.ApplyVector(MeshData.Triangles[elem.first].Vertices[elem.second].Normal);
						FFloat3 tangent = localToWorld.ApplyVector(MeshData.Triangles[elem.first].Vertices[elem.second].Tangent);
						FFloat3 binormal = localToWorld.ApplyVector(MeshData.Triangles[elem.first].Vertices[elem.second].Binormal);
						axis.DirX = binormal;
						axis.DirY = normal;
						axis.DirZ = tangent;
						axis.Length = segLen;
						AxisRenderer.AddAxis(axis);
					}
				}
			}
		}

		FFloat4x4 w;
		w.SetIdentity();
		if (displayNormal)
		{
			SegmentRenderer.SetWorldMatrix(w);
		}

		if (displayTangent)
		{
			AxisRenderer.SetWorldMatrix(w);
		}
	}
}

void LostCore::FSkeletalModel::Draw(IRenderContext * rc, float sec)
{
	if (Material != nullptr)
	{
		Material->UpdateConstantBuffer(rc, (const void*)&Matrices, sizeof(Matrices));
		Material->Draw(rc, sec);
	}

	if (Primitive != nullptr)
	{
		Primitive->Draw(rc, sec);
	}

	bool displayNormal = FGlobalHandler::Get()->IsDisplayNormal(MeshData.VertexFlags);
	if (displayNormal)
	{
		SegmentRenderer.Draw(rc, sec);
	}

	bool displayTangent = FGlobalHandler::Get()->IsDisplayTangent(MeshData.VertexFlags);
	if (displayTangent)
	{
		AxisRenderer.Draw(rc, sec);
	}
}

void LostCore::FSkeletalModel::SetWorldMatrix(const FFloat4x4 & world)
{
	Matrices.World = world;
}

void LostCore::FSkeletalModel::SetPrimitiveVertexFlags(uint32 flags)
{
	VertexFlags = flags;
}
