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

LostCore::FBasicModel::FBasicModel()
	: Primitive(nullptr)
	, Material(nullptr)
{
}

LostCore::FBasicModel::~FBasicModel()
{
	assert(Primitive == nullptr);
	assert(Material == nullptr);
}

bool LostCore::FBasicModel::Load(IRenderContext * rc, const char* url)
{
	const char* head = "LostCore::FBasicModel::Load";

	FJson modelJson;
	if (!FDirectoryHelper::Get()->GetModelJson(url, modelJson))
	{
		return false;
	}

	assert(Primitive == nullptr && Material == nullptr && "model should be clear before load anything");
	assert(modelJson.find("type") != modelJson.end() && "model needs [type] section");
	assert((modelJson.find("material") != modelJson.end() ||  modelJson.find("material_prefix") != modelJson.end()) && "model needs [material] section");

	auto itType = modelJson.find("type");
	if (itType.value() == (int)EPrimitiveType::Skeleton)
	{
		assert(modelJson.find("primitive") != modelJson.end() && "model needs [primitive] section");

		string primitivePath;
		if (!FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(modelJson.find("primitive").value(), primitivePath))
		{
			string path = modelJson.find("primitive").value();
			LVERR(head, "failed to find primitive json: %s", path.c_str());
			return false;
		}

		FBinaryIO stream;
		stream.ReadFromFile(primitivePath.c_str());

		stream >> MeshData;
		MeshData.BuildGPUData();

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

			uint32 vbStride = GetAlignedSize(GetVertexDetails(MeshData.VertexFlags).Stride, 16);
			assert(GetPaddingSize(vbStride, 16) == 0);
			assert(Primitive->ConstructVB(rc, &(MeshData.Vertices[0]), MeshData.Vertices.size(), vbStride, false));
		}
	}
	else
	{
		assert(0 && "Unknown primitive type");
		return false;
	}

	if (((MeshData.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin && D3D11::WrappedCreateMaterial_SceneObjectSkinned(&Material) == SSuccess) ||
		((MeshData.VertexFlags & EVertexElement::Skin) != EVertexElement::Skin && D3D11::WrappedCreateMaterial_SceneObject(&Material) == SSuccess))
	{
		if (modelJson.find("material") != modelJson.end())
		{
			string materialPath = modelJson.find("material").value();
			return Material->Initialize(rc, materialPath.c_str());
		}
		else
		{
			string materialPath = modelJson.find("material_prefix").value();
			string vertexName = GetVertexDetails(MeshData.VertexFlags).Name;
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

void LostCore::FBasicModel::Fini()
{
	if (Primitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(Primitive));
		Primitive = nullptr;
	}

	if (Material != nullptr)
	{
		if ((MeshData.VertexFlags & EVertexElement::Skin) == EVertexElement::Skin)
		{
			D3D11::WrappedDestroyMaterial_SceneObjectSkinned(std::forward<IMaterial*>(Material));
		}
		else
		{
			D3D11::WrappedDestroyMaterial_SceneObject(std::forward<IMaterial*>(Material));
		}

		Material = nullptr;
	}
}

void LostCore::FBasicModel::Tick(float sec)
{
	Root.UpdateWorldMatrix(Matrices.World);

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

	const float constSegLength = 1.1f;
	const FColor96 constSegColor((uint32)0x0000ff);
	bool bDisplayNormal = true;
	if (bDisplayNormal)
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

			FSegmentData seg;
			seg.StartPt = localToWorld.ApplyPoint(MeshData.Coordinates[i]);
			FFloat3 normal;
			if (MeshData.Normals.size() > 0)
			{
				normal = localToWorld.ApplyVector(MeshData.Normals[i]);
			}

			seg.StopPt = seg.StartPt + normal * constSegLength;
			seg.Color = constSegColor;

			SegmentRenderer.AddSegment(seg);
		}

		FFloat4x4 w;
		w.SetIdentity();
		SegmentRenderer.SetWorldMatrix(w);
	}
}

void LostCore::FBasicModel::Draw(IRenderContext * rc, float sec)
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

	bool bDisplayNormal = true;
	if (bDisplayNormal)
	{
		SegmentRenderer.Draw(rc, sec);
	}
}

void LostCore::FBasicModel::SetWorldMatrix(const FFloat4x4 & world)
{
	Matrices.World = world;
}
