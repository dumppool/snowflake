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
		virtual void SetWorldMatrix(const FMatrix& world);

	protected:
		IPrimitiveGroup* Primitive;
		IMaterial* Material;

		struct 
		{
			FMatrix World;
			array<FMatrix, MAX_BONES_PER_BATCH> Bones;
		} Matrices;

		map<string, int32> SkeletonIndexMap;

		FSkelPoseTree Root;
		//FAnimation Animation;

		uint32 PrimitiveFlags;

		FAABoundingBox BoundingBox;
	};
}