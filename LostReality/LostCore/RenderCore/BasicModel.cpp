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
	: Url("")
	, Primitive(nullptr)
	, Material(nullptr)
	, MatricesBuffer(nullptr)
	, CustomBuffer(nullptr)
	, ActorFlags(0)
{
}

LostCore::FBasicModel::~FBasicModel()
{
	Fini();
}

bool LostCore::FBasicModel::Config(const FJson & config)
{
	const char* head = "LostCore::FBasicModel::Config";

	if (config.find("primitive") == config.end())
	{
		LVERR(head, "Config file need [%s] section.", "primitive");
		return false;
	}

	if (!ConfigPrimitive(config["primitive"], Primitive, PrimitiveData))
	{
		return false;
	}

	// 可以指定material文件
	// 也可以只指定文件前缀，根据primitive结构自动完成material文件名
	string materialConfig;
	if (config.find(K_MATERIAL) != config.end())
	{
		materialConfig = config[K_MATERIAL].get<string>();
	}
	else if (config.find(K_AUTO) != config.end())
	{
		string vertexName = GetVertexDetails(PrimitiveData.VertexFlags).Name;
		materialConfig = config[K_AUTO].get<string>().append("_").append(vertexName).append(".json");
	}
	else
	{
		LVERR(head, "Config file need [%s] or [%s] section.", K_MATERIAL, K_AUTO);
		return false;
	}

	if (!ConfigMaterial(materialConfig))
	{
		return false;
	}

	return true;
}

FJson LostCore::FBasicModel::Save()
{
	FJson config;
	config[K_TYPE] = (int32)ESceneNodeType::Model;
	config[K_PATH] = Url;
	config[K_TRANSFORM] = GetWorldMatrix();
	return config;
}

void LostCore::FBasicModel::Tick()
{
	UpdateConstant();
	UpdateGizmosBoundingBox();
}

void LostCore::FBasicModel::Draw()
{
	DrawModel();
	DrawGizmos();
}

void LostCore::FBasicModel::DrawModel()
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	if (MatricesBuffer != nullptr)
	{
		MatricesBuffer->Bind(rc, 1 | SHADER_SLOT_VS);
	}

	if (CustomBuffer != nullptr)
	{
		CustomBuffer->Bind(rc, 2 | SHADER_SLOT_VS | SHADER_SLOT_PS);
	}

	if (Material != nullptr)
	{
		Material->Bind(rc);
	}

	if (Primitive != nullptr)
	{
		Primitive->Draw(rc, sec);
	}
}

bool LostCore::FBasicModel::ConfigPrimitive(const string& url, IPrimitiveGroup*& pg, FMeshData& pgdata)
{
	const char* head = "FBasicModel::ConfigPrimitive";
	auto rc = FGlobalHandler::Get()->GetRenderContext();

	// 获取primitive文件路径.
	string urlAbs;
	if (!FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(url, urlAbs))
	{
		LVERR(head, "Failed to find primitive json: %s", url.c_str());
		return false;
	}

	// 构造渲染数据.
	pgdata.Load(urlAbs);
	pgdata.BuildGPUData();

	// 构造包围盒数据.
	ValidateBoundingBox();

	// 根据渲染数据创建索引/定点缓存.
	if (D3D11::WrappedCreatePrimitiveGroup(&pg) == SSuccess)
	{
		if (pgdata.IndexCount > 0)
		{
			uint32 ibStride = pgdata.VertexCount < (1 << 16) ? 2 : 4;
			assert(pg->ConstructIB(rc, &(pgdata.Indices[0]), pgdata.Indices.size(), ibStride, false));
		}

		uint32 vbStride = GetAlignedSize(GetVertexDetails(pgdata.VertexFlags).Stride, 16);
		assert(pg->ConstructVB(rc, &(pgdata.Vertices[0]), pgdata.Vertices.size(), vbStride, false));
	}

	return pg != nullptr;
}

bool LostCore::FBasicModel::ConfigMaterial(const string& url)
{
	bool success = true;
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	success &= D3D11::WrappedCreateConstantBuffer(&MatricesBuffer) == SSuccess;
	success &= D3D11::WrappedCreateConstantBuffer(&CustomBuffer) == SSuccess && CustomBuffer->Initialize(rc, sizeof(Custom), false);
	success &= D3D11::WrappedCreateMaterial(&Material) == SSuccess && Material->Initialize(rc, url.c_str());
	return success;
}

