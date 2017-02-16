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
	};

#ifdef TYPEDEF_DECL_FVEC4
#error "FVec4 already defined somewhere else"
#else
	typedef FVec4NonVectorized FVec4;
	#define TYPEDEF_DECL_FVEC4
#endif
}