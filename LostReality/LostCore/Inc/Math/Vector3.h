/*
* file Vector3.h
*
* author luoxw
* date 2017/02/08
*
*
*/

#pragma once

#include "MathBase.h"

namespace LostCore
{
	class FVec3NonVectorized
	{
	public:
		union
		{
			float X;
			float Roll;
		};
		
		union
		{
			float Y;
			float Pitch;
		};

		union
		{
			float Z;
			float Yaw;
		};

	public:
		INLINE FVec3NonVectorized() : X(0.f), Y(0.f), Z(0.f) {}
		INLINE FVec3NonVectorized(float x, float y, float z);

		INLINE FVec3NonVectorized operator-() const;
		INLINE FVec3NonVectorized operator+(const FVec3NonVectorized& vec) const;
		INLINE FVec3NonVectorized operator-(const FVec3NonVectorized& vec) const;
		INLINE FVec3NonVectorized operator*(const FVec3NonVectorized& vec) const;
		INLINE FVec3NonVectorized operator/(const FVec3NonVectorized& vec) const;
		INLINE FVec3NonVectorized operator*(float s) const;

		INLINE FVec3NonVectorized& operator+=(const FVec3NonVectorized& vec);
		INLINE FVec3NonVectorized& operator-=(const FVec3NonVectorized& vec);
		INLINE FVec3NonVectorized& operator*=(const FVec3NonVectorized& vec);
		INLINE FVec3NonVectorized& operator/=(const FVec3NonVectorized& vec);
		INLINE FVec3NonVectorized& operator*=(float s);

		INLINE bool operator==(const FVec3NonVectorized& vec) const;
		INLINE bool operator!=(const FVec3NonVectorized& vec) const;

		INLINE float operator[](int32 index) const;
		INLINE float& operator[](int32 index);

		INLINE float operator|(const FVec3NonVectorized& vec) const;
		INLINE FVec3NonVectorized operator^(const FVec3NonVectorized& vec) const;

		INLINE float SizeSquared() const;
		INLINE float Size() const;

		INLINE FVec3NonVectorized GetNormal() const;
		INLINE void Normalize();
		INLINE bool IsZero() const;

	public:
		INLINE static FVec3NonVectorized& GetZero()
		{
			static FVec3NonVectorized SZeroVec(0.f, 0.f, 0.f);
			return SZeroVec;
		}

		INLINE static FVec3NonVectorized GetReciprocal(const FVec3NonVectorized& vec)
		{
			FVec3NonVectorized result;
			result.X = LostCore::IsZero(vec.X) ? 0.f : (1.f / vec.X);
			result.Y = LostCore::IsZero(vec.Y) ? 0.f : (1.f / vec.Y);
			result.Z = LostCore::IsZero(vec.Z) ? 0.f : (1.f / vec.Z);
			return result;
		}
	};

	INLINE FVec3NonVectorized::FVec3NonVectorized(float x, float y, float z) : X(x), Y(y), Z(z)
	{

	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator-() const
	{
		return FVec3NonVectorized(-X, -Y, -Z);
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator+(const FVec3NonVectorized& vec) const
	{
		return FVec3NonVectorized(X + vec.X, Y + vec.Y, Z + vec.Z);
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator-(const FVec3NonVectorized& vec) const
	{
		return FVec3NonVectorized(X - vec.X, Y - vec.Y, Z - vec.Z);
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator*(const FVec3NonVectorized& vec) const
	{
		return FVec3NonVectorized(X * vec.X, Y * vec.Y, Z * vec.Z);
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator/(const FVec3NonVectorized& vec) const
	{
		return FVec3NonVectorized(X / vec.X, Y / vec.Y, Z / vec.Z);
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator*(float s) const
	{
		return FVec3NonVectorized(s*X, s*Y, s*Z);
	}

	INLINE FVec3NonVectorized& FVec3NonVectorized::operator+=(const FVec3NonVectorized& vec)
	{
		X += vec.X;
		Y += vec.Y;
		Z += vec.Z;
		return *this;
	}

	INLINE FVec3NonVectorized& FVec3NonVectorized::operator-=(const FVec3NonVectorized& vec)
	{
		X -= vec.X;
		Y -= vec.Y;
		Z -= vec.Z;
		return *this;
	}

	INLINE FVec3NonVectorized& FVec3NonVectorized::operator*=(const FVec3NonVectorized& vec)
	{
		X *= vec.X;
		Y *= vec.Y;
		Z *= vec.Z;
		return *this;
	}

	INLINE FVec3NonVectorized& FVec3NonVectorized::operator/=(const FVec3NonVectorized& vec)
	{
		X /= vec.X;
		Y /= vec.Y;
		Z /= vec.Z;
		return *this;
	}

	INLINE FVec3NonVectorized& FVec3NonVectorized::operator*=(float s)
	{
		X *= s;
		Y *= s;
		Z *= s;
		return *this;
	}

	INLINE bool FVec3NonVectorized::operator==(const FVec3NonVectorized& vec) const
	{
		return LostCore::IsEqual(X, vec.X) &&
			LostCore::IsEqual(Y, vec.Y) &&
			LostCore::IsEqual(Z, vec.Z);
	}

	INLINE bool FVec3NonVectorized::operator!=(const FVec3NonVectorized& vec) const
	{
		return !(*this == vec);
	}

	INLINE float& FVec3NonVectorized::operator[](int32 index)
	{
		return index == 0 ? X : (index == 1 ? Y : Z);
	}

	INLINE float FVec3NonVectorized::operator[](int32 index) const
	{
		return (*this)[index];
	}

	INLINE float FVec3NonVectorized::operator|(const FVec3NonVectorized& vec) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::operator^(const FVec3NonVectorized& vec) const
	{
		return FVec3NonVectorized(
			Y * vec.Z - Z * vec.Y,
			Z * vec.X - X * vec.Z,
			X * vec.Y - Y * vec.X);
	}

	INLINE float FVec3NonVectorized::SizeSquared() const
	{
		return X * X + Y * Y + Z * Z;
	}

	INLINE float FVec3NonVectorized::Size() const
	{
		return std::sqrtf(SizeSquared());
	}

	INLINE FVec3NonVectorized FVec3NonVectorized::GetNormal() const
	{
		const float squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const float scale = InvSqrt(squared);
			return FVec3NonVectorized(X * scale, Y * scale, Z * scale);
		}
		else
		{
			return FVec3NonVectorized(0.f, 0.f, 0.f);
		}
	}

	INLINE void FVec3NonVectorized::Normalize()
	{
		const float squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const float scale = InvSqrt(squared);
			X *= scale;
			Y *= scale;
			Z *= scale;
		}
		else
		{
			X = 0.f;
			Y = 0.f;
			Z = 0.f;
		}
	}

	INLINE bool FVec3NonVectorized::IsZero() const
	{
		return LostCore::IsZero(X) && LostCore::IsZero(Y) && LostCore::IsZero(Z);
	}

#ifdef TYPEDEF_DECL_FVEC3
#error "FVec3 already defined somewhere else"
#else
	typedef FVec3NonVectorized FVec3;
	#define TYPEDEF_DECL_FVEC3
#endif
}