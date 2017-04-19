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
	__declspec(align(16)) class FVec4NonVectorized
	{
	public:
		float X;
		float Y;
		float Z;
		float W;

		FVec4NonVectorized(float x, float y, float z, float w)
			: X(x), Y(y), Z(z), W(w) {}
	};

#ifdef TYPEDEF_DECL_FVEC4
#error "FVec4 already defined somewhere else"
#else
	typedef FVec4NonVectorized FVec4;
	#define TYPEDEF_DECL_FVEC4
#endif
}