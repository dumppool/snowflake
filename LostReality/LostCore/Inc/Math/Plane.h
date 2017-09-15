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
		FPlane& Transform(const FFloat4x4& mat);
		FPlane GetTransformed(const FFloat4x4& mat) const;

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
		, Distance(d)
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

	inline FPlane & FPlane::Transform(const FFloat4x4 & mat)
	{
		FFloat3 pt;
		if (!IsZero(Normal.X))
		{
			pt.X = Distance / Normal.X;
			pt.Y = pt.Z = (FFloat4x4::FT)0.0;
		}
		else if (!IsZero(Normal.Y))
		{
			pt.Y = Distance / Normal.Y;
			pt.X = pt.Z = (FFloat4x4::FT)0.0;
		}
		else if (!IsZero(Normal.Z))
		{
			pt.Z = Distance / Normal.Z;
			pt.X = pt.Y = (FFloat4x4::FT)0.0;
		}

		Normal = mat.ApplyVector(Normal).GetNormal();
		pt = mat.ApplyPoint(pt);
		Distance = Normal.Dot(pt);
		return *this;
	}

	inline FPlane FPlane::GetTransformed(const FFloat4x4 & mat) const
	{
		FPlane result(*this);
		return result.Transform(mat);
	}
}