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

	// ��������ƹ���
	class FAxisTool
	{
	private:
		FSegmentTool SegmentTool;

		FFloat4x4 World;
		vector<FAxisData> Data;

	public:
		FAxisTool();

		void Draw(IRenderContext* rc, float sec = 0.f);
		void AddAxis(const FAxisData& axis);
		void SetWorldMatrix(const FFloat4x4& mat);

		static FAxisTool* Get()
		{
			static FAxisTool SInstance;
			return &SInstance;
		}
	};
}

