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
	// 射线和平面相交测试，射线在正方向上与平面正面相交返回true，其他情况包括平行/包含等返回false.
	// 如果返回true，输出的distance为参数定义形式的射线参数.
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

	// 射线和三角面相交测试
	static bool RayTriangleIntersect(const FRay& ray, const FTriangle& tri, FRay::FT& distance)
	{
		return false;
	}

	// 射线和AABB相交测试
	static bool RayBoxIntersect(const FRay& ray, const FAABoundingBox& aabb, FRay::FT& distance)
	{
		return false;
	}
}