void LostCore::FBasicModel::UpdateConstant()
{
	if (CustomBuffer != nullptr)
	{
		CustomBuffer->UpdateBuffer(FGlobalHandler::Get()->GetRenderContext(), &Custom.GetBuffer(), sizeof(Custom));
	}
}

void LostCore::FBasicModel::UpdateGizmosBoundingBox()
{
	SegmentRenderer.ResetData();
	if (FGlobalHandler::Get()->IsDisplay(FLAG_DISPLAY_BB) || BoundingBox.bVisible)
	{
		auto world = GetWorldMatrix();
		auto ptMin = world.ApplyPoint(BoundingBox.Min);
		auto ptMax = world.ApplyPoint(BoundingBox.Max);

		const FColor96 col((uint32)0xffff00);
		FSegmentData seg;
		seg.StartPtColor = seg.StopPtColor = col;

		// Different x
		seg.StartPt = FFloat3(ptMin.X, ptMin.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMin.Y, ptMin.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMin.X, ptMax.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMax.Y, ptMin.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMin.X, ptMin.Y, ptMax.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMin.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMin.X, ptMax.Y, ptMax.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMax.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		// Different y
		seg.StartPt = FFloat3(ptMin.X, ptMin.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMin.X, ptMax.Y, ptMin.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMin.X, ptMin.Y, ptMax.Z);
		seg.StopPt = FFloat3(ptMin.X, ptMax.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMax.X, ptMin.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMax.Y, ptMin.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMax.X, ptMin.Y, ptMax.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMax.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		// Different z
		seg.StartPt = FFloat3(ptMin.X, ptMin.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMin.X, ptMin.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMin.X, ptMax.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMin.X, ptMax.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMax.X, ptMin.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMin.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);

		seg.StartPt = FFloat3(ptMax.X, ptMax.Y, ptMin.Z);
		seg.StopPt = FFloat3(ptMax.X, ptMax.Y, ptMax.Z);
		SegmentRenderer.AddSegment(seg);
	}
}

void LostCore::FBasicModel::DrawGizmos()
{
	SegmentRenderer.Draw();
}

IPrimitiveGroup * LostCore::FBasicModel::GetPrimitive()
{
	return Primitive;
}

IMaterial * LostCore::FBasicModel::GetMaterial()
{
	return Material;
}

IConstantBuffer * LostCore::FBasicModel::GetMatricesBuffer()
{
	return MatricesBuffer;
}

IConstantBuffer * LostCore::FBasicModel::GetCustomBuffer()
{
	return CustomBuffer;
}

FMeshData * LostCore::FBasicModel::GetPrimitiveData()
{
	return &PrimitiveData;
}

void LostCore::FBasicModel::SetColor(const FColor128 & color)
{
	Custom.Color = color;
}

bool LostCore::FBasicModel::RayTest(const FRay & ray, FRay::FT & dist)
{
	return RayBoxIntersect(ray, BoundingBox, GetWorldMatrix().GetInvert(), dist);
}

FSegmentTool * LostCore::FBasicModel::GetSegmentRenderer()
{
	return &SegmentRenderer;
}

void LostCore::FBasicModel::Clone(FBasicModel & model)
{
}

void LostCore::FBasicModel::EnableDepthTest(bool depthTest)
{
	if (Material != nullptr)
	{
		Material->SetDepthStencilState(depthTest ? K_DEPTH_STENCIL_Z_WRITE : K_DEPTH_STENCIL_ALWAYS);
	}
}

void LostCore::FBasicModel::EnableFlags(uint32 flags)
{
	ActorFlags |= flags;
}

void LostCore::FBasicModel::DisableFlags(uint32 flags)
{
	ActorFlags &= ~flags;
}

bool LostCore::FBasicModel::HasFlags(uint32 flags) const
{
	return HAS_FLAGS(flags, ActorFlags);
}

void LostCore::FBasicModel::SetUrl(const string & url)
{
	FDirectoryHelper::Get()->GetModelRelativePath(url, Url);
}

const string & LostCore::FBasicModel::GetUrl() const
{
	return Url;
}

FAABoundingBox * LostCore::FBasicModel::GetBoundingBox()
{
	return &BoundingBox;
}

void LostCore::FBasicModel::ValidateBoundingBox()
{
	if (BoundingBox.IsValid())
	{
		return;
	}

	for (auto& coord : PrimitiveData.Coordinates)
	{
		BoundingBox.AddPoint(coord);
	}
}

void LostCore::FBasicModel::Fini()
{
	if (Primitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(forward<IPrimitiveGroup*>(Primitive));
		Primitive = nullptr;
	}

	if (Material != nullptr)
	{
		D3D11::WrappedDestroyMaterial(forward<IMaterial*>(Material));
		Material = nullptr;
	}

	if (MatricesBuffer != nullptr)
	{
		D3D11::WrappedDestroyConstantBuffer(forward<IConstantBuffer*>(MatricesBuffer));
		MatricesBuffer = nullptr;
	}

	if (CustomBuffer != nullptr)
	{
		D3D11::WrappedDestroyConstantBuffer(forward<IConstantBuffer*>(CustomBuffer));
		CustomBuffer = nullptr;
	}
}

LostCore::FStaticModel::FStaticModel() : FBasicModel()
{
}

LostCore::FStaticModel::~FStaticModel()
{
	Fini();
}

void LostCore::FStaticModel::Tick()
{
	FBasicModel::Tick();
	UpdateGizmosNormalTangent();
}

void LostCore::FStaticModel::SetWorldMatrix(const FFloat4x4 & world)
{
	World.Matrix = world;
}

FFloat4x4 LostCore::FStaticModel::GetWorldMatrix()
{
	return World.Matrix;
}

void LostCore::FStaticModel::Clone(FBasicModel & model)
{
	FBasicModel::Clone(model);
}

bool LostCore::FStaticModel::ConfigMaterial(const string & url)
{
	bool success = FBasicModel::ConfigMaterial(url);
	auto cb = GetMatricesBuffer();
	if (cb != nullptr)
	{
		success &= cb->Initialize(FGlobalHandler::Get()->GetRenderContext(), sizeof(World), false);
	}

	return success;
}

void LostCore::FStaticModel::UpdateConstant()
{
	FBasicModel::UpdateConstant();
	auto cb = GetMatricesBuffer();
	if (cb != nullptr)
	{
		cb->UpdateBuffer(FGlobalHandler::Get()->GetRenderContext(), (const void*)&World.GetBuffer(), sizeof(World));
	}
}

void LostCore::FStaticModel::DrawGizmos()
{
	FBasicModel::DrawGizmos();
	AxisRenderer.Draw();
}

void LostCore::FStaticModel::UpdateGizmosNormalTangent()
{
	FSegmentTool& renderer = *GetSegmentRenderer();
	AxisRenderer.ResetData();

	FMeshData& prim = *GetPrimitiveData();
	bool displayTangent =
		HAS_FLAGS(VERTEX_TANGENT, prim.VertexFlags) &&
		HAS_FLAGS(FLAG_DISPLAY_TANGENT, FGlobalHandler::Get()->GetDisplayFlags());

	bool displayNormal = !displayTangent &&
		HAS_FLAGS(VERTEX_NORMAL, prim.VertexFlags)&&
		HAS_FLAGS(FLAG_DISPLAY_NORMAL, FGlobalHandler::Get()->GetDisplayFlags());

	if (!displayNormal && !displayTangent)
	{
		return;
	}

	const FColor96 normalColor((uint32)0x0000ff);
	const FColor96 endColor((uint32)0xffffff);
	const float segLen = FGlobalHandler::Get()->GetDisplayNormalLength();
	auto& world = World.Matrix;
	for (uint32 i = 0; i < prim.Coordinates.size(); ++i)
	{
		if (displayNormal)
		{
			FSegmentData seg;
			seg.StartPt = world.ApplyPoint(prim.Coordinates[i]);
			seg.StartPtColor = normalColor;
			seg.StopPtColor = endColor;

			if (prim.Normals.size() > 0)
			{
				FFloat3 normal = world.ApplyVector(prim.Normals[i]);
				seg.StopPt = seg.StartPt + normal * segLen;
				renderer.AddSegment(seg);
			}
			else
			{
				for (const auto& elem : prim.VertexPolygonMap[i])
				{
					FFloat3 normal = world.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Normal);
					seg.StopPt = seg.StartPt + normal * segLen;
					renderer.AddSegment(seg);
				}
			}
		}

		if (displayTangent)
		{
			FAxisData axis;
			axis.Origin = world.ApplyPoint(prim.Coordinates[i]);

			if (prim.Normals.size() > 0)
			{
				FFloat3 normal = world.ApplyVector(prim.Normals[i]);
				FFloat3 tangent = world.ApplyVector(prim.Tangents[i]);
				FFloat3 binormal = world.ApplyVector(prim.Binormals[i]);
				axis.DirX = binormal;
				axis.DirY = normal;
				axis.DirZ = tangent;
				axis.Length = segLen;
				AxisRenderer.AddAxis(axis);
			}
			else
			{
				for (auto& elem : prim.VertexPolygonMap[i])
				{
					FFloat3 normal = world.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Normal);
					FFloat3 tangent = world.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Tangent);
					FFloat3 binormal = world.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Binormal);
					axis.DirX = binormal;
					axis.DirY = normal;
					axis.DirZ = tangent;
					axis.Length = segLen;
					AxisRenderer.AddAxis(axis);
				}
			}
		}
	}
}

