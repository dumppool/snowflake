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
	class IPrimitive;
	class IMaterial;
	class IResourceLoader;

	class FBasicModel
	{
	public:
		FBasicModel();
		virtual ~FBasicModel();

		virtual bool Config(const FJson& config);
		virtual FJson Save();
		virtual void Tick();

		virtual void SetWorldMatrix(const FFloat4x4& world) = 0;
		virtual FFloat4x4 GetWorldMatrix() = 0;
		virtual void Clone(FBasicModel& model);
		virtual void EnableDepthTest(bool depthTest);
		virtual void EnableFlags(uint32 flags);
		virtual void DisableFlags(uint32 flags);
		virtual bool HasFlags(uint32 flags) const;
		
		void SetUrl(const string& url);
		const string& GetUrl() const;

		FAABoundingBox* GetBoundingBox();
		IPrimitive* GetPrimitive();
		IMaterial* GetMaterial();
		IConstantBuffer* GetMatricesBuffer();
		IConstantBuffer* GetCustomBuffer();
		FMeshData* GetPrimitiveData();
		void SetColor(const FColor128& color);

		bool RayTest(const FRay& ray, FRay::FT& dist);

	protected:
		virtual bool ConfigPrimitive(const string& url, IPrimitive*& pg, FMeshData& pgdata);
		virtual bool ConfigMaterial(const string& url);

		virtual void UpdateConstant();
		virtual void UpdateGizmosBoundingBox();

		virtual void CommitGizmos();
		virtual void CommitModel();

		FSegmentTool* GetSegmentRenderer();

	private:
		void ValidateBoundingBox();
		void Destroy();

		string Url;
		IPrimitive* Primitive;
		IMaterial* Material;
		IConstantBuffer* MatricesBuffer;
		FMeshData PrimitiveData;
		FSegmentTool SegmentRenderer;
		FAABoundingBox BoundingBox;
		FCustomParameter Custom;
		IConstantBuffer* CustomBuffer;

		uint32 ActorFlags;
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
		virtual void CommitGizmos() override;
		//virtual void RayTest() = 0;

		void UpdateGizmosNormalTangent();

	private:
		void Destroy();

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

		virtual bool ConfigPrimitive(const string& url, IPrimitive*& pg, FMeshData& pgdata) override;
		virtual bool ConfigMaterial(const string& url) override;

		virtual void CommitGizmos();

		void UpdateGizmosNormalTangent();
		void UpdateGizmosSkeleton();

	private:
		void Destroy();

	private:

		FSkinnedParameter Matrices;
		FSkeletonTree Root;
		FAxisRenderer AxisRenderer;
		FSegmentTool SkeletonRenderer;
	};
}