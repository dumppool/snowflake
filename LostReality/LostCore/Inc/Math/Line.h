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

		// ���ߵ��������ߵ�����������.
		FFloat3 RcpNormal;
		bool RcpNormalSignX;
		bool RcpNormalSignY;
		bool RcpNormalSignZ;

	public:
		FRay();
		FRay(const FFloat3& p0, const FFloat3& n);

		// ��ȡ���϶�Ӧlength�ĵ�, factorΪ0ʱ��P0��.
		FFloat3 GetPointInRay(FT length);
	};
	
	inline FRay::FRay()
	{}

	inline FRay::FRay(const FFloat3 & p0, const FFloat3 & n)
		: P0(p0)
		, Normal(n.GetNormal())
		, RcpNormal(FFloat3(1.0,1.0,1.0)/Normal)
		, RcpNormalSignX(RcpNormal.X >= 0)
		, RcpNormalSignY(RcpNormal.Y >= 0)
		, RcpNormalSignZ(RcpNormal.Z >= 0)
	{
	}

	inline FFloat3 FRay::GetPointInRay(FT length)
	{
		return FFloat3();
	}

}