void LostCore::FStaticModel::Fini()
{
}

LostCore::FSkeletalModel::FSkeletalModel() : FBasicModel()
{
}

LostCore::FSkeletalModel::~FSkeletalModel()
{
	Fini();
}

void LostCore::FSkeletalModel::Fini()
{
}

void LostCore::FSkeletalModel::PlayAnimation(const string & animName)
{
	Root.SetAnimation(animName);
}

void LostCore::FSkeletalModel::Tick()
{

	FBasicModel::Tick();
	UpdateGizmosNormalTangent();
	UpdateGizmosSkeleton();
}

void LostCore::FSkeletalModel::SetWorldMatrix(const FFloat4x4 & world)
{
	Matrices.World = world;
}

FFloat4x4 LostCore::FSkeletalModel::GetWorldMatrix()
{
	return Matrices.World;
}

void LostCore::FSkeletalModel::Clone(FBasicModel & model)
{
	FBasicModel::Clone(model);
}

void LostCore::FSkeletalModel::UpdateConstant()
{
	FBasicModel::UpdateConstant();
	auto& prim = *GetPrimitiveData();
	Root.UpdateWorldMatrix(Matrices.World);

	LostCore::FPoseMap pose;
	Root.GetWorldPose(pose);
	for (const auto& it : pose)
	{
		if (prim.SkeletonIndexMap.find(it.first) != prim.SkeletonIndexMap.end())
		{
			Matrices.Bones[prim.SkeletonIndexMap[it.first]] = it.second;
		}
	}

	auto cb = GetMatricesBuffer();
	if (cb != nullptr)
	{
		cb->UpdateBuffer(FGlobalHandler::Get()->GetRenderContext(), (const void*)&Matrices.GetBuffer(), sizeof(Matrices));
	}
}

