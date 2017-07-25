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
	template <typename T>
	class TVec3NonVectorized
	{
	public:
		union
		{
			T X;
			T Pitch;
		};
		
		union
		{
			T Y;
			T Yaw;
		};

		union
		{
			T Z;
			T Roll;
		};

	public:
		INLINE TVec3NonVectorized() : X(0.0), Y(0.0), Z(0.0) {}
		INLINE TVec3NonVectorized(T x, T y, T z);

		INLINE TVec3NonVectorized<T> operator-() const;
		INLINE TVec3NonVectorized<T> operator+(const TVec3NonVectorized<T>& vec) const;
		INLINE TVec3NonVectorized<T> operator-(const TVec3NonVectorized<T>& vec) const;
		INLINE TVec3NonVectorized<T> operator*(const TVec3NonVectorized<T>& vec) const;
		INLINE TVec3NonVectorized<T> operator/(const TVec3NonVectorized<T>& vec) const;
		INLINE TVec3NonVectorized<T> operator*(T s) const;

		INLINE TVec3NonVectorized<T>& operator+=(const TVec3NonVectorized<T>& vec);
		INLINE TVec3NonVectorized<T>& operator-=(const TVec3NonVectorized<T>& vec);
		INLINE TVec3NonVectorized<T>& operator*=(const TVec3NonVectorized<T>& vec);
		INLINE TVec3NonVectorized<T>& operator/=(const TVec3NonVectorized<T>& vec);
		INLINE TVec3NonVectorized<T>& operator*=(T s);

		INLINE bool operator==(const TVec3NonVectorized<T>& vec) const;
		INLINE bool operator!=(const TVec3NonVectorized<T>& vec) const;

		INLINE T operator[](int32 index) const;
		INLINE T& operator[](int32 index);

		INLINE T operator|(const TVec3NonVectorized<T>& vec) const;
		INLINE TVec3NonVectorized<T> operator^(const TVec3NonVectorized<T>& vec) const;

		INLINE T SizeSquared() const;
		INLINE T Size() const;

		INLINE TVec3NonVectorized<T> GetNormal() const;
		INLINE void Normalize();
		INLINE bool IsZero() const;

	public:
		INLINE static TVec3NonVectorized<T>& GetZero()
		{
			static TVec3NonVectorized<T> SZeroVec(0.0, 0.0, 0.0);
			return SZeroVec;
		}

		INLINE static TVec3NonVectorized<T> GetReciprocal(const TVec3NonVectorized<T>& vec)
		{
			TVec3NonVectorized<T> result;
			result.X = LostCore::IsZero(vec.X) ? 0.0 : (1.0 / vec.X);
			result.Y = LostCore::IsZero(vec.Y) ? 0.0 : (1.0 / vec.Y);
			result.Z = LostCore::IsZero(vec.Z) ? 0.0 : (1.0 / vec.Z);
			return result;
		}
	};

	template <typename T>
	INLINE TVec3NonVectorized<T>::TVec3NonVectorized(T x, T y, T z) : X(x), Y(y), Z(z)
	{

	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator-() const
	{
		return TVec3NonVectorized<T>(-X, -Y, -Z);
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator+(const TVec3NonVectorized<T>& vec) const
	{
		return TVec3NonVectorized<T>(X + vec.X, Y + vec.Y, Z + vec.Z);
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator-(const TVec3NonVectorized<T>& vec) const
	{
		return TVec3NonVectorized<T>(X - vec.X, Y - vec.Y, Z - vec.Z);
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator*(const TVec3NonVectorized<T>& vec) const
	{
		return TVec3NonVectorized<T>(X * vec.X, Y * vec.Y, Z * vec.Z);
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator/(const TVec3NonVectorized<T>& vec) const
	{
		return TVec3NonVectorized<T>(X / vec.X, Y / vec.Y, Z / vec.Z);
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator*(T s) const
	{
		return TVec3NonVectorized<T>(s*X, s*Y, s*Z);
	}

	template <typename T>
	INLINE TVec3NonVectorized<T>& TVec3NonVectorized<T>::operator+=(const TVec3NonVectorized<T>& vec)
	{
		X += vec.X;
		Y += vec.Y;
		Z += vec.Z;
		return *this;
	}

	template <typename T>
	INLINE TVec3NonVectorized<T>& TVec3NonVectorized<T>::operator-=(const TVec3NonVectorized<T>& vec)
	{
		X -= vec.X;
		Y -= vec.Y;
		Z -= vec.Z;
		return *this;
	}

	template <typename T>
	INLINE TVec3NonVectorized<T>& TVec3NonVectorized<T>::operator*=(const TVec3NonVectorized<T>& vec)
	{
		X *= vec.X;
		Y *= vec.Y;
		Z *= vec.Z;
		return *this;
	}

	template <typename T>
	INLINE TVec3NonVectorized<T>& TVec3NonVectorized<T>::operator/=(const TVec3NonVectorized<T>& vec)
	{
		X /= vec.X;
		Y /= vec.Y;
		Z /= vec.Z;
		return *this;
	}

	template <typename T>
	INLINE TVec3NonVectorized<T>& TVec3NonVectorized<T>::operator*=(T s)
	{
		X *= s;
		Y *= s;
		Z *= s;
		return *this;
	}

	template <typename T>
	INLINE bool TVec3NonVectorized<T>::operator==(const TVec3NonVectorized<T>& vec) const
	{
		return LostCore::IsEqual(X, vec.X) &&
			LostCore::IsEqual(Y, vec.Y) &&
			LostCore::IsEqual(Z, vec.Z);
	}

	template <typename T>
	INLINE bool TVec3NonVectorized<T>::operator!=(const TVec3NonVectorized<T>& vec) const
	{
		return !(*this == vec);
	}

	template <typename T>
	INLINE T& TVec3NonVectorized<T>::operator[](int32 index)
	{
		return index == 0 ? X : (index == 1 ? Y : Z);
	}

	template <typename T>
	INLINE T TVec3NonVectorized<T>::operator[](int32 index) const
	{
		return (*this)[index];
	}

	template <typename T>
	INLINE T TVec3NonVectorized<T>::operator|(const TVec3NonVectorized<T>& vec) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::operator^(const TVec3NonVectorized<T>& vec) const
	{
		return TVec3NonVectorized<T>(
			Y * vec.Z - Z * vec.Y,
			Z * vec.X - X * vec.Z,
			X * vec.Y - Y * vec.X);
	}

	template <typename T>
	INLINE T TVec3NonVectorized<T>::SizeSquared() const
	{
		return X * X + Y * Y + Z * Z;
	}

	template <typename T>
	INLINE T TVec3NonVectorized<T>::Size() const
	{
		return std::sqrtf(SizeSquared());
	}

	template <typename T>
	INLINE TVec3NonVectorized<T> TVec3NonVectorized<T>::GetNormal() const
	{
		const T squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const T scale = InvSqrt(squared);
			return TVec3NonVectorized<T>(X * scale, Y * scale, Z * scale);
		}
		else
		{
			return TVec3NonVectorized<T>(0.0, 0.0, 0.0);
		}
	}

	template <typename T>
	INLINE void TVec3NonVectorized<T>::Normalize()
	{
		const T squared = SizeSquared();
		if (squared > SSmallFloat)
		{
			const T scale = InvSqrt(squared);
			X *= scale;
			Y *= scale;
			Z *= scale;
		}
		else
		{
			X = 0.0;
			Y = 0.0;
			Z = 0.0;
		}
	}

	template <typename T>
	INLINE bool TVec3NonVectorized<T>::IsZero() const
	{
		return LostCore::IsZero(X) && LostCore::IsZero(Y) && LostCore::IsZero(Z);
	}

	template <typename T>
	INLINE void from_json(const FJson& j, TVec3NonVectorized<T>& val)
	{
		if (j.is_array() && j.size() >= 3)
		{
			val.X = j[0];
			val.Y = j[1];
			val.Z = j[2];
		}
	}

	template <typename T>
	INLINE void to_json(FJson& j, const TVec3NonVectorized<T>& val)
	{
		j[0] = val.X;
		j[1] = val.Y;
		j[2] = val.Z;
	}

#ifdef TYPEDEF_DECL_VEC3
#error "TYPEDEF_DECL_VEC3 already defined somewhere else"
#endif

typedef TVec3NonVectorized<float> FFloat3;
typedef TVec3NonVectorized<double> FDouble3;
typedef TVec3NonVectorized<int32> FSInt3;
typedef TVec3NonVectorized<uint8> FByte3;
#define TYPEDEF_DECL_VEC3
}