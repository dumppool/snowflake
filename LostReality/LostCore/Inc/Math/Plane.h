/*
* file Plane.h
*
* author luoxw
* date 2017/09/08
*
*
*/

#pragma once

#include "MathBase.h"

namespace LostCore
{
	class FPlane
	{
	public:
		typedef FFloat3::FT FT;

		// 对于平面上任意点p, Dot(p, Normal) == Distance总是成立, 其中Normal为单位向量.
		FFloat3 Normal;
		FT Distance;

	public:
		// 默认构造非法平面.
		FPlane();
		explicit FPlane(const vector<FFloat3>& pts);
		FPlane(const FFloat3& n, FT d);

		// 通过若干(不共线点数量>2)点构造平面.
		void ConstructPlane(const vector<FFloat3>& pts);

		FT DistanceFrom(const FFloat3& pt);

	};

	class FTriangle
	{
	public:
		typedef FFloat3::FT FT;

		FFloat3 P[3];
	};

	inline FPlane::FPlane()
	{
	}

	inline FPlane::FPlane(const vector<FFloat3>& pts)
	{
		ConstructPlane(pts);
	}

	inline FPlane::FPlane(const FFloat3& n, FPlane::FT d)
		: Normal(n.GetNormal())
		, Distance(d/n.Size())
	{
	}

	inline void FPlane::ConstructPlane(const vector<FFloat3>& pts)
	{
		if (pts.size() < 3)
		{
			return;
		}

		int32 curr = 0, prev = pts.size() - 1;
		FFloat3 sumNormal(FFloat3::GetZero()), sumPt(FFloat3::GetZero());
		for (; curr < pts.size(); ++curr)
		{
			const auto& currPt = pts[curr];
			sumPt += currPt;

			const auto& prevPt = pts[prev];
			sumNormal.X += (prevPt.Z + currPt.Z) * (prevPt.Y - currPt.Y);
			sumNormal.Y += (prevPt.X + currPt.X) * (prevPt.Z - currPt.Z);
			sumNormal.Z += (prevPt.Y + currPt.Y) * (prevPt.X - currPt.X);

			prev = curr;
		}

		Normal = sumNormal.GetNormal();
		Distance = sumPt.Dot(Normal) / pts.size();
	}

	inline FPlane::FT FPlane::DistanceFrom(const FFloat3 & pt)
	{
		return (Normal.Dot(pt) - Distance);
	}
}