bool LostCore::FSkeletalModel::ConfigPrimitive(const string& url, IPrimitiveGroup*& pg, FMeshData& pgdata)
{
	if (FBasicModel::ConfigPrimitive(url, pg, pgdata))
	{
		FFloat4x4 world;
		world.SetIdentity();
		Root.LoadSkeleton(pgdata.Skeleton, world);
		return true;
	}
	
	return false;
}

bool LostCore::FSkeletalModel::ConfigMaterial(const string & url)
{
	bool success = FBasicModel::ConfigMaterial(url);
	SkeletonRenderer.EnableDepthTest(false);
	auto cb = GetMatricesBuffer();
	if (cb != nullptr)
	{
		success &= cb->Initialize(FGlobalHandler::Get()->GetRenderContext(), sizeof(Matrices), false);
	}

	return success;
}

void LostCore::FSkeletalModel::DrawGizmos()
{
	FBasicModel::DrawGizmos();
	AxisRenderer.Draw();
	SkeletonRenderer.Draw();
}

void LostCore::FSkeletalModel::UpdateGizmosNormalTangent()
{
	FSegmentTool& renderer = *GetSegmentRenderer();
	AxisRenderer.ResetData();
	SkeletonRenderer.ResetData();

	FMeshData& prim = *GetPrimitiveData();
	bool displayTangent = 
		HAS_FLAGS(VERTEX_TANGENT, prim.VertexFlags) &&
		HAS_FLAGS(FLAG_DISPLAY_TANGENT, FGlobalHandler::Get()->GetDisplayFlags());

	bool displayNormal = !displayTangent &&
		HAS_FLAGS(VERTEX_NORMAL, prim.VertexFlags) &&
		HAS_FLAGS(FLAG_DISPLAY_NORMAL, FGlobalHandler::Get()->GetDisplayFlags());

	if (!displayNormal && !displayTangent)
	{
		return;
	}

	const FColor96 normalColor((uint32)0x0000ff);
	const FColor96 endColor((uint32)0xffffff);
	const float segLen = FGlobalHandler::Get()->GetDisplayNormalLength();
	for (uint32 i = 0; i < prim.Coordinates.size(); ++i)
	{
		FFloat4x4 localToWorld;
		localToWorld.SetZero();
		for (uint32 j = 0; j < 4; ++j)
		{
			if (prim.BlendIndices[i][j] >= 0)
			{
				localToWorld = localToWorld + Matrices.Bones[prim.BlendIndices[i][j]] * prim.BlendWeights[i][j];
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
			seg.StartPt = localToWorld.ApplyPoint(prim.Coordinates[i]);
			seg.StartPtColor = normalColor;
			seg.StopPtColor = endColor;

			if (prim.Normals.size() > 0)
			{
				FFloat3 normal = localToWorld.ApplyVector(prim.Normals[i]);
				seg.StopPt = seg.StartPt + normal * segLen;
				renderer.AddSegment(seg);
			}
			else
			{
				for (auto& elem : prim.VertexPolygonMap[i])
				{
					FFloat3 normal = localToWorld.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Normal);
					seg.StopPt = seg.StartPt + normal * segLen;
					renderer.AddSegment(seg);
				}
			}
		}

		if (displayTangent)
		{
			FAxisData axis;
			axis.Origin = localToWorld.ApplyPoint(prim.Coordinates[i]);

			if (prim.Normals.size() > 0)
			{
				FFloat3 normal = localToWorld.ApplyVector(prim.Normals[i]);
				FFloat3 tangent = localToWorld.ApplyVector(prim.Tangents[i]);
				FFloat3 binormal = localToWorld.ApplyVector(prim.Binormals[i]);
				axis.DirX = binormal;
				axis.DirY = normal;
				axis.DirZ = tangent;
				axis.Length = segLen;
				AxisRenderer.AddAxis(axis);
			}
			else
			{
				for (auto& elem : prim.VertexPolygonMap[i])
				{
					FFloat3 normal = localToWorld.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Normal);
					FFloat3 tangent = localToWorld.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Tangent);
					FFloat3 binormal = localToWorld.ApplyVector(prim.Triangles[elem.first].Vertices[elem.second].Binormal);
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
		renderer.SetWorldMatrix(w);
	}

	if (displayTangent)
	{
		AxisRenderer.SetWorldMatrix(w);
	}
}

void LostCore::FSkeletalModel::UpdateGizmosSkeleton()
{
	if (FGlobalHandler::Get()->IsDisplay(FLAG_DISPLAY_SKEL))
	{
		map<string, pair<FFloat3, vector<FFloat3>>> skels;

		Root.GetSkeletonRenderData(skels);
		for (auto& skel : skels)
		{
			FSegmentData seg;
			seg.StartPt = skel.second.first;
			seg.StartPtColor = FColor96((uint32)0x80ff40);
			seg.StopPtColor = FColor96((uint32)0xffffff);
			for (auto & childSkel : skel.second.second)
			{
				seg.StopPt = childSkel;
				SkeletonRenderer.AddSegment(seg);
			}
		}

		FFloat4x4 mat;
		mat.SetIdentity();
		SkeletonRenderer.SetWorldMatrix(mat);
	}
}
