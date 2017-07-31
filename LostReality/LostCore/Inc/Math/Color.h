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

		explicit FColor128(float val)
			: R(val), G(val), B(val), A(val)
		{
		}

		FColor128(float r, float g, float b, float a = 0.f)
			: R(r), G(g), B(b), A(a)
		{
		}
	};

	struct FColor96
	{
		float R;
		float G;
		float B;

		FColor96() : R(0.f), G(0.f), B(0.f)
		{
		}

		explicit FColor96(uint32 rgb)
			: R(((rgb >> 16) & 0xff) / 255.f)
			, G(((rgb >> 8) & 0xff) / 255.f)
			, B(((rgb >> 0) & 0xff) / 255.f)
		{
		}

		explicit FColor96(float val)
			: R(val), G(val), B(val)
		{
		}

		FColor96(float r, float g, float b)
			: R(r), G(g), B(b)
		{
		}

		bool operator==(const FColor96& rhs) const
		{
			return (R == rhs.R
				&& G == rhs.G
				&& B == rhs.B);
		}
	};
}