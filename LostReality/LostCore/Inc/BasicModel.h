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

	enum class EPrimitiveType : uint8
	{
		Undefined = 0,
		PrimitiveFile,
		TriangularPyramid,
	};

	class FBasicModel : public IBasicInterface
	{
	public:
		virtual bool Load(IRenderContext * rc, const char* url) override;

		virtual void SetWorldMatrix(const FFloat4x4& world) = 0;
		virtual void SetPrimitiveVertexFlags(uint32 flags) = 0;
	};

	class FStaticModel : public FBasicModel
	{
	public:
		FStaticModel();
		virtual ~FStaticModel() override;

		virtual bool Config(IRenderContext * rc, const FJson& config) override;
		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;

		virtual void SetWorldMatrix(const FFloat4x4& world) override;
		virtual void SetPrimitiveVertexFlags(uint32 flags) override;

	public:
		void Fini();

	protected:
		FFloat4x4 World;
		IPrimitiveGroup* Primitive;
		IMaterial* Material;
		FMeshData MeshData;
		uint32 VertexFlags;
		FAxisTool AxisRenderer;
		FSegmentTool SegmentRenderer;
	};

	class FSkeletalModel : public FBasicModel
	{
	public:
		FSkeletalModel();
		virtual ~FSkeletalModel() override;

		virtual bool Config(IRenderContext * rc, const FJson& config) override;
		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;

		virtual void SetWorldMatrix(const FFloat4x4& world) override;
		virtual void SetPrimitiveVertexFlags(uint32 flags) override;

	public:
		void Fini();
		void PlayAnimation(const string& animName);

	protected:

		struct
		{
			FFloat4x4 World;
			array<FFloat4x4, MAX_BONES_PER_BATCH> Bones;
		} Matrices;

		IPrimitiveGroup* Primitive;
		IMaterial* Material;
		FSkeletonTree Root;
		FMeshData MeshData;
		FAABoundingBox BoundingBox;
		uint32 VertexFlags;
		FAxisTool AxisRenderer;
		FSegmentTool SegmentRenderer;
		FSegmentTool SkeletonRenderer;

		FSkeletonTree Root2;
	};
}