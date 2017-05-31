/*
* file Vector2.h
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
	class FVec2NonVectorized
	{
	public:
		float X;
		float Y;

	public:
		INLINE FVec2NonVectorized() {}
		INLINE FVec2NonVectorized(float x, float y);

		INLINE FVec2NonVectorized operator-() const;
		INLINE FVec2NonVectorized operator+(const FVec2NonVectorized& vec) const;
		INLINE FVec2NonVectorized operator-(const FVec2NonVectorized& vec) const;
		INLINE FVec2NonVectorized operator*(const FVec2NonVectorized& vec) const;
		INLINE FVec2NonVectorized operator/(const FVec2NonVectorized& vec) const;

		INLINE FVec2NonVectorized operator+=(const FVec2NonVectorized& vec);
		INLINE FVec2NonVectorized operator-=(const FVec2NonVectorized& vec);
		INLINE FVec2NonVectorized operator*=(const FVec2NonVectorized& vec);
		INLINE FVec2NonVectorized operator/=(const FVec2NonVectorized& vec);

		INLINE FVec2NonVectorized operator*(float scaler) const;
		INLINE FVec2NonVectorized operator*=(float scaler);

		INLINE bool operator==(const FVec2NonVectorized& vec) const;
		INLINE bool operator!=(const FVec2NonVectorized& vec) const;

		INLINE float operator[](int32 index) const;
		INLINE float& operator[](int32 index);

		INLINE float operator|(const FVec2NonVectorized& vec) const;
		INLINE float operator^(const FVec2NonVectorized& vec) const;

		INLINE float SizeSquared() const;
		INLINE float Size() const;

		INLINE FVec2NonVectorized GetRotated(float rad) const;
		INLINE FVec2NonVectorized GetNormal() const;
		INLINE void Normalize();
		INLINE bool IsZero() const;
	};

	INLINE FVec2NonVectorized::FVec2NonVectorized(float x, float y) : X(x), Y(y)
	{

	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator-() const
	{
		return FVec2NonVectorized(-X, -Y);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator+(const FVec2NonVectorized& vec) const
	{
		return FVec2NonVectorized(X + vec.X, Y + vec.Y);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator-(const FVec2NonVectorized& vec) const
	{
		return FVec2NonVectorized(X - vec.X, Y - vec.Y);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator*(const FVec2NonVectorized& vec) const
	{
		return FVec2NonVectorized(X * vec.X, Y * vec.Y);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator/(const FVec2NonVectorized& vec) const
	{
		return FVec2NonVectorized(X / vec.X, Y / vec.Y);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator+=(const FVec2NonVectorized& vec)
	{
		X += vec.X;
		Y += vec.Y;
		return *this;
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator-=(const FVec2NonVectorized& vec)
	{
		X -= vec.X;
		Y -= vec.Y;
		return *this;
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator*=(const FVec2NonVectorized& vec)
	{
		X *= vec.X;
		Y *= vec.Y;
		return *this;
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator/=(const FVec2NonVectorized& vec)
	{
		X /= vec.X;
		Y /= vec.Y;
		return *this;
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator*(float scaler) const
	{
		return FVec2NonVectorized(X * scaler, Y * scaler);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::operator*=(float scaler)
	{
		X *= scaler;
		Y *= scaler;
		return *this;
	}

	INLINE bool FVec2NonVectorized::operator==(const FVec2NonVectorized& vec) const
	{
		return LostCore::IsEqual(X, vec.X) && LostCore::IsEqual(Y, vec.Y);
	}

	INLINE bool FVec2NonVectorized::operator!=(const FVec2NonVectorized& vec) const
	{
		return !(*this == vec);
	}

	INLINE float& FVec2NonVectorized::operator[](int32 index)
	{
		return index == 0 ? X : Y;
	}

	INLINE float FVec2NonVectorized::operator[](int32 index) const
	{
		return (*this)[index];
	}

	INLINE float FVec2NonVectorized::operator|(const FVec2NonVectorized& vec) const
	{
		return X * vec.X + Y * vec.Y;
	}

	INLINE float FVec2NonVectorized::operator^(const FVec2NonVectorized& vec) const
	{
		return X * vec.Y - Y * vec.X;
	}

	INLINE float FVec2NonVectorized::SizeSquared() const
	{
		return X * X + Y * Y;
	}

	INLINE float FVec2NonVectorized::Size() const
	{
		return std::sqrtf(SizeSquared());
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::GetRotated(float rad) const
	{
		float s = std::sinf(rad);
		float c = std::cosf(rad);
		return FVec2NonVectorized(c * X - s * Y, s * X + c * Y);
	}

	INLINE FVec2NonVectorized FVec2NonVectorized::GetNormal() const
	{
		const float squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const float scale = InvSqrt(squared);
			return FVec2NonVectorized(X * scale, Y * scale);
		}
		else
		{
			return FVec2NonVectorized(0.f, 0.f);
		}
	}

	INLINE void FVec2NonVectorized::Normalize()
	{
		const float squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const float scale = InvSqrt(squared);
			X *= scale;
			Y *= scale;
		}
		else
		{
			X = 0.f;
			Y = 0.f;
		}
	}

	INLINE bool FVec2NonVectorized::IsZero() const
	{
		return LostCore::IsZero(X) && LostCore::IsZero(Y);
	}

	INLINE void from_json(const FJson& j, FVec2NonVectorized& val)
	{
		if (j.is_array() && j.size() >= 2)
		{
			val.X = j[0];
			val.Y = j[1];
		}
}

	INLINE void to_json(FJson& j, const FVec2NonVectorized& val)
	{
		j[0] = val.X;
		j[1] = val.Y;
	}

#ifdef TYPEDEF_DECL_FVEC2
#error "FVec2 already defined somewhere else"
#else
	typedef FVec2NonVectorized FVec2;
	#define TYPEDEF_DECL_FVEC2
#endif
}