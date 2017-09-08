/*
* file Intersect.h
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
	// ���ߺ�ƽ���ཻ���ԣ�����������������ƽ�������ཻ����true�������������ƽ��/�����ȷ���false.
	// �������true�������distanceΪ����������ʽ�����߲���.
	static bool RayPlaneIntersect(const FRay& ray, const FPlane& plane, FRay::FT& distance)
	{
		FRay::FT div = ray.Normal.Dot(plane.Normal);
		if (IsZero(div))
		{
			return false;
		}

		if (div > (FRay::FT)0.0)
		{
			return false;
		}

		FRay::FT divided = plane.Distance - ray.P0.Dot(plane.Normal);
		if (divided > (FRay::FT)0.0)
		{
			return false;
		}

		distance = divided / div;
		return true;
	}

	// ���ߺ��������ཻ����
	static bool RayTriangleIntersect(const FRay& ray, const FTriangle& tri, FRay::FT& distance)
	{
		return false;
	}

	// ���ߺ�AABB�ཻ����
	static bool RayBoxIntersect(const FRay& ray, const FAABoundingBox& aabb, FRay::FT& distance)
	{
		return false;
	}
}