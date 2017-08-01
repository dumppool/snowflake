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
		Static = 0,
		Skeleton = 1,
		TriangularPyramid = 2,
	};

	class FBasicModel : public IBasicInterface
	{
	public:
		FBasicModel();
		virtual ~FBasicModel() override;

		virtual bool Load(IRenderContext * rc, const char* url) override;
		virtual void Fini() override;
		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;

	public:
		virtual void SetWorldMatrix(const FFloat4x4& world);

	protected:
		IPrimitiveGroup* Primitive;
		IMaterial* Material;

		struct 
		{
			FFloat4x4 World;
			array<FFloat4x4, MAX_BONES_PER_BATCH> Bones;
		} Matrices;

		FSkelPoseTree Root;
		//FAnimation Animation;

		FMeshData MeshData;

		FAxisTool AxisRenderer;
		FSegmentTool SegmentRenderer;

		FAABoundingBox BoundingBox;
	};
}