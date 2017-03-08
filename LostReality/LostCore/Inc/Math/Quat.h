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
		INLINE FQuatNonVectorized() {}
		INLINE FQuatNonVectorized(float x, float y, float z, float w);
		INLINE FQuatNonVectorized(const FQuatNonVectorized& quat);

		INLINE FQuatNonVectorized& operator=(const FQuatNonVectorized& quat);
		INLINE FQuatNonVectorized  operator+(const FQuatNonVectorized& quat) const;
		INLINE FQuatNonVectorized& operator+=(const FQuatNonVectorized& quat);
		INLINE FQuatNonVectorized  operator-(const FQuatNonVectorized& quat) const;
		INLINE FQuatNonVectorized& operator-=(const FQuatNonVectorized& quat);
		INLINE FQuatNonVectorized  operator*(const FQuatNonVectorized& quat) const;
		INLINE FQuatNonVectorized& operator*=(const FQuatNonVectorized& quat);
		INLINE FQuatNonVectorized  operator*(float value) const;
		INLINE FQuatNonVectorized& operator*=(float value);

		INLINE bool operator==(const FQuatNonVectorized& quat) const;
		INLINE bool operator!=(const FQuatNonVectorized& quat) const;
		INLINE float operator|(const FQuatNonVectorized& quat) const;

		INLINE FVec3 Euler() const;
		INLINE FQuatNonVectorized& FromEuler(const FVec3& euler);

		INLINE FQuatNonVectorized GetNormalized() const;
		INLINE FQuatNonVectorized& Normalize();
		INLINE bool IsNormalized() const;
		INLINE float SizeSquared() const;
		INLINE float Size() const;

		INLINE FVec3 RotateVector(const FVec3& vec) const;
		INLINE FQuatNonVectorized GetInversed() const;
		INLINE FQuatNonVectorized& Inverse();

		INLINE FVec3 GetForwardVector() const;
		INLINE FVec3 GetUpVector() const;
		INLINE FVec3 GetRightVector() const;

	public:
		static const FQuatNonVectorized& GetIdentity()
		{
			static FQuatNonVectorized SIdentity(0.f, 0.f, 0.f, 1.f);
			return SIdentity;
		}
	};

	INLINE FQuatNonVectorized::FQuatNonVectorized(float x, float y, float z, float w) :
		X(x), Y(y), Z(z), W(w)
	{
	}

	INLINE FQuatNonVectorized::FQuatNonVectorized(const FQuatNonVectorized& quat) :
		X(quat.X), Y(quat.Y), Z(quat.Z), W(quat.W)
	{
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::operator=(const FQuatNonVectorized& quat)
	{
		X = quat.X;
		Y = quat.Y;
		Z = quat.Z;
		W = quat.W;
	}

	INLINE FQuatNonVectorized FQuatNonVectorized::operator+(const FQuatNonVectorized& quat) const
	{
		return FQuatNonVectorized(X + quat.X, Y + quat.Y, Z + quat.Z, W + quat.W);
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::operator+=(const FQuatNonVectorized& quat)
	{
		X += quat.X;
		Y += quat.Y;
		Z += quat.Z;
		W += quat.W;
	}

	INLINE FQuatNonVectorized FQuatNonVectorized::operator-(const FQuatNonVectorized& quat) const
	{
		return FQuatNonVectorized(X - quat.X, Y - quat.Y, Z - quat.Z, W - quat.W);
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::operator-=(const FQuatNonVectorized& quat)
	{
		X -= quat.X;
		Y -= quat.Y;
		Z -= quat.Z;
		W -= quat.W;
	}

	INLINE FQuatNonVectorized  FQuatNonVectorized::operator*(const FQuatNonVectorized& quat) const
	{
		return FQuatNonVectorized(
			W*quat.X + X*quat.W + Y*quat.Z - Z*quat.Y,
			W*quat.Y - X*quat.Z + Y*quat.W + Z*quat.X,
			W*quat.Z + X*quat.Y - Y*quat.X + Z*quat.W,
			W*quat.W - X*quat.X - Y*quat.Y - Z*quat.Z);
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::operator*=(const FQuatNonVectorized& quat)
	{
		*this = (*this)*quat;
		return *this;
	}

	INLINE FQuatNonVectorized FQuatNonVectorized::operator*(float value) const
	{
		return FQuatNonVectorized(X*value, Y*value, Z*value, W*value);
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::operator*=(float value)
	{
		X *= value;
		Y *= value;
		Z *= value;
		W *= value;
		return *this;
	}

	INLINE bool FQuatNonVectorized::operator==(const FQuatNonVectorized& quat) const
	{
		return LostCore::IsEqual(X, quat.X) &&
			LostCore::IsEqual(Y, quat.Y) &&
			LostCore::IsEqual(Z, quat.Z) &&
			LostCore::IsEqual(W, quat.W);
	}

	INLINE bool FQuatNonVectorized::operator!=(const FQuatNonVectorized& quat) const
	{
		return !(*this == quat);
	}

	INLINE float FQuatNonVectorized::operator|(const FQuatNonVectorized& quat) const
	{
		return X*quat.X + Y*quat.Y + Z*quat.Z + W*quat.W;
	}

	INLINE FVec3 FQuatNonVectorized::Euler() const
	{
		FVec3 euler;

		const float singularity = Y*Z - W*X;
		const float yawY = 2.f * (W*Y + Z*X);
		const float yawX = (1.f - 2.f*(X*X + Y*Y));
		const float threshold = 0.4999995f;
		if (singularity < -threshold)
		{
			euler.Pitch = -90.f;
			euler.Yaw = std::atan2(yawY, yawX) * SR2DConstant;
			euler.Roll = ClampAngle(-euler.Yaw - (2.f * std::atan2(Z, W) * SR2DConstant));
		}
		else if (singularity > threshold)
		{
			euler.Pitch = 90.f;
			euler.Yaw = Atan2(yawY, yawX) * SR2DConstant;
			euler.Roll = ClampAngle(euler.Yaw - (2.f * Atan2(Z, W) * SR2DConstant));
		}
		else
		{
			euler.Pitch = Asin(2.f * singularity) * SR2DConstant;
			euler.Yaw = Atan2(yawY, yawX) * SR2DConstant;
			euler.Roll = Atan2(-2.f * (W*Z + X*Y), (1.f - 2.f*(Z*Z + X*X))) * SR2DConstant;
		}

		return euler;
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::FromEuler(const FVec3& euler)
	{
		const float halfD2R = SD2RConstant * 0.5f;
		float SP, SY, SR;
		float CP, CY, CR;
		SinCos(SP, CP, euler.Pitch * halfD2R);
		SinCos(SY, CY, euler.Yaw * halfD2R);
		SinCos(SR, CR, euler.Roll * halfD2R);

		Z = CR*SP*SY - SR*CP*CY;
		X = -CR*SP*CY - SR*CP*SY;
		Y = CR*CP*SY - SR*SP*CY;
		W = CR*CP*CY + SR*SP*SY;

		return *this;
	}

	INLINE FQuatNonVectorized FQuatNonVectorized::GetNormalized() const
	{
		FQuatNonVectorized quat(*this);
		return quat.Normalize();
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::Normalize()
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

	INLINE bool FQuatNonVectorized::IsNormalized() const
	{
		return LostCore::IsEqual(1.f, SizeSquared());
	}

	INLINE float FQuatNonVectorized::SizeSquared() const
	{
		return X*X + Y*Y + Z*Z + W*W;
	}

	INLINE float FQuatNonVectorized::Size() const
	{
		return Sqrt(SizeSquared());
	}

	INLINE FVec3 FQuatNonVectorized::RotateVector(const FVec3& vec) const
	{
		const FVec3 q(X, Y, Z);
		const FVec3 t((q^vec) * 2.f);
		return FVec3(vec + (t * W) + (q^t));
	}

	INLINE FQuatNonVectorized FQuatNonVectorized::GetInversed() const
	{
		return FQuatNonVectorized(-X, -Y, -Z, W);
	}

	INLINE FQuatNonVectorized& FQuatNonVectorized::Inverse()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
		return *this;
	}

	INLINE FVec3 FQuatNonVectorized::GetForwardVector() const
	{
		return RotateVector(FVec3(0.f, 0.f, 1.f));
	}

	INLINE FVec3 FQuatNonVectorized::GetUpVector() const
	{
		return RotateVector(FVec3(0.f, 1.f, 0.f));
	}

	INLINE FVec3 FQuatNonVectorized::GetRightVector() const
	{
		return RotateVector(FVec3(1.f, 0.f, 0.f));
	}

#ifdef TYPEDEF_DECL_FQUAT
#error "FQuat already defined somewhere else"
#else
	typedef FQuatNonVectorized FQuat;
	#define TYPEDEF_DECL_FQUAT
#endif
}