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
	template <typename T>
	class TVec2NonVectorized
	{
	public:
		T X;
		T Y;

	public:
		FORCEINLINE TVec2NonVectorized() {}
		FORCEINLINE TVec2NonVectorized(T x, T y);

		FORCEINLINE TVec2NonVectorized<T> operator-() const;
		FORCEINLINE TVec2NonVectorized<T> operator+(const TVec2NonVectorized<T>& vec) const;
		FORCEINLINE TVec2NonVectorized<T> operator-(const TVec2NonVectorized<T>& vec) const;
		FORCEINLINE TVec2NonVectorized<T> operator*(const TVec2NonVectorized<T>& vec) const;
		FORCEINLINE TVec2NonVectorized<T> operator/(const TVec2NonVectorized<T>& vec) const;

		FORCEINLINE TVec2NonVectorized<T> operator+=(const TVec2NonVectorized<T>& vec);
		FORCEINLINE TVec2NonVectorized<T> operator-=(const TVec2NonVectorized<T>& vec);
		FORCEINLINE TVec2NonVectorized<T> operator*=(const TVec2NonVectorized<T>& vec);
		FORCEINLINE TVec2NonVectorized<T> operator/=(const TVec2NonVectorized<T>& vec);

		FORCEINLINE TVec2NonVectorized<T> operator*(T scaler) const;
		FORCEINLINE TVec2NonVectorized<T> operator*=(T scaler);

		FORCEINLINE bool operator==(const TVec2NonVectorized<T>& vec) const;
		FORCEINLINE bool operator!=(const TVec2NonVectorized<T>& vec) const;

		FORCEINLINE T operator[](int32 index) const;
		FORCEINLINE T& operator[](int32 index);

		FORCEINLINE T operator|(const TVec2NonVectorized<T>& vec) const;
		FORCEINLINE T operator^(const TVec2NonVectorized<T>& vec) const;

		FORCEINLINE T SizeSquared() const;
		FORCEINLINE T Size() const;

		FORCEINLINE TVec2NonVectorized<T> GetRotated(T rad) const;
		FORCEINLINE TVec2NonVectorized<T> GetNormal() const;
		FORCEINLINE void Normalize();
		FORCEINLINE bool IsZero() const;
	};

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T>::TVec2NonVectorized(T x, T y) : X(x), Y(y)
	{

	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator-() const
	{
		return TVec2NonVectorized(-X, -Y);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator+(const TVec2NonVectorized<T>& vec) const
	{
		return TVec2NonVectorized(X + vec.X, Y + vec.Y);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator-(const TVec2NonVectorized<T>& vec) const
	{
		return TVec2NonVectorized(X - vec.X, Y - vec.Y);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator*(const TVec2NonVectorized<T>& vec) const
	{
		return TVec2NonVectorized(X * vec.X, Y * vec.Y);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator/(const TVec2NonVectorized<T>& vec) const
	{
		return TVec2NonVectorized(X / vec.X, Y / vec.Y);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator+=(const TVec2NonVectorized<T>& vec)
	{
		X += vec.X;
		Y += vec.Y;
		return *this;
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator-=(const TVec2NonVectorized<T>& vec)
	{
		X -= vec.X;
		Y -= vec.Y;
		return *this;
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator*=(const TVec2NonVectorized<T>& vec)
	{
		X *= vec.X;
		Y *= vec.Y;
		return *this;
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator/=(const TVec2NonVectorized<T>& vec)
	{
		X /= vec.X;
		Y /= vec.Y;
		return *this;
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator*(T scaler) const
	{
		return TVec2NonVectorized(X * scaler, Y * scaler);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::operator*=(T scaler)
	{
		X *= scaler;
		Y *= scaler;
		return *this;
	}

	template <typename T>
	FORCEINLINE bool TVec2NonVectorized<T>::operator==(const TVec2NonVectorized<T>& vec) const
	{
		return LostCore::IsEqual(X, vec.X) && LostCore::IsEqual(Y, vec.Y);
	}

	template <typename T>
	FORCEINLINE bool TVec2NonVectorized<T>::operator!=(const TVec2NonVectorized<T>& vec) const
	{
		return !(*this == vec);
	}

	template <typename T>
	FORCEINLINE T& TVec2NonVectorized<T>::operator[](int32 index)
	{
		return index == 0 ? X : Y;
	}

	template <typename T>
	FORCEINLINE T TVec2NonVectorized<T>::operator[](int32 index) const
	{
		return (*this)[index];
	}

	template <typename T>
	FORCEINLINE T TVec2NonVectorized<T>::operator|(const TVec2NonVectorized<T>& vec) const
	{
		return X * vec.X + Y * vec.Y;
	}

	template <typename T>
	FORCEINLINE T TVec2NonVectorized<T>::operator^(const TVec2NonVectorized<T>& vec) const
	{
		return X * vec.Y - Y * vec.X;
	}

	template <typename T>
	FORCEINLINE T TVec2NonVectorized<T>::SizeSquared() const
	{
		return X * X + Y * Y;
	}

	template <typename T>
	FORCEINLINE T TVec2NonVectorized<T>::Size() const
	{
		return std::sqrtf(SizeSquared());
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::GetRotated(T rad) const
	{
		T s = std::sinf(rad);
		T c = std::cosf(rad);
		return TVec2NonVectorized(c * X - s * Y, s * X + c * Y);
	}

	template <typename T>
	FORCEINLINE TVec2NonVectorized<T> TVec2NonVectorized<T>::GetNormal() const
	{
		const T squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const T scale = InvSqrt(squared);
			return TVec2NonVectorized(X * scale, Y * scale);
		}
		else
		{
			return TVec2NonVectorized(0.0, 0.0);
		}
	}

	template <typename T>
	FORCEINLINE void TVec2NonVectorized<T>::Normalize()
	{
		const T squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const T scale = InvSqrt(squared);
			X *= scale;
			Y *= scale;
		}
		else
		{
			X = 0.0;
			Y = 0.0;
		}
	}

	template <typename T>
	FORCEINLINE bool TVec2NonVectorized<T>::IsZero() const
	{
		return LostCore::IsZero(X) && LostCore::IsZero(Y);
	}

	template <typename T>
	FORCEINLINE void from_json(const FJson& j, TVec2NonVectorized<T>& val)
	{
		if (j.is_array() && j.size() >= 2)
		{
			val.X = j[0];
			val.Y = j[1];
		}
	}

	template <typename T>
	FORCEINLINE void to_json(FJson& j, const TVec2NonVectorized<T>& val)
	{
		j[0] = val.X;
		j[1] = val.Y;
	}

#ifdef TYPEDEF_DECL_VEC2
#error "TYPEDEF_DECL_VEC2 already defined somewhere else"
#endif

typedef TVec2NonVectorized<float> FFloat2;
typedef TVec2NonVectorized<double> FDouble2;
typedef TVec2NonVectorized<int32> FSInt2;
typedef TVec2NonVectorized<uint8> FByte2;
#define TYPEDEF_DECL_VEC2
}