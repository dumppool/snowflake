/*
* file Matrix.h
*
* author luoxw
* date 2017/02/08
*
*
*/

#pragma once

#include "MathBase.h"
#include "Vector3.h"
#include "Quat.h"

namespace LostCore
{
	ALIGNED_LR(16) class FMatrixNonVectorized
	{
	public:
		__declspec(align(16)) float M[4][4];

		INLINE FVec3 GetScale() const
		{
			FVec3 scale(FVec3::GetZero());

			const float squared0 = M[0][0] * M[0][0] + M[0][1] * M[0][1] + M[0][2] * M[0][2];
			const float squared1 = M[1][0] * M[1][0] + M[1][1] * M[1][1] + M[1][2] * M[1][2];
			const float squared2 = M[2][0] * M[2][0] + M[2][1] * M[2][1] + M[2][2] * M[2][2];

			if (squared0 > SSmallFloat)
			{
				scale.X = LostCore::Sqrt(squared0);
			}

			if (squared1 > SSmallFloat)
			{
				scale.Y = LostCore::Sqrt(squared1);
			}

			if (squared2 > SSmallFloat)
			{
				scale.Z = LostCore::Sqrt(squared2);
			}

			return scale;
		}

		INLINE FQuat GetOrientation() const
		{
			FQuat quat;
			const float tr = M[0][0] + M[1][1] + M[2][2];
			if (tr > 0.f)
			{
				float invsqrt = LostCore::InvSqrt(tr + 1.f);
				float s = 0.5f * invsqrt;
				quat.X = (M[1][2] - M[2][1]) * s;
				quat.Y = (M[2][0] - M[0][2]) * s;
				quat.Z = (M[0][1] - M[1][0]) * s;
				quat.W = 0.5f * (1.f / invsqrt);
			}
			else
			{
				int32 i = 0;
				if (M[1][1] > M[0][0])
				{
					i = 1;
				}

				if (M[2][2] > M[i][i])
				{
					i = 2;
				}

				static const int32 nxt[3] = { 1,2,0 };
				const int32 j = nxt[i];
				const int32 k = nxt[j];
				float s = M[i][i] - M[j][j] - M[k][k] + 1.f;
				float invsqrt = LostCore::InvSqrt(s);
				float qt[4];
				qt[i] = 0.5f*(1.f / invsqrt);
				s = 0.5f * invsqrt;
				qt[3] = (M[j][k] - M[k][j]) * s;
				qt[j] = (M[i][j] + M[j][i]) * s;
				qt[k] = (M[i][k] + M[k][i]) * s;

				quat.X = qt[0];
				quat.Y = qt[1];
				quat.Z = qt[2];
				quat.W = qt[3];
			}

			return quat;
		}

		INLINE FVec3 GetOrigin() const
		{
			return FVec3(M[3][0], M[3][1], M[3][2]);
		}

		INLINE FMatrixNonVectorized operator*(const FMatrixNonVectorized& rhs)
		{
			FMatrixNonVectorized result;
			for (int32 i = 0; i < 4; ++i)
			{
				for (int32 j = 0; j < 4; ++j)
				{
					result.M[i][j] = M[i][0] * rhs.M[0][j] + M[i][1] * rhs.M[1][j] + M[i][2] * rhs.M[2][j] + M[i][3] * rhs.M[3][j];
				}
			}

			return result;
		}

		INLINE FMatrixNonVectorized GetTranspose() const
		{
			FMatrixNonVectorized ret;
			for (int32 i = 0; i < 4; ++i)
			{
				for (int32 j = 0; j < 4; ++j)
				{
					ret.M[i][j] = this->M[j][i];
				}
			}

			return ret;
		}
	};

#ifdef TYPEDEF_DECL_FMATRIX
#error "FMatrix already defined somewhere else"
#else
	typedef FMatrixNonVectorized FMatrix;
#define TYPEDEF_DECL_FMATRIX
#endif

}