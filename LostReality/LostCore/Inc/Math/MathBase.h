/*
* file MathBase.h
*
* author luoxw
* date 2017/02/08
*
* ��ѧ������
* ΢������ϵ ����ǰ
* ��������� ���˳�������
* Euler Pitch(x-axis), Yaw(y-axis), Roll(z-axis)
* 
* ����ϵ������ϵ��ת���ο�
* UnrealEngine\Engine\Source\Editor\UnrealEd\Private\Fbx\FbxUtilsImport.cpp
* Vector				����(X, Y, Z) -> ����(X, Y, -Z)
* Euler					����(Pitch, Yaw, Roll) -> ����(Pitch, -Yaw, Roll��
* Quaternion			����(Qx, Qy, Qz, Qw) -> ����(Qx, Qy, -Qz, -Qw)
* Matrix(affine)		����	| M00, M01, M02, 0 |	����	| M00, -M01, M02, 0	|
*							| M10, M11, M12, 0 |	->	| M10, -M11, M12, 0	|
*							| M20, M21, M22, 0 |		| -M20, M21, -M22, 0|
*							| M30, M31, M32, 1 |		| M30, -M31, M32, 1	|
*
* �����㷨�ο�wiki
* https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_Angles_to_Quaternion_Conversion
* https://en.wikipedia.org/wiki/Rotation_matrix#Quaternion
* https://en.wikipedia.org/wiki/Matrix_multiplication
* ��Ű��һ�������ܽ���wiki����ѧ������
* ����ϵ ������
* ��������� ���˳����ҵ���
* Euler Pitch(y-axis), Yaw(z-axis), Roll(x-axis)
*
* UE4��ѧ������
* ����ϵ ǰ����
* ��������� ���˳�������
* Euler Pitch(y-axis), Yaw(z-axis), Roll(x-axis)��
*
* ��Ȼһ��ʼ�ͽ��ø߶��Ż���UE4��ѧ�㷨��̫��ʵ�����ȱ�֤��ȷ�ԣ�����������һ���̶����Ż�����.
* 
* FbxAMatrixע���е��ƿڣ���������к���Ӧ�ò���ͬһ������ĸ�����������ʱ����Ҫע�����������ǰ�ᡣ
* һ�㵱�������������ʡ�
* [����]
* * Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), 
* * the last row of the matrix represents the translation part of the transformation.
* [/����]
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