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
		virtual void Clone(FBasicModel& model);
		virtual void EnableDepthTest(bool depthTest);
		
		FAABoundingBox* GetBoundingBox();
		IPrimitiveGroup* GetPrimitive();
		IMaterial* GetMaterial();
		IConstantBuffer* GetMatricesBuffer();
		IConstantBuffer* GetCustomBuffer();
		FMeshData* GetPrimitiveData();
		void SetColor(const FColor128& color);

	protected:
		//virtual void RayTest() = 0;

		virtual bool ConfigPrimitive(const string& url, IPrimitiveGroup*& pg, FMeshData& pgdata);
		virtual bool ConfigMaterial(const string& url);

		virtual void UpdateConstant();
		virtual void UpdateGizmosBoundingBox();

		virtual void DrawGizmos();
		virtual void DrawModel();

		FSegmentTool* GetSegmentRenderer();

	private:
		void ValidateBoundingBox();
		void Fini();

		IPrimitiveGroup* Primitive;
		IMaterial* Material;
		IConstantBuffer* MatricesBuffer;
		FMeshData PrimitiveData;
		FSegmentTool SegmentRenderer;
		FAABoundingBox BoundingBox;
		FCustomParameter Custom;
		IConstantBuffer* CustomBuffer;
	};

	class FStaticModel : public FBasicModel
	{
	public:
		FStaticModel();
		virtual ~FStaticModel() override;

		virtual void Tick() override;
		virtual void SetWorldMatrix(const FFloat4x4& world) override;
		virtual FFloat4x4 GetWorldMatrix() override;
		virtual void Clone(FBasicModel& model) override;

	protected:
		virtual bool ConfigMaterial(const string& url) override;
		virtual void UpdateConstant() override;
		virtual void DrawGizmos() override;
		//virtual void RayTest() = 0;

		void UpdateGizmosNormalTangent();

	private:
		void Fini();

	private:
		FSingleMatrixParameter World;
		FAxisRenderer AxisRenderer;
	};

	class FSkeletalModel : public FBasicModel
	{
	public:
		FSkeletalModel();
		virtual ~FSkeletalModel() override;

		virtual void Tick() override;
		virtual void SetWorldMatrix(const FFloat4x4& world) override;
		virtual FFloat4x4 GetWorldMatrix() override;
		virtual void Clone(FBasicModel& model) override;

		void PlayAnimation(const string& animName);

	protected:
		virtual void UpdateConstant() override;
		//virtual void RayTest() = 0;

		virtual bool ConfigPrimitive(const string& url, IPrimitiveGroup*& pg, FMeshData& pgdata) override;
		virtual bool ConfigMaterial(const string& url) override;

		virtual void DrawGizmos();

		void UpdateGizmosNormalTangent();
		void UpdateGizmosSkeleton();

	private:
		void Fini();

	private:

		FSkinnedParameter Matrices;
		FSkeletonTree Root;
		FAxisRenderer AxisRenderer;
		FSegmentTool SkeletonRenderer;
	};
}