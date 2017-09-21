/*
* file Line.h
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
	class FRay
	{
	public:
		typedef FFloat3::FT FT;
		FFloat3 P0;
		FFloat3 Normal;
		FFloat3 RcpNormal;
		FT Distance;

	public:
		FRay();
		FRay(const FFloat3& p0, const FFloat3& n, FT dist = 10000.0f);
		//FRay::FRay(const FFloat3& p0, const FFloat3& p1);

		// 获取线上对应length的点, factor为0时即P0点.
		FFloat3 GetPoint(FT length) const;

		FRay& Transform(const FFloat4x4& mat);
		FRay GetTransformed(const FFloat4x4& mat) const;
	};
	
	inline FRay::FRay()
	{}

	inline FRay::FRay(const FFloat3 & p0, const FFloat3 & n, FT dist)
		: P0(p0)
		, Normal(n.GetNormal())
		, RcpNormal(FFloat3(1.0,1.0,1.0)/Normal)
		, Distance(dist)
	{
	}

	//inline FRay::FRay(const FFloat3& p0, const FFloat3& p1)
	//	: P0(p0)
	//	, Normal((p1 - p0).GetNormal())
	//	, RcpNormal(FFloat3(1.0, 1.0, 1.0) / Normal)
	//	, Distance((p1 - p0).Size())
	//{
	//}

	inline FFloat3 FRay::GetPoint(FT length) const
	{
		return P0 + Normal * length;
	}

	inline FRay& FRay::Transform(const FFloat4x4 & mat)
	{
		auto p1 = GetPoint(Distance);
		p1 = mat.ApplyPoint(p1);
		P0 = mat.ApplyPoint(P0);
		Distance = (p1 - P0).Size();
		Normal = (p1 - P0).GetNormal();
		RcpNormal = FFloat3(1.0, 1.0, 1.0) / Normal;
		return *this;
	}

	inline FRay FRay::GetTransformed(const FFloat4x4 & mat) const
	{
		FRay result(*this);
		return result.Transform(mat);
	}

}