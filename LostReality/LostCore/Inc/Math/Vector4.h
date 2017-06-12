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
	class FVec4NonVectorized
	{
	public:
		float X;
		float Y;
		float Z;
		float W;

		FVec4NonVectorized()
			: X(0.f)
			, Y(0.f)
			, Z(0.f)
			, W(0.f)
		{}

		FVec4NonVectorized(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w) {}
	};

	INLINE void from_json(const FJson& j, FVec4NonVectorized& val)
	{
		if (j.is_array() && j.size() >= 4)
		{
			val.X = j[0];
			val.Y = j[1];
			val.Z = j[2];
			val.W = j[3];
		}
	}

	INLINE void to_json(FJson& j, const FVec4NonVectorized& val)
	{
		j[0] = val.X;
		j[1] = val.Y;
		j[2] = val.Z;
		j[3] = val.W;
	}

#ifdef TYPEDEF_DECL_FVEC4
#error "FVec4 already defined somewhere else"
#else
	typedef FVec4NonVectorized FVec4;
	#define TYPEDEF_DECL_FVEC4
#endif
}