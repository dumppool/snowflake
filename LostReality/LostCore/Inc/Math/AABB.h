/*
* file AABB.h
*
* author luoxw
* date 2017/06/28
*
*
*/

#pragma once

#include "MathBase.h"

namespace LostCore
{
	class FAABoundingBox
	{
		FVec3 Min;
		FVec3 Max;

	public:
		FAABoundingBox()
			: Min(FLT_MAX, FLT_MAX, FLT_MAX)
			, Max(-FLT_MAX, -FLT_MAX, -FLT_MAX)
		{}

		FAABoundingBox(const FVec3& min, const FVec3& max)
			: Min(min)
			, Max(max)
		{}

		void Set(const FVec3& min, const FVec3& max)
		{
			Min = min;
			Max = max;
		}

		FAABoundingBox& operator=(const FAABoundingBox& rhs)
		{
			Min = rhs.Min;
			Max = rhs.Max;
			return *this;
		}

		bool operator==(const FAABoundingBox& rhs) const
		{
			return Min == rhs.Min && Max == rhs.Max;
		}

		bool operator!=(const FAABoundingBox& rhs) const
		{
			return !(*this == rhs);
		}

		bool IsValid() const
		{
			static FAABoundingBox SInvalid;
			return *this != SInvalid;
		}

		const FVec3& GetMin() const
		{
			return Min;
		}

		const FVec3& GetMax() const
		{
			return Max;
		}

		FVec3 GetSize() const
		{
			return IsValid() ? Max - Min : FVec3();
		}

		void AddPoint(const FVec3& p)
		{
			if (!IsValid())
			{
				Min = p;
				Max = p;
				return;
			}

			if (Min.X > p.X)
			{
				Min.X = p.X;
			}

			if (p.X > Max.X)
			{
				Max.X = p.X;
			}

			if (Min.Y > p.Y)
			{
				Min.Y = p.Y;
			}

			if (p.Y > Max.Y)
			{
				Max.Y = p.Y;
			}

			if (Min.Z > p.Z)
			{
				Min.Z = p.Z;
			}

			if (p.Z > Max.Z)
			{
				Max.Z = p.Z;
			}
		}

		void AddBound(const FAABoundingBox& b)
		{
			AddPoint(b.Min);
			AddPoint(b.Max);
		}
	};
}