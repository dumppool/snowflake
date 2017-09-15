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
	// 射线和平面相交测试
	// 如果返回true,输出的distance为参数定义形式的射线参数.
	// strict模式,射线在正方向上范围内与平面正面相交返回true，其他情况包括平行/包含/正面但是范围外相交等返回false.
	// 非strict模式允许背面相交或者范围外,仅平行/包含返回false.
	static bool RayPlaneIntersect(const FRay& ray, const FPlane& plane, FRay::FT& distance, bool strict = false)
	{
		distance = INFINITY;
		FRay::FT div = ray.Normal.Dot(plane.Normal);
		if (IsZero(div))
		{
			return false;
		}

		if (div > (FRay::FT)0.0 && strict)
		{
			return false;
		}

		FRay::FT divided = plane.Distance - ray.P0.Dot(plane.Normal);
		if (divided > (FRay::FT)0.0 && strict)
		{
			return false;
		}

		distance = divided / div;
		return !strict || distance < ray.Distance;
	}

	// 射线和三角面相交测试
	static bool RayTriangleIntersect(const FRay& ray, const FTriangle& tri, FRay::FT& distance)
	{
		return false;
	}

	// 射线和AABB相交测试,测试成功返回true且distance输出相交距离.
	static bool RayBoxIntersect(const FRay& ray, const FAABoundingBox& box, FRay::FT& distance)
	{
		FRay::FT tmin, tmax, ymin, ymax, zmin, zmax;
		tmin = ((ray.Normal.X >= 0 ? box.Min.X : box.Max.X) - ray.P0.X) * ray.RcpNormal.X;
		tmax = ((ray.Normal.X >= 0 ? box.Max.X : box.Min.X) - ray.P0.X) * ray.RcpNormal.X;
		ymin = ((ray.Normal.Y >= 0 ? box.Min.Y : box.Max.Y) - ray.P0.Y) * ray.RcpNormal.Y;
		ymax = ((ray.Normal.Y >= 0 ? box.Max.Y : box.Min.Y) - ray.P0.Y) * ray.RcpNormal.Y;
		if ((tmin > ymax) || (ymin > tmax))
		{
			return false;
		}

		if (ymin > tmin)
		{
			tmin = ymin;
		}

		if (ymax < tmax)
		{
			tmax = ymax;
		}

		zmin = ((ray.Normal.Z >= 0 ? box.Min.Z : box.Max.Z) - ray.P0.Z) * ray.RcpNormal.Z;
		zmax = ((ray.Normal.Z >= 0 ? box.Max.Z : box.Min.Z) - ray.P0.Z) * ray.RcpNormal.Z;
		if ((tmin > zmax) || (zmin > tmax))
		{
			return false;
		}

		if (zmin > tmin)
		{
			tmin = zmin;
		}

		if (zmax < tmax)
		{
			tmax = zmax;
		}

		distance = tmin;
		return ((tmin < ray.Distance) && (tmax > 0));
	}

	// 不同空间的射线和AABB相交测试, 返回的distance已转换到射线空间.
	static bool RayBoxIntersect(const FRay& ray, const FAABoundingBox& box, const FFloat4x4& matRay2Box, FRay::FT& distance)
	{
		auto localRay = ray.GetTransformed(matRay2Box);
		auto scale = ray.Distance / localRay.Distance;
		float dist;
		if (RayBoxIntersect(localRay, box, dist))
		{
			distance = dist * scale;
			return true;
		}
		else
		{
			return false;
		}
	}
}