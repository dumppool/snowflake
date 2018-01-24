/*
* file GizmoLine.h
*
* author luoxw
* date 2017/07/25
*
*
*/

#pragma once


namespace LostCore
{
	class IPrimitive;
	class IMaterial;

	struct FSegmentVertex
	{
		FFloat3 Coordinate;
		FColor128 Color;
		FFloat2 Unused;

		FSegmentVertex() {}
		FSegmentVertex(const FFloat3& pos, const FColor128& col)
			: Coordinate(pos), Color(col) {}
	};

	struct FSegmentData
	{
		FFloat3 StartPt;
		FColor128 StartPtColor;

		FFloat3 StopPt;
		FColor128 StopPtColor;
	};

	// 坐标轴绘制工具
	class FSegmentTool
	{
	private:
		FSingleMatrixParameter World;
		vector<FSegmentVertex> Data;

		bool bConstructed;
		uint32 CurrentPrimitiveBytes;
		IPrimitive* Primitive;
		IConstantBuffer* ConstantBuffer;
		bool bDepthTest;

		bool ConstructPrimitive(const void* buf, uint32 sz);
		void DestroyPrimitive();

	public:
		FSegmentTool();
		~FSegmentTool();

		void ResetData();
		void Commit();
		void AddSegment(const FSegmentData& seg);

		void SetWorldMatrix(const FFloat4x4& mat);
		void EnableDepthTest(bool enable);

		static FSegmentTool* Get()
		{
			FSegmentTool SInstance;
			return &SInstance;
		}
	};
}

