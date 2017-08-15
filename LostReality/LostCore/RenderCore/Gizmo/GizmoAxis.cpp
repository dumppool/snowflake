/*
* file GizmoAxis.cpp
*
* author luoxw
* date 2017/07/25
*
*
*/

#include "stdafx.h"
#include "Gizmo/GizmoAxis.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace std;
using namespace LostCore;

LostCore::FAxisTool::FAxisTool()
{
	Data.clear();
}

void LostCore::FAxisTool::Draw(IRenderContext * rc, float sec)
{
	if (Data.size() == 0)
	{
		return;
	}

	vector<FAxisData> axises(Data.size());
	memcpy(&axises[0], &Data[0], axises.size() * sizeof(FAxisData));
	Data.clear();

	for (uint32 i = 0; i < axises.size(); ++i)
	{
		FSegmentData seg;

		// axis x
		seg.StartPt = axises[i].Origin;
		seg.StopPt = seg.StartPt + axises[i].DirX * axises[i].Length;
		seg.Color = FColor96((uint32)0xff0000);
		SegmentTool.AddSegment(seg);

		// axis y
		seg.StartPt = axises[i].Origin;
		seg.StopPt = seg.StartPt + axises[i].DirY * axises[i].Length;
		seg.Color = FColor96((uint32)0x00ff00);
		SegmentTool.AddSegment(seg);

		// axis z
		seg.StartPt = axises[i].Origin;
		seg.StopPt = seg.StartPt + axises[i].DirZ * axises[i].Length;
		seg.Color = FColor96((uint32)0x0000ff);
		SegmentTool.AddSegment(seg);
	}

	SegmentTool.SetWorldMatrix(World);
	SegmentTool.Draw(rc, sec);
}

void LostCore::FAxisTool::AddAxis(const FAxisData & axis)
{
	Data.push_back(axis);
}

void LostCore::FAxisTool::SetWorldMatrix(const FFloat4x4 & mat)
{
	World = mat;
}
