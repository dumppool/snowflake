/*
* file Vector4.h
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
	class TVec4NonVectorized
	{
	public:
		T X;
		T Y;
		T Z;
		T W;

		TVec4NonVectorized()
			: X(0.f)
			, Y(0.f)
			, Z(0.f)
			, W(0.f)
		{}

		TVec4NonVectorized(T x, T y, T z, T w)
			: X(x), Y(y), Z(z), W(w) {}

		TVec4NonVectorized(const FFloat3& vec, T w)
			: X(vec.X), Y(vec.Y), Z(vec.Z), W(w) {}

		explicit TVec4NonVectorized(const T* p)
		{
			memcpy(&X, p, sizeof(T) * 4);
		}

		FORCEINLINE T operator|(const TVec4NonVectorized<T>& vec) const;
		FORCEINLINE T operator[](int32 index) const;
		FORCEINLINE T& operator[](int32 index);
		FORCEINLINE bool operator==(const TVec4NonVectorized<T>& rhs) const;
		FORCEINLINE TVec4NonVectorized<T>& operator=(const TVec4NonVectorized<T>& rhs);
	};

	template <typename T>
	FORCEINLINE T TVec4NonVectorized<T>::operator|(const TVec4NonVectorized<T>& vec) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z + W * vec.W;
	}

	template <typename T>
	FORCEINLINE T& TVec4NonVectorized<T>::operator[](int32 index)
	{
		return index == 0 ? X : (index == 1 ? Y : (index == 2 ? Z : W));
	}

	template <typename T>
	FORCEINLINE T TVec4NonVectorized<T>::operator[](int32 index) const
	{
		return (*this)[index];
	}

	template <typename T>
	FORCEINLINE bool TVec4NonVectorized<T>::operator==(const TVec4NonVectorized<T>& rhs) const
	{
		return (X == rhs.X
			&& Y == rhs.Y
			&& Z == rhs.Z
			&& W == rhs.W);
	}

	template <typename T>
	FORCEINLINE TVec4NonVectorized<T>&  TVec4NonVectorized<T>::operator=(const TVec4NonVectorized<T>& rhs)
	{
		memcpy(&X, &rhs.X, sizeof(rhs));
		return *this;
	}

	template <typename T>
	FORCEINLINE void from_json(const FJson& j, TVec4NonVectorized<T>& val)
	{
		if (j.is_array() && j.size() >= 4)
		{
			val.X = j[0];
			val.Y = j[1];
			val.Z = j[2];
			val.W = j[3];
		}
	}

	template <typename T>
	FORCEINLINE void to_json(FJson& j, const TVec4NonVectorized<T>& val)
	{
		j[0] = val.X;
		j[1] = val.Y;
		j[2] = val.Z;
		j[3] = val.W;
	}

#ifdef TYPEDEF_DECL_VEC4
#error "TYPEDEF_DECL_VEC4 already defined somewhere else"
#endif

typedef TVec4NonVectorized<float> FFloat4;
typedef TVec4NonVectorized<double> FDouble4;
typedef TVec4NonVectorized<int32> FSInt4;
typedef TVec4NonVectorized<uint8> FByte4;
#define TYPEDEF_DECL_VEC4
}