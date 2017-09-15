/*
* file GizmoAxis.h
*
* author luoxw
* date 2017/07/25
*
*
*/

#pragma once

#include "GizmoLine.h"

namespace LostCore
{
	struct FAxisData
	{
		FFloat3 DirX;
		FFloat3 DirY;
		FFloat3 DirZ;
		FFloat3 Origin;
		float Length;

		FAxisData() {}
		FAxisData(const FFloat3& dirx, 
			const FFloat3& diry, 
			const FFloat3& dirz,
			const FFloat3& origin, 
			float len)
			: DirX(dirx.GetNormal())
			, DirY(diry.GetNormal())
			, DirZ(dirz.GetNormal())
			, Origin(origin)
			, Length(len)
		{
		}
	};

	// 坐标轴绘制工具
	class FAxisRenderer
	{
	private:
		FSegmentTool SegmentTool;

		FFloat4x4 World;
		vector<FAxisData> Data;

		FColor96 ColorAxisX;
		FColor96 ColorAxisY;
		FColor96 ColorAxisZ;

	public:
		FAxisRenderer();

		void ResetData();
		void Draw();
		void AddAxis(const FAxisData& axis);
		void SetWorldMatrix(const FFloat4x4& mat);
		void SetColor(const FColor96& colX, const FColor96& colY, const FColor96& colZ);

		static FAxisRenderer* Get()
		{
			static FAxisRenderer SInstance;
			return &SInstance;
		}
	};
}

