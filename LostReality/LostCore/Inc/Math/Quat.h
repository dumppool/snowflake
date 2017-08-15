/*
* file Quat.h
*
* author luoxw
* date 2017/02/08
*
*
*/

#pragma once

#include "Vector3.h"

namespace LostCore
{
	__declspec(align(16)) class FQuatNonVectorized
	{
	public:

		float X;
		float Y;
		float Z;
		float W;

	public:
		FORCEINLINE FQuatNonVectorized() {}
		FORCEINLINE FQuatNonVectorized(float x, float y, float z, float w);
		FORCEINLINE FQuatNonVectorized(const FQuatNonVectorized& quat);

		FORCEINLINE FQuatNonVectorized& operator=(const FQuatNonVectorized& quat);
		FORCEINLINE FQuatNonVectorized  operator+(const FQuatNonVectorized& quat) const;
		FORCEINLINE FQuatNonVectorized& operator+=(const FQuatNonVectorized& quat);
		FORCEINLINE FQuatNonVectorized  operator-(const FQuatNonVectorized& quat) const;
		FORCEINLINE FQuatNonVectorized& operator-=(const FQuatNonVectorized& quat);
		FORCEINLINE FQuatNonVectorized  operator*(const FQuatNonVectorized& quat) const;
		FORCEINLINE FQuatNonVectorized& operator*=(const FQuatNonVectorized& quat);
		FORCEINLINE FQuatNonVectorized  operator*(float value) const;
		FORCEINLINE FQuatNonVectorized& operator*=(float value);

		FORCEINLINE bool operator==(const FQuatNonVectorized& quat) const;
		FORCEINLINE bool operator!=(const FQuatNonVectorized& quat) const;
		FORCEINLINE float operator|(const FQuatNonVectorized& quat) const;

		FORCEINLINE FFloat3 Euler() const;
		FORCEINLINE FQuatNonVectorized& FromEuler(const FFloat3& euler);

		FORCEINLINE FQuatNonVectorized GetNormalized() const;
		FORCEINLINE FQuatNonVectorized& Normalize();
		FORCEINLINE bool IsNormalized() const;
		FORCEINLINE float SizeSquared() const;
		FORCEINLINE float Size() const;

		FORCEINLINE FFloat3 RotateVector(const FFloat3& vec) const;
		FORCEINLINE FQuatNonVectorized GetInversed() const;
		FORCEINLINE FQuatNonVectorized& Inverse();

		FORCEINLINE FFloat3 GetForwardVector() const;
		FORCEINLINE FFloat3 GetUpVector() const;
		FORCEINLINE FFloat3 GetRightVector() const;

	public:
		static const FQuatNonVectorized& GetIdentity()
		{
			static FQuatNonVectorized SIdentity(0.f, 0.f, 0.f, 1.f);
			return SIdentity;
		}
	};

	FORCEINLINE FQuatNonVectorized::FQuatNonVectorized(float x, float y, float z, float w) :
		X(x), Y(y), Z(z), W(w)
	{
	}

