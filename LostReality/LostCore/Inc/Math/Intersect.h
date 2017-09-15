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
	// ���ߺ�ƽ���ཻ����
	// �������true,�����distanceΪ����������ʽ�����߲���.
	// strictģʽ,�������������Ϸ�Χ����ƽ�������ཻ����true�������������ƽ��/����/���浫�Ƿ�Χ���ཻ�ȷ���false.
	// ��strictģʽ�������ཻ���߷�Χ��,��ƽ��/��������false.
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

	// ���ߺ��������ཻ����
	static bool RayTriangleIntersect(const FRay& ray, const FTriangle& tri, FRay::FT& distance)
	{
		return false;
	}

	// ���ߺ�AABB�ཻ����,���Գɹ�����true��distance����ཻ����.
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

	// ��ͬ�ռ�����ߺ�AABB�ཻ����, ���ص�distance��ת�������߿ռ�.
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