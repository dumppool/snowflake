/*
* file MathBase.h
*
* author luoxw
* date 2017/02/08
*
* 数学基础：
* 微软左手系 右上前
* 行主序矩阵 组合顺序从左到右
* Euler Pitch(x-axis), Yaw(y-axis), Roll(z-axis)
* 
* 右手系到左手系的转换参考
* UnrealEngine\Engine\Source\Editor\UnrealEd\Private\Fbx\FbxUtilsImport.cpp
* Vector				右手(X, Y, Z) -> 左手(X, Y, -Z)
* Euler					右手(Pitch, Yaw, Roll) -> 左手(Pitch, -Yaw, Roll）
* Quaternion			右手(Qx, Qy, Qz, Qw) -> 左手(Qx, Qy, -Qz, -Qw)
* Matrix(affine)		右手	| M00, M01, M02, 0 |	左手	| M00, -M01, M02, 0	|
*							| M10, M11, M12, 0 |	->	| M10, -M11, M12, 0	|
*							| M20, M21, M22, 0 |		| -M20, M21, -M22, 0|
*							| M30, M31, M32, 1 |		| M30, -M31, M32, 1	|
*
* 部分算法参考wiki
* https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_Angles_to_Quaternion_Conversion
* https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
* https://en.wikipedia.org/wiki/Matrix_multiplication
* 被虐了一个下午总结下wiki的数学基础：
* 右手系 后右上
* 列主序矩阵 组合顺序从右到左
* Euler Pitch(y-axis), Yaw(z-axis), Roll(x-axis)
*
* UE4数学基础：
* 左手系 前右上
* 行主序矩阵 组合顺序从左到右
* Euler Pitch(y-axis), Yaw(z-axis), Roll(x-axis)、
*
* 果然一开始就借用高度优化的UE4数学算法不太现实，首先保证正确性，功能完整到一定程度再优化性能.
* 
* FbxAMatrix注释有点绕口，这里面的列和行应该不是同一个层面的概念，仅矩阵相乘时才需要注意列主序这个前提。
* 一般当作行主序矩阵访问。
* [引用]
* * Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), 
* * the last row of the matrix represents the translation part of the transformation.
* [/引用]
*
*/

#pragma once

namespace LostCore
{
	static const float SSmallFloat = 1.e-8f;
	static const float SSmallFloat2 = 1.e-4f;
	static const float SPI = 3.14159265359f;
	static const float SHalfPI = SPI * 0.5f;
	static const float SInvPI = (1.f / SPI);
	static const float SD2RConstant = SPI / 180.f;
	static const float SR2DConstant = 180.f / SPI;

	template<typename T>
	FORCEINLINE T Max(const T& left, const T& right)
	{
		return (left > right) ? left : right;
	}

	template <typename T>
	FORCEINLINE bool IsZero(T value, T tolerance = (T)SSmallFloat)
	{
		return std::abs(value) < tolerance;
	}

	template <typename T>
	FORCEINLINE bool IsEqual(T f1, T f2, T tolerance = (T)SSmallFloat)
	{
		return IsZero(f1 - f2, tolerance);
	}

	template <typename T1, typename T2>
	FORCEINLINE T1 SafeBy(const T1& value, const T2& by)
	{
		if (IsZero(by))
		{
			return value;
		}
		else
		{
			return value * (1.0 / by);
		}
	}

	FORCEINLINE void SinCos(float& oSin, float& oCos, float rad)
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

	FORCEINLINE void SinCos2(float& oSin, float& oCos, float rad)
	{
		oSin = sinf(rad);
		oCos = cosf(rad);
	}

	FORCEINLINE float Sqrt(float val)
	{
		return std::sqrt(val);
	}

	FORCEINLINE float InvSqrt(float val)
	{
		float s = Sqrt(val);
		return 1.f / s;
	}

	// return angle in the range (-180, 180]
	FORCEINLINE float ClampAngle(float deg)
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

	FORCEINLINE float Atan2(float y, float x)
	{
		return std::atan2(y, x);
	}

	FORCEINLINE float Asin(float val)
	{
		return std::asin(val);
	}

	FORCEINLINE bool IsValidEuler(float)
	{
		return true;
	}

	template <typename T>
	FORCEINLINE T InCircle(T value, T circle)
	{
		T result = value - ((int)(value / circle)) * circle;
		if (value < 0)
		{
			result += circle;
		}

		return result;
	}
	
	template <typename T>
	FORCEINLINE T InRange(T value, T rangeMin, T rangeMax)
	{
		return InCircle(value - rangeMin, rangeMax - rangeMin) + rangeMin;
	}
}