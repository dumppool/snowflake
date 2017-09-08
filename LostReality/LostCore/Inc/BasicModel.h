/*
* file BasicModel.h
*
* author luoxw
* date 2017/02/10
*
*
*/

#pragma once

#include "BasicInterface.h"
#include "Animation.h"
#include "Gizmo/GizmoAxis.h"
#include "Gizmo/GizmoLine.h"

namespace LostCore
{
	class IPrimitiveGroup;
	class IMaterial;
	class IResourceLoader;

	class FBasicModel : public IBasicInterface
	{
	public:
		FBasicModel();
		virtual ~FBasicModel() override;

		virtual bool Load(const char* url) override;
		virtual bool Config(const FJson& config) override;
		virtual void Tick() override;
		virtual void Draw() override;

		virtual void SetWorldMatrix(const FFloat4x4& world) = 0;
		virtual FFloat4x4 GetWorldMatrix() = 0;

	protected:
		virtual void UpdateConstant() = 0;
		virtual void DrawModel();
		//virtual void RayTest() = 0;

		virtual bool ConfigPrimitive(const string& url, IPrimitiveGroup*& pg, FMeshData& pgdata);
		virtual bool ConfigMaterial(const string& url, IMaterial*& mat);

		virtual void UpdateGizmosBoundingBox();
		virtual void DrawGizmos();

		IPrimitiveGroup* GetPrimitive();
		IMaterial* GetMaterial();
		FMeshData* GetPrimitiveData();
		FSegmentTool* GetSegmentRenderer();
		FAABoundingBox* GetBoundingBox();

	private:
		void ValidateBoundingBox();
		void Fini();

		IPrimitiveGroup* Primitive;
		IMaterial* Material;
		FMeshData PrimitiveData;
		FSegmentTool SegmentRenderer;
		FAABoundingBox BoundingBox;
	};

	class FStaticModel : public FBasicModel
	{
	public:
		FStaticModel();
		virtual ~FStaticModel() override;

		virtual void Tick() override;
		virtual void SetWorldMatrix(const FFloat4x4& world) override;
		virtual FFloat4x4 GetWorldMatrix() override;

	protected:
		virtual void UpdateConstant() override;
		virtual void DrawGizmos() override;
		//virtual void RayTest() = 0;

		void UpdateGizmosNormalTangent();

	private:
		void Fini();

	private:
		FFloat4x4 World;
		FAxisTool AxisRenderer;
	};

	class FSkeletalModel : public FBasicModel
	{
	public:
		FSkeletalModel();
		virtual ~FSkeletalModel() override;

		virtual void Tick() override;
		virtual void SetWorldMatrix(const FFloat4x4& world) override;
		virtual FFloat4x4 GetWorldMatrix() override;

		void PlayAnimation(const string& animName);

	protected:
		virtual void UpdateConstant() override;
		//virtual void RayTest() = 0;

		virtual bool ConfigPrimitive(const string& url, IPrimitiveGroup*& pg, FMeshData& pgdata) override;
		virtual bool ConfigMaterial(const string& url, IMaterial*& mat) override;

		virtual void DrawGizmos();

		void UpdateGizmosNormalTangent();
		void UpdateGizmosSkeleton();

	private:
		void Fini();

	private:

		struct
		{
			FFloat4x4 World;
			array<FFloat4x4, MAX_BONES_PER_BATCH> Bones;
		} Matrices;


		FSkeletonTree Root;
		FAxisTool AxisRenderer;
		FSegmentTool SkeletonRenderer;
	};
}