	FORCEINLINE FQuatNonVectorized::FQuatNonVectorized(const FQuatNonVectorized& quat) :
		X(quat.X), Y(quat.Y), Z(quat.Z), W(quat.W)
	{
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::operator=(const FQuatNonVectorized& quat)
	{
		X = quat.X;
		Y = quat.Y;
		Z = quat.Z;
		W = quat.W;
		return *this;
	}

	FORCEINLINE FQuatNonVectorized FQuatNonVectorized::operator+(const FQuatNonVectorized& quat) const
	{
		return FQuatNonVectorized(X + quat.X, Y + quat.Y, Z + quat.Z, W + quat.W);
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::operator+=(const FQuatNonVectorized& quat)
	{
		X += quat.X;
		Y += quat.Y;
		Z += quat.Z;
		W += quat.W;
	}

	FORCEINLINE FQuatNonVectorized FQuatNonVectorized::operator-(const FQuatNonVectorized& quat) const
	{
		return FQuatNonVectorized(X - quat.X, Y - quat.Y, Z - quat.Z, W - quat.W);
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::operator-=(const FQuatNonVectorized& quat)
	{
		X -= quat.X;
		Y -= quat.Y;
		Z -= quat.Z;
		W -= quat.W;
	}

	FORCEINLINE FQuatNonVectorized  FQuatNonVectorized::operator*(const FQuatNonVectorized& quat) const
	{
		return FQuatNonVectorized(
			W*quat.X + X*quat.W + Y*quat.Z - Z*quat.Y,
			W*quat.Y - X*quat.Z + Y*quat.W + Z*quat.X,
			W*quat.Z + X*quat.Y - Y*quat.X + Z*quat.W,
			W*quat.W - X*quat.X - Y*quat.Y - Z*quat.Z);
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::operator*=(const FQuatNonVectorized& quat)
	{
		*this = (*this)*quat;
		return *this;
	}

	FORCEINLINE FQuatNonVectorized FQuatNonVectorized::operator*(float value) const
	{
		return FQuatNonVectorized(X*value, Y*value, Z*value, W*value);
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::operator*=(float value)
	{
		X *= value;
		Y *= value;
		Z *= value;
		W *= value;
		return *this;
	}

	FORCEINLINE bool FQuatNonVectorized::operator==(const FQuatNonVectorized& quat) const
	{
		return LostCore::IsEqual(X, quat.X) &&
			LostCore::IsEqual(Y, quat.Y) &&
			LostCore::IsEqual(Z, quat.Z) &&
			LostCore::IsEqual(W, quat.W);
	}

	FORCEINLINE bool FQuatNonVectorized::operator!=(const FQuatNonVectorized& quat) const
	{
		return !(*this == quat);
	}

	FORCEINLINE float FQuatNonVectorized::operator|(const FQuatNonVectorized& quat) const
	{
		return X*quat.X + Y*quat.Y + Z*quat.Z + W*quat.W;
	}

	FORCEINLINE FFloat3 FQuatNonVectorized::Euler() const
	{
		FFloat3 euler;

		//const float singularity = Y*Z - W*X;
		//const float yawY = 2.f * (W*Y + Z*X);
		//const float yawX = (1.f - 2.f*(X*X + Y*Y));
		//const float threshold = 0.4999995f;
		//if (singularity < -threshold)
		//{
		//	euler.Pitch = -90.f;
		//	euler.Yaw = std::atan2(yawY, yawX) * SR2DConstant;
		//	euler.Roll = ClampAngle(-euler.Yaw - (2.f * std::atan2(Z, W) * SR2DConstant));
		//}
		//else if (singularity > threshold)
		//{
		//	euler.Pitch = 90.f;
		//	euler.Yaw = Atan2(yawY, yawX) * SR2DConstant;
		//	euler.Roll = ClampAngle(euler.Yaw - (2.f * Atan2(Z, W) * SR2DConstant));
		//}
		//else
		//{
		//	euler.Pitch = Asin(2.f * singularity) * SR2DConstant;
		//	euler.Yaw = Atan2(yawY, yawX) * SR2DConstant;
		//	euler.Roll = Atan2(-2.f * (W*Z + X*Y), (1.f - 2.f*(Z*Z + X*X))) * SR2DConstant;
		//}

		float yy = Y*Y;
		
		float t0 = 2.f * (-W*X - Y*Z);
		float t1 = 1.f - 2.f*(X*X + yy);
		euler.Pitch = Atan2(t0, t1) * SR2DConstant;

		float t2 = 2.f*(-W*Y + Z*X);
		t2 = t2 > 1.f ? 1.f : t2;
		t2 = t2 < -1.f ? -1.f : t2;
		euler.Yaw = -Asin(t2) * SR2DConstant;

		float t3 = 2.f*(W*Z + X*Y);
		float t4 = 1.f - 2.f*(yy + Z*Z);
		euler.Roll = Atan2(t3, t4) * SR2DConstant;

		return euler;
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::FromEuler(const FFloat3& euler)
	{
		const float halfD2R = SD2RConstant * 0.5f;
		float t0, t1, t2, t3, t4, t5;
		SinCos(t1, t0, euler.Roll * halfD2R);
		SinCos(t3, t2, euler.Pitch * halfD2R);
		SinCos(t5, t4, -euler.Yaw * halfD2R);

		W = -t0*t2*t4 - t1*t3*t5;
		X = t0*t3*t4 - t1*t2*t5;
		Y = t0*t2*t5 + t1*t3*t4;
		Z = -t1*t2*t4 + t0*t3*t5;

		return *this;
	}

	FORCEINLINE FQuatNonVectorized FQuatNonVectorized::GetNormalized() const
	{
		FQuatNonVectorized quat(*this);
		return quat.Normalize();
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::Normalize()
	{
		const float squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const float scale = InvSqrt(squared);
			X *= scale;
			Y *= scale;
			Z *= scale;
			W *= scale;
		}
		else
		{
			*this = FQuatNonVectorized::GetIdentity();
		}

		return *this;
	}

	FORCEINLINE bool FQuatNonVectorized::IsNormalized() const
	{
		return LostCore::IsEqual(1.f, SizeSquared());
	}

	FORCEINLINE float FQuatNonVectorized::SizeSquared() const
	{
		return X*X + Y*Y + Z*Z + W*W;
	}

	FORCEINLINE float FQuatNonVectorized::Size() const
	{
		return Sqrt(SizeSquared());
	}

	FORCEINLINE FFloat3 FQuatNonVectorized::RotateVector(const FFloat3& vec) const
	{
		const FFloat3 q(X, Y, Z);
		const FFloat3 t((q.Cross(vec)) * 2.f);
		return FFloat3(vec + (t * W) + (q.Cross(t)));
	}

	FORCEINLINE FQuatNonVectorized FQuatNonVectorized::GetInversed() const
	{
		return FQuatNonVectorized(-X, -Y, -Z, W);
	}

	FORCEINLINE FQuatNonVectorized& FQuatNonVectorized::Inverse()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
		return *this;
	}

	FORCEINLINE FFloat3 FQuatNonVectorized::GetForwardVector() const
	{
		return RotateVector(FFloat3(0.f, 0.f, 1.f));
	}

	FORCEINLINE FFloat3 FQuatNonVectorized::GetUpVector() const
	{
		return RotateVector(FFloat3(0.f, 1.f, 0.f));
	}

	FORCEINLINE FFloat3 FQuatNonVectorized::GetRightVector() const
	{
		return RotateVector(FFloat3(1.f, 0.f, 0.f));
	}

#ifdef TYPEDEF_DECL_FQUAT
#error "FQuat already defined somewhere else"
#else
	typedef FQuatNonVectorized FQuat;
	#define TYPEDEF_DECL_FQUAT
#endif
}