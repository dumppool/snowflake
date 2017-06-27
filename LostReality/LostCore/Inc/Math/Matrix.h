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
	class FMatrixNonVectorized
	{

	public:
		float M[4][4];

		FMatrixNonVectorized()
		{
			SetIdentity();
		}

		INLINE FMatrixNonVectorized& SetZero()
		{
			memset(this, 0, sizeof(*this));
			return *this;
		}

		INLINE FMatrixNonVectorized& SetIdentity()
		{
			M[0][0] = 1.f; M[0][1] = 0.f; M[0][2] = 0.f; M[0][3] = 0.f;
			M[1][0] = 0.f; M[1][1] = 1.f; M[1][2] = 0.f; M[1][3] = 0.f;
			M[2][0] = 0.f; M[2][1] = 0.f; M[2][2] = 1.f; M[2][3] = 0.f;
			M[3][0] = 0.f; M[3][1] = 0.f; M[3][2] = 0.f; M[3][3] = 1.f;
			return *this;
		}

		INLINE FMatrixNonVectorized& SetScale(const float x, const float y, const float z)
		{
			M[0][0] = x;   M[0][1] = 0.f; M[0][2] = 0.f; M[0][3] = 0.f;
			M[1][0] = 0.f; M[1][1] = y;   M[1][2] = 0.f; M[1][3] = 0.f;
			M[2][0] = 0.f; M[2][1] = 0.f; M[2][2] = z;   M[2][3] = 0.f;
			M[3][0] = 0.f; M[3][1] = 0.f; M[3][2] = 0.f; M[3][3] = 1.f;
			return *this;
		}

		INLINE FMatrixNonVectorized& SetScale(const FVec3& scale)
		{
			return SetScale(scale.X, scale.Y, scale.Z);
		}

		INLINE FMatrixNonVectorized& SetTranslate(const float x, const float y, const float z)
		{
			M[0][0] = 1.f; M[0][1] = 0.f; M[0][2] = 0.f; M[0][3] = 0.f;
			M[1][0] = 0.f; M[1][1] = 1.f; M[1][2] = 0.f; M[1][3] = 0.f;
			M[2][0] = 0.f; M[2][1] = 0.f; M[2][2] = 1.f; M[2][3] = 0.f;
			M[3][0] = x;   M[3][1] = y;   M[3][2] = z;   M[3][3] = 1.f;
			return *this;
		}

		INLINE FMatrixNonVectorized& SetTranslate(const FVec3& translate)
		{
			return SetTranslate(translate.X, translate.Y, translate.Z);
		}

		INLINE void SetRotateAndOrigin(const FQuat& q, const FVec3& origin)
		{
			float xx = q.X * q.X * 2.f;
			float xy = q.X * q.Y * 2.f;
			float xz = q.X * q.Z * 2.f;

			float yy = q.Y * q.Y * 2.f;
			float yz = q.Y * q.Z * 2.f;

			float zz = q.Z * q.Z * 2.f;

			float wx = q.W * q.X * 2.f;
			float wy = q.W * q.Y * 2.f;
			float wz = q.W * q.Z * 2.f;

			M[0][0] = 1.f - (yy + zz);
			M[1][0] = xy - wz;
			M[2][0] = xz + wy;

			M[0][1] = xy + wz;
			M[1][1] = 1.f - (xx + zz);
			M[2][1] = yz - wx;

			M[0][2] = xz - wy;
			M[1][2] = yz + wx;
			M[2][2] = 1.f - (xx + yy);

			M[0][3] = M[1][3] = M[2][3] = 0.f;

			M[3][0] = origin.X;
			M[3][1] = origin.Y;
			M[3][2] = origin.Z;
			M[3][3] = 1.f;
		}

		INLINE void SetRotate(const FQuat& q)
		{
			float xx = q.X * q.X * 2.f;
			float xy = q.X * q.Y * 2.f;
			float xz = q.X * q.Z * 2.f;

			float yy = q.Y * q.Y * 2.f;
			float yz = q.Y * q.Z * 2.f;

			float zz = q.Z * q.Z * 2.f;

			float wx = q.W * q.X * 2.f;
			float wy = q.W * q.Y * 2.f;
			float wz = q.W * q.Z * 2.f;

			M[0][0] = 1.f - (yy + zz); 
			M[1][0] = xy - wz; 
			M[2][0] = xz + wy;

			M[0][1] = xy + wz;
			M[1][1] = 1.f - (xx + zz);
			M[2][1] = yz - wx;

			M[0][2] = xz - wy;
			M[1][2] = yz + wx;
			M[2][2] = 1.f - (xx + yy);

			M[0][3] = M[1][3] = M[2][3] = M[3][0] = M[3][1] = M[3][2] = 0.f;
			M[3][3] = 1.f;
		}

		INLINE void SetRotate(const FVec3& euler)
		{
		}

		INLINE void SetRotateInverse(const FVec3& euler)
		{
		}

		INLINE FMatrixNonVectorized& Multiply(const FMatrixNonVectorized& m1, const FMatrixNonVectorized& m2)
		{
			return *this;
		}

		INLINE FMatrixNonVectorized& PreMultiply(const FMatrixNonVectorized& m)
		{
			return *this;
		}

		INLINE FMatrixNonVectorized& PostMultiply(const FMatrixNonVectorized& m)
		{
			return *this;
		}

		INLINE FMatrixNonVectorized& InvertOrthonormal(const FMatrixNonVectorized& m)
		{
			return *this;
		}

		INLINE FMatrixNonVectorized& InvertOrthonormal()
		{
			return *this;
		}

		INLINE FMatrixNonVectorized& Invert34()
		{
			FMatrixNonVectorized m(*this);
			float det = m.GetDeterminant34();
			assert(!IsZero(det) && "the matrix is not invertible 34");
			
			float rcp = 1.f / det;
			M[0][0] = m.M[1][1] * m.M[2][2] - m.M[1][2] * m.M[2][1];
			M[0][1] = m.M[0][2] * m.M[2][1] - m.M[0][1] * m.M[2][2];
			M[0][2] = m.M[0][1] * m.M[1][2] - m.M[0][2] * m.M[1][1];

			M[1][0] = m.M[1][2] * m.M[2][0] - m.M[1][0] * m.M[2][2];
			M[1][1] = m.M[0][0] * m.M[2][2] - m.M[0][2] * m.M[2][0];
			M[1][2] = m.M[0][2] * m.M[1][0] - m.M[0][0] * m.M[1][2];

			M[2][0] = m.M[1][0] * m.M[2][1] - m.M[1][1] * m.M[2][0];
			M[2][1] = m.M[0][1] * m.M[2][0] - m.M[0][0] * m.M[2][1];
			M[2][2] = m.M[0][0] * m.M[1][1] - m.M[0][1] * m.M[1][0];

			M[3][0] = -(m.M[3][0] * M[0][0] + m.M[3][1] * M[1][0] + m.M[3][2] * M[2][0]);
			M[3][1] = -(m.M[3][0] * M[0][1] + m.M[3][1] * M[1][1] + m.M[3][2] * M[2][1]);
			M[3][2] = -(m.M[3][0] * M[0][2] + m.M[3][1] * M[1][2] + m.M[3][2] * M[2][2]);

			for (int32 row = 0; row < 4; ++row)
			{
				for (int32 col = 0; col < 3; ++col)
				{
					M[row][col] *= rcp;
				}
			}

			return *this;
		}

		INLINE float GetDeterminant34() const
		{
			float det = 0.f;
			det += M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]);
			det -= M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]);
			det += M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
			return det;
		}

		INLINE FMatrixNonVectorized& Invert()
		{
			float a0 = M[0][0] * M[1][1] - M[0][1] * M[1][0];
			float a1 = M[0][0] * M[1][2] - M[0][2] * M[1][0];
			float a2 = M[0][0] * M[1][3] - M[0][3] * M[1][0];
			float a3 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
			float a4 = M[0][1] * M[1][3] - M[0][3] * M[1][1];
			float a5 = M[0][2] * M[1][3] - M[0][3] * M[1][2];

			float b0 = M[2][0] * M[3][1] - M[2][1] * M[3][0];
			float b1 = M[2][0] * M[3][2] - M[2][2] * M[3][0];
			float b2 = M[2][0] * M[3][3] - M[2][3] * M[3][0];
			float b3 = M[2][1] * M[3][2] - M[2][2] * M[3][1];
			float b4 = M[2][1] * M[3][3] - M[2][3] * M[3][1];
			float b5 = M[2][2] * M[3][3] - M[2][3] * M[3][2];

			float det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

			assert(!IsZero(det) && "the matrix is not invertible");

			FMatrixNonVectorized m(*this);
			M[0][0] = +m.M[1][1] * b5 - m.M[1][2] * b4 + m.M[1][3] * b3;
			M[1][0] = -m.M[1][0] * b5 + m.M[1][2] * b2 - m.M[1][3] * b1;
			M[2][0] = +m.M[1][0] * b4 - m.M[1][1] * b2 + m.M[1][3] * b0;
			M[3][0] = -m.M[1][0] * b3 + m.M[1][1] * b1 - m.M[1][2] * b0;

			M[0][1] = -m.M[0][1] * b5 + m.M[0][2] * b4 - m.M[0][3] * b3;
			M[1][1] = +m.M[0][0] * b5 - m.M[0][2] * b2 + m.M[0][3] * b1;
			M[2][1] = -m.M[0][0] * b4 + m.M[0][1] * b2 - m.M[0][3] * b0;
			M[3][1] = +m.M[0][0] * b3 - m.M[0][1] * b1 + m.M[0][2] * b0;

			M[0][2] = +m.M[3][1] * a5 - m.M[3][2] * a4 + m.M[3][3] * a3;
			M[1][2] = -m.M[3][0] * a5 + m.M[3][2] * a2 - m.M[3][3] * a1;
			M[2][2] = +m.M[3][0] * a4 - m.M[3][1] * a2 + m.M[3][3] * a0;
			M[3][2] = -m.M[3][0] * a3 + m.M[3][1] * a1 - m.M[3][2] * a0;

			M[0][3] = -m.M[2][1] * a5 + m.M[2][2] * a4 - m.M[2][3] * a3;
			M[1][3] = +m.M[2][0] * a5 - m.M[2][2] * a2 + m.M[2][3] * a1;
			M[2][3] = -m.M[2][0] * a4 + m.M[2][1] * a2 - m.M[2][3] * a0;
			M[3][3] = +m.M[2][0] * a3 - m.M[2][1] * a1 + m.M[2][2] * a0;

			float rcp = 1.f / det;
			for (int32 row = 0; row < 4; ++row)
			{
				for (int32 col = 0; col < 4; ++col)
				{
					M[row][col] *= rcp;
				}
			}

			return *this;
		}

		INLINE float GetDeterminant() const
		{
			float a0 = M[0][0] * M[1][1] - M[0][1] * M[1][0];
			float a1 = M[0][0] * M[1][2] - M[0][2] * M[1][0];
			float a2 = M[0][0] * M[1][3] - M[0][3] * M[1][0];
			float a3 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
			float a4 = M[0][1] * M[1][3] - M[0][3] * M[1][1];
			float a5 = M[0][2] * M[1][3] - M[0][3] * M[1][2];

			float b0 = M[2][0] * M[3][1] - M[2][1] * M[3][0];
			float b1 = M[2][0] * M[3][2] - M[2][2] * M[3][0];
			float b2 = M[2][0] * M[3][3] - M[2][3] * M[3][0];
			float b3 = M[2][1] * M[3][2] - M[2][2] * M[3][1];
			float b4 = M[2][1] * M[3][3] - M[2][3] * M[3][1];
			float b5 = M[2][2] * M[3][3] - M[2][3] * M[3][2];

			return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
		}

		// NOTE: view matrix
		INLINE FMatrixNonVectorized& LookAt(const FVec3& pos, const FVec3& dir, const FVec3& up)
		{
			FVec3 u, d, r;

			u = up.GetNormal();
			d = dir.GetNormal();
			r = u ^ d;
			r.Normalize();
			u = d ^ r;
			u.Normalize();

			M[0][0] = r.X; M[0][1] = u.X; M[0][2] = d.X; M[0][3] = 0.f;
			M[1][0] = r.Y; M[1][1] = u.Y; M[1][2] = d.Y; M[1][3] = 0.f;
			M[2][0] = r.Z; M[2][1] = u.Z; M[2][2] = d.Z; M[2][3] = 0.f;
			M[3][0] = -(pos | r);
			M[3][1] = -(pos | u);
			M[3][2] = -(pos | d);
			M[3][3] = 1.f;

			return *this;
		}

		INLINE void ApplyVector4(FVec4& ret, const FVec4& p)
		{
			ret.X = p | FVec4(M[0][0], M[1][0], M[2][0], M[3][0]);
			ret.Y = p | FVec4(M[0][1], M[1][1], M[2][1], M[3][1]);
			ret.Z = p | FVec4(M[0][2], M[1][2], M[2][2], M[3][2]);
			ret.W = p | FVec4(M[0][3], M[1][3], M[2][3], M[3][3]);
		}

		INLINE void ApplyPoint(FVec3& ret, const FVec3& p)
		{
			FVec4 ret4;
			ApplyVector4(ret4, FVec4(p.X, p.Y, p.Z, 1.f));
			ret.X = ret4.X;
			ret.Y = ret4.Y;
			ret.Z = ret4.Z;
		}

		INLINE FVec3 ApplyPoint(const FVec3& p)
		{
			FVec3 ret;
			ApplyPoint(ret, p);
			return ret;
		}

		INLINE void ApplyVector(FVec3& ret, const FVec3& v)
		{
			FVec4 ret4;
			ApplyVector4(ret4, FVec4(v.X, v.Y, v.Z, 0.f));
			ret.X = ret4.X;
			ret.Y = ret4.Y;
			ret.Z = ret4.Z;
		}

		INLINE FVec3 ApplyVector(const FVec3& v)
		{
			FVec3 ret;
			ApplyVector(ret, v);
			return ret;
		}

		INLINE FMatrixNonVectorized& PreRotateX(const float angle)
		{

		}

		INLINE FMatrixNonVectorized& PreRotateY(const float angle)
		{

		}

		INLINE FMatrixNonVectorized& PreRotateZ(const float angle)
		{

		}

		INLINE FMatrixNonVectorized& PostRotateX(const float angle)
		{
		}

		INLINE FMatrixNonVectorized& PostRotateY(const float angle)
		{
		}

		INLINE FMatrixNonVectorized& PostRotateZ(const float angle)
		{
		}

		INLINE FMatrixNonVectorized& OrthogonalProjection(float w, float h, float zn, float zf)
		{

		}

		INLINE FMatrixNonVectorized& PerspectiveProjection(float fov, float aspectRatio, float np, float fp)
		{

		}

		INLINE float GetYaw() const
		{

		}

		INLINE float GetPitch() const
		{

		}

		INLINE float GetRoll() const
		{

		}

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

		INLINE FMatrixNonVectorized operator*(const FMatrixNonVectorized& rhs) const
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

		INLINE FMatrixNonVectorized& Transpose()
		{
			FMatrixNonVectorized m(*this);
			for (int32 i = 0; i < 4; ++i)
			{
				for (int32 j = 0; j < 4; ++j)
				{
					M[i][j] = m.M[j][i];
				}
			}

			return *this;
		}
	};

	INLINE void from_json(const FJson& j, FMatrixNonVectorized& matrix)
	{
		if (j.is_array() && j.size() >= 16)
		{
			for (int row = 0; row < 4; ++row)
			{
				for (int col = 0; col < 4; ++col)
				{
					matrix.M[row][col] = j[row * 4 + col];
				}
			}
		}
	}

	INLINE void to_json(FJson& j, const FMatrixNonVectorized& matrix)
	{
		j.clear();
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				j.push_back(matrix.M[row][col]);
			}
		}
	}

#ifdef TYPEDEF_DECL_FMATRIX
#error "FMatrix already defined somewhere else"
#else
	typedef FMatrixNonVectorized FMatrix;
#define TYPEDEF_DECL_FMATRIX
#endif

}