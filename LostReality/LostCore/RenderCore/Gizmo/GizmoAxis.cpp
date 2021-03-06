/*
* file GizmoAxis.cpp
*
* author luoxw
* date 2017/07/25
*
*
*/

#include "stdafx.h"
#include "GizmoAxis.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace std;
using namespace LostCore;

LostCore::FAxisRenderer::FAxisRenderer()
	: ColorAxisX(0xff0000)
	, ColorAxisY(0x0000ff)
	, ColorAxisZ(0x00ff00)
{
	ResetData();
}

void LostCore::FAxisRenderer::ResetData()
{
	Data.clear();
}

void LostCore::FAxisRenderer::Commit()
{
	if (Data.size() == 0)
	{
		return;
	}

	vector<FAxisData> axises(Data.size());
	memcpy(&axises[0], &Data[0], axises.size() * sizeof(FAxisData));

	for (uint32 i = 0; i < axises.size(); ++i)
	{
		FSegmentData seg;

		// axis x
		seg.StartPt = axises[i].Origin;
		seg.StopPt = seg.StartPt + axises[i].DirX * axises[i].Length;
		seg.StartPtColor = ColorAxisX;
		SegmentTool.AddSegment(seg);

		// axis y
		seg.StartPt = axises[i].Origin;
		seg.StopPt = seg.StartPt + axises[i].DirY * axises[i].Length;
		seg.StartPtColor = ColorAxisY;
		SegmentTool.AddSegment(seg);

		// axis z
		seg.StartPt = axises[i].Origin;
		seg.StopPt = seg.StartPt + axises[i].DirZ * axises[i].Length;
		seg.StartPtColor = ColorAxisZ;
		SegmentTool.AddSegment(seg);
	}

	SegmentTool.SetWorldMatrix(World);
	SegmentTool.Commit();
}

void LostCore::FAxisRenderer::AddAxis(const FAxisData & axis)
{
	Data.push_back(axis);
}

void LostCore::FAxisRenderer::SetWorldMatrix(const FFloat4x4 & mat)
{
	World = mat;
}

void LostCore::FAxisRenderer::SetColor(const FColor128 & colX, const FColor128 & colY, const FColor128 & colZ)
{
	ColorAxisX = colX;
	ColorAxisY = colY;
	ColorAxisZ = colZ;
}
