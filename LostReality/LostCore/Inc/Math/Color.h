/*
* file Color.h
*
* author luoxw
* date 2017/07/25
*
*
*/

#pragma once

#include "MathBase.h"

namespace LostCore
{
	struct FColor128
	{
		float R;
		float G;
		float B;
		float A;

		FColor128() : R(0.f), G(0.f), B(0.f), A(0.f)
		{
		}

		explicit FColor128(uint32 argb)
			: R(((argb >> 16) & 0xff) / 255.f)
			, G(((argb >> 8) & 0xff) / 255.f)
			, B(((argb >> 0)  & 0xff) / 255.f)
			, A(((argb >> 24) & 0xff) / 255.f)
		{
		}

		FColor128(float r, float g, float b, float a = 0.f)
			: R(r), G(g), B(b), A(a)
		{
		}

		bool operator==(const FColor128& rhs) const
		{
			return (R == rhs.R
				&& G == rhs.G
				&& B == rhs.B
				&& A == rhs.A);
		}
	};

	struct FColor24
	{
		float R;
		float G;
		float B;

		FColor24() : R(0.f), G(0.f), B(0.f)
		{
		}

		explicit FColor24(uint32 rgb)
			: R(((rgb >> 16) & 0xff) / 255.f)
			, G(((rgb >> 8) & 0xff) / 255.f)
			, B(((rgb >> 0) & 0xff) / 255.f)
		{
		}

		explicit FColor24(float val)
			: R(val), G(val), B(val)
		{
		}

		FColor24(float r, float g, float b)
			: R(r), G(g), B(b)
		{
		}

		bool operator==(const FColor24& rhs) const
		{
			return (R == rhs.R
				&& G == rhs.G
				&& B == rhs.B);
		}
	};
}