/*
* file MathBase.h
*
* author luoxw
* date 2017/02/08
*
* All these great algorithms in the math library come from UnrealEngine4,
* thank them for saving me.
*/

#pragma once

namespace LostCore
{
	static const float SSmallFloat = 1.e-8f;
	static const float SPI = 3.14159265359f;
	static const float SHalfPI = SPI * 0.5f;
	static const float SInvPI = (1.f / SPI);
	static const float SD2RConstant = SPI / 180.f;
	static const float SR2DConstant = 180.f / SPI;

	INLINE bool IsZero(float value, float tolerance = SSmallFloat)
	{
		return std::abs(value) < tolerance;
	}

	INLINE bool IsEqual(float f1, float f2, float tolerance = SSmallFloat)
	{
		return IsZero(f1 - f2, tolerance);
	}

	INLINE void SinCos(float& oSin, float& oCos, float rad)
	{
		float quotient = (SInvPI * 0.5f) * rad;
		if (rad >= 0.f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}

		float y = rad - (2.f * SPI) * quotient;

		float sign;
		if (y > SHalfPI)
		{
			y = SPI - y;
			sign = -1.f;
		}
		else if (y < -SHalfPI)
		{
			y = -SPI - y;
			sign = -1.f;
		}
		else
		{
			sign = 1.f;
		}

		float y2 = y * y;


		// 11-degree minimax approximation
		oSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		oCos = sign*p;
	}

	INLINE float Sqrt(float val)
	{
		return std::sqrt(val);
	}

	INLINE float InvSqrt(float val)
	{
		float s = Sqrt(val);
		return 1.f / s;
	}

	// return angle in the range (-180, 180]
	INLINE float ClampAngle(float deg)
	{
		deg = std::fmod(deg, 360.f);
		if (deg < 0.f)
		{
			deg += 360.f;
		}

		if (deg > 180.f)
		{
			deg -= 360.f;
		}

		return deg;
	}

	INLINE float Atan2(float y, float x)
	{
		return std::atan2(y, x);
	}

	INLINE float Asin(float val)
	{
		return std::asin(val);
	}
}