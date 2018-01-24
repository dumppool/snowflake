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
#include "Vector4.h"
#include "Quat.h"

namespace LostCore
{
	template <typename T>
	class TMatrixNonVectorized
	{

	public:
		typedef T FT;
		//TVec4NonVectorized<T> M[4];
		T M[4][4];

		TMatrixNonVectorized()
		{
			SetIdentity();
		}

		void SetRow(int32 row, const FFloat4& vec)
		{
			memcpy((T*)M + 4 * row, &vec, sizeof(vec));
		}

		FFloat4 GetRow(int32 row) const
		{
			FFloat4 vec;
			memcpy(&vec, (T*)M + 4 * row, sizeof(vec));
			return vec;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  SetZero()
		{
			memset(this, 0, sizeof(*this));
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  SetIdentity()
		{
			M[0][0] = 1.0; M[0][1] = 0.0; M[0][2] = 0.0; M[0][3] = 0.0;
			M[1][0] = 0.0; M[1][1] = 1.0; M[1][2] = 0.0; M[1][3] = 0.0;
			M[2][0] = 0.0; M[2][1] = 0.0; M[2][2] = 1.0; M[2][3] = 0.0;
			M[3][0] = 0.0; M[3][1] = 0.0; M[3][2] = 0.0; M[3][3] = 1.0;
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  SetScale(const T x, const T y, const T z)
		{
			M[0][0] = x;   M[0][1] = 0.0; M[0][2] = 0.0; M[0][3] = 0.0;
			M[1][0] = 0.0; M[1][1] = y;   M[1][2] = 0.0; M[1][3] = 0.0;
			M[2][0] = 0.0; M[2][1] = 0.0; M[2][2] = z;   M[2][3] = 0.0;
			M[3][0] = 0.0; M[3][1] = 0.0; M[3][2] = 0.0; M[3][3] = 1.0;
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  SetScale(const TVec3NonVectorized<T>& scale)
		{
			return SetScale(scale.X, scale.Y, scale.Z);
		}

		FORCEINLINE TMatrixNonVectorized<T>&  SetTranslate(const T x, const T y, const T z)
		{
			M[0][0] = 1.0; M[0][1] = 0.0; M[0][2] = 0.0; M[0][3] = 0.0;
			M[1][0] = 0.0; M[1][1] = 1.0; M[1][2] = 0.0; M[1][3] = 0.0;
			M[2][0] = 0.0; M[2][1] = 0.0; M[2][2] = 1.0; M[2][3] = 0.0;
			M[3][0] = x;   M[3][1] = y;   M[3][2] = z;   M[3][3] = 1.0;
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  SetTranslate(const TVec3NonVectorized<T>& translate)
		{
			return SetTranslate(translate.X, translate.Y, translate.Z);
		}

		FORCEINLINE TMatrixNonVectorized<T>&  AddTranslate(const TVec3NonVectorized<T>& delta)
		{
			return SetTranslate(M[3][0] + delta.X, M[3][1] + delta.Y, M[3][2] + delta.Z);
		}

		FORCEINLINE TMatrixNonVectorized<T>& SetRotateAndOrigin(const FQuat& q, const TVec3NonVectorized<T>& origin, const TVec3NonVectorized<T>& scale = TVec3NonVectorized<T>(1.0, 1.0, 1.0))
		{
			SetRotate(q);

			M[0][3] = M[1][3] = M[2][3] = 0.0;

			M[3][0] = origin.X;
			M[3][1] = origin.Y;
			M[3][2] = origin.Z;
			M[3][3] = 1.0;
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>& SetRotate(const FQuat& q)
		{
			T one = static_cast<T>(1.0);
			T two = static_cast<T>(2.0);
			T xx = q.X * q.X * two;
			T xy = q.X * q.Y * two;
			T xz = q.X * q.Z * two;

			T yy = q.Y * q.Y * two;
			T yz = q.Y * q.Z * two;

			T zz = q.Z * q.Z * two;

			T wx = q.W * q.X * two;
			T wy = q.W * q.Y * two;
			T wz = q.W * q.Z * two;

			M[0][0] = (one - (yy + zz));
			M[0][1] = (xy + wz);
			M[0][2] = (xz - wy);

			M[1][0] = (xy - wz);
			M[1][1] = (one - (xx + zz));
			M[1][2] = (yz + wx);

			M[2][0] = (xz + wy);
			M[2][1] = (yz - wx);
			M[2][2] = (one - (xx + yy));

			M[0][3] = M[1][3] = M[2][3] = M[3][0] = M[3][1] = M[3][2] = static_cast<T>(0.0);
			M[3][3] = one;
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>& SetRotate(const TVec3NonVectorized<T>& euler)
		{
			return SetRotate(FQuat().FromEuler(euler));
		}

		FORCEINLINE void SetRotateInverse(const TVec3NonVectorized<T>& euler)
		{
		}

		FORCEINLINE TMatrixNonVectorized<T>&  Multiply(const TMatrixNonVectorized<T>&  m1, const TMatrixNonVectorized<T>&  m2)
		{
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  PreMultiply(const TMatrixNonVectorized<T>&  m)
		{
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  PostMultiply(const TMatrixNonVectorized<T>&  m)
		{
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  InvertOrthonormal(const TMatrixNonVectorized<T>&  m)
		{
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  InvertOrthonormal()
		{
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  Invert34()
		{
			TMatrixNonVectorized<T> m(*this);
			T det = m.GetDeterminant34();
			assert(!IsZero(det) && "the matrix is not invertible 34");
			
			T rcp = 1.0 / det;
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

		FORCEINLINE T GetDeterminant34() const
		{
			T det = 0.0;
			det += M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]);
			det -= M[0][1] * (M[1][0] * M[2][2] - M[1][2] * M[2][0]);
			det += M[0][2] * (M[1][0] * M[2][1] - M[1][1] * M[2][0]);
			return det;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  Invert()
		{
			T a0 = M[0][0] * M[1][1] - M[0][1] * M[1][0];
			T a1 = M[0][0] * M[1][2] - M[0][2] * M[1][0];
			T a2 = M[0][0] * M[1][3] - M[0][3] * M[1][0];
			T a3 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
			T a4 = M[0][1] * M[1][3] - M[0][3] * M[1][1];
			T a5 = M[0][2] * M[1][3] - M[0][3] * M[1][2];

			T b0 = M[2][0] * M[3][1] - M[2][1] * M[3][0];
			T b1 = M[2][0] * M[3][2] - M[2][2] * M[3][0];
			T b2 = M[2][0] * M[3][3] - M[2][3] * M[3][0];
			T b3 = M[2][1] * M[3][2] - M[2][2] * M[3][1];
			T b4 = M[2][1] * M[3][3] - M[2][3] * M[3][1];
			T b5 = M[2][2] * M[3][3] - M[2][3] * M[3][2];

			T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;

			assert(!IsZero(det) && "the matrix is not invertible");

			TMatrixNonVectorized<T> m(*this);
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

			T rcp = static_cast<T>(1.0) / det;
			for (int32 row = 0; row < 4; ++row)
			{
				for (int32 col = 0; col < 4; ++col)
				{
					M[row][col] *= rcp;
				}
			}

			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>  GetInvert() const
		{
			TMatrixNonVectorized<T> result(*this);
			return result.Invert();
		}

		FORCEINLINE T GetDeterminant() const
		{
			T a0 = M[0][0] * M[1][1] - M[0][1] * M[1][0];
			T a1 = M[0][0] * M[1][2] - M[0][2] * M[1][0];
			T a2 = M[0][0] * M[1][3] - M[0][3] * M[1][0];
			T a3 = M[0][1] * M[1][2] - M[0][2] * M[1][1];
			T a4 = M[0][1] * M[1][3] - M[0][3] * M[1][1];
			T a5 = M[0][2] * M[1][3] - M[0][3] * M[1][2];

			T b0 = M[2][0] * M[3][1] - M[2][1] * M[3][0];
			T b1 = M[2][0] * M[3][2] - M[2][2] * M[3][0];
			T b2 = M[2][0] * M[3][3] - M[2][3] * M[3][0];
			T b3 = M[2][1] * M[3][2] - M[2][2] * M[3][1];
			T b4 = M[2][1] * M[3][3] - M[2][3] * M[3][1];
			T b5 = M[2][2] * M[3][3] - M[2][3] * M[3][2];

			return a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
		}

		// NOTE: view matrix
		FORCEINLINE TMatrixNonVectorized<T>&  LookAt(const TVec3NonVectorized<T>& pos, const TVec3NonVectorized<T>& dir, const TVec3NonVectorized<T>& up)
		{
			TVec3NonVectorized<T> u, d, r;

			u = up.GetNormal();
			d = dir.GetNormal();
			r = u.Cross(d);
			r.Normalize();
			u = d.Cross(r);
			u.Normalize();

			M[0][0] = r.X; M[0][1] = u.X; M[0][2] = d.X; M[0][3] = 0.0;
			M[1][0] = r.Y; M[1][1] = u.Y; M[1][2] = d.Y; M[1][3] = 0.0;
			M[2][0] = r.Z; M[2][1] = u.Z; M[2][2] = d.Z; M[2][3] = 0.0;
			M[3][0] = -(pos | r);
			M[3][1] = -(pos | u);
			M[3][2] = -(pos | d);
			M[3][3] = 1.0;

			return *this;
		}

		FORCEINLINE void ApplyVector4(TVec4NonVectorized<T>& ret, const TVec4NonVectorized<T>& p) const
		{
			ret.X = p | TVec4NonVectorized<T>(M[0][0], M[1][0], M[2][0], M[3][0]);
			ret.Y = p | TVec4NonVectorized<T>(M[0][1], M[1][1], M[2][1], M[3][1]);
			ret.Z = p | TVec4NonVectorized<T>(M[0][2], M[1][2], M[2][2], M[3][2]);
			ret.W = p | TVec4NonVectorized<T>(M[0][3], M[1][3], M[2][3], M[3][3]);
		}

		FORCEINLINE void ApplyPoint(TVec3NonVectorized<T>& ret, const TVec3NonVectorized<T>& p) const
		{
			TVec4NonVectorized<T> ret4;
			ApplyVector4(ret4, TVec4NonVectorized<T>(p.X, p.Y, p.Z, 1.0));
			ret.X = ret4.X;
			ret.Y = ret4.Y;
			ret.Z = ret4.Z;
		}

		FORCEINLINE TVec3NonVectorized<T> ApplyPoint(const TVec3NonVectorized<T>& p) const
		{
			TVec3NonVectorized<T> ret;
			ApplyPoint(ret, p);
			return ret;
		}

		FORCEINLINE void ApplyVector(TVec3NonVectorized<T>& ret, const TVec3NonVectorized<T>& v) const
		{
			TVec4NonVectorized<T> ret4;
			ApplyVector4(ret4, TVec4NonVectorized<T>(v.X, v.Y, v.Z, 0.0));
			ret.X = ret4.X;
			ret.Y = ret4.Y;
			ret.Z = ret4.Z;
		}

		FORCEINLINE TVec3NonVectorized<T> ApplyVector(const TVec3NonVectorized<T>& v) const
		{
			TVec3NonVectorized<T> ret;
			ApplyVector(ret, v);
			return ret;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  PreRotateX(const T angle)
		{

		}

		FORCEINLINE TMatrixNonVectorized<T>&  PreRotateY(const T angle)
		{

		}

		FORCEINLINE TMatrixNonVectorized<T>&  PreRotateZ(const T angle)
		{

		}

		FORCEINLINE TMatrixNonVectorized<T>&  PostRotateX(const T angle)
		{
		}

		FORCEINLINE TMatrixNonVectorized<T>&  PostRotateY(const T angle)
		{
		}

		FORCEINLINE TMatrixNonVectorized<T>&  PostRotateZ(const T angle)
		{
		}

		FORCEINLINE TMatrixNonVectorized<T>&  OrthogonalProjection(T w, T h, T zn, T zf)
		{

		}

		FORCEINLINE TMatrixNonVectorized<T>&  PerspectiveProjection(T fov, T aspectRatio, T np, T fp)
		{

		}

		FORCEINLINE T GetYaw() const
		{

		}

		FORCEINLINE T GetPitch() const
		{

		}

		FORCEINLINE T GetRoll() const
		{

		}

		FORCEINLINE TVec3NonVectorized<T> GetScale() const
		{
			TVec3NonVectorized<T> scale(TVec3NonVectorized<T>::GetZero());

			const T squared0 = M[0][0] * M[0][0] + M[0][1] * M[0][1] + M[0][2] * M[0][2];
			const T squared1 = M[1][0] * M[1][0] + M[1][1] * M[1][1] + M[1][2] * M[1][2];
			const T squared2 = M[2][0] * M[2][0] + M[2][1] * M[2][1] + M[2][2] * M[2][2];

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

		FORCEINLINE FQuat GetOrientation() const
		{
			FQuat quat;
			const T tr = M[0][0] + M[1][1] + M[2][2];
			if (tr > 0.0)
			{
				T invsqrt = LostCore::InvSqrt(tr + 1.0);
				T s = 0.5f * invsqrt;
				quat.X = (M[1][2] - M[2][1]) * s;
				quat.Y = (M[2][0] - M[0][2]) * s;
				quat.Z = (M[0][1] - M[1][0]) * s;
				quat.W = 0.5f * (1.0 / invsqrt);
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
				T s = M[i][i] - M[j][j] - M[k][k] + 1.0;
				T invsqrt = LostCore::InvSqrt(s);
				T qt[4];
				qt[i] = 0.5f*(1.0 / invsqrt);
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

		FORCEINLINE TVec3NonVectorized<T> GetOrigin() const
		{
			return TVec3NonVectorized<T>(M[3][0], M[3][1], M[3][2]);
		}

		FORCEINLINE TMatrixNonVectorized<T>& Scale(const TVec3NonVectorized<T>& val)
		{
			M[0][0] *= val.X; M[0][1] *= val.X; M[0][2] *= val.X;
			M[1][0] *= val.Y; M[1][1] *= val.Y; M[1][2] *= val.Y;
			M[2][0] *= val.Z; M[2][1] *= val.Z; M[2][2] *= val.Z;
			return *this;
		}

		FORCEINLINE bool operator==(const TMatrixNonVectorized<T>& rhs) const
		{
			return (TVec4NonVectorized<T>(M[0]) == TVec4NonVectorized<T>(rhs.M[0])
				&& TVec4NonVectorized<T>(M[1]) == TVec4NonVectorized<T>(rhs.M[1])
				&& TVec4NonVectorized<T>(M[2]) == TVec4NonVectorized<T>(rhs.M[2])
				&& TVec4NonVectorized<T>(M[3]) == TVec4NonVectorized<T>(rhs.M[3]));
		}

		FORCEINLINE TMatrixNonVectorized<T>& operator=(const TMatrixNonVectorized<T>& rhs)
		{
			memcpy(M, rhs.M, sizeof(T) * 4 * 4);
			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T> operator-() const
		{
			TMatrixNonVectorized<T> result;

			result.M[0][0] = -M[0][0];
			result.M[0][1] = -M[0][1];
			result.M[0][2] = -M[0][2];
			result.M[0][3] = -M[0][3];

			result.M[1][0] = -M[1][0];
			result.M[1][1] = -M[1][1];
			result.M[1][2] = -M[1][2];
			result.M[1][3] = -M[1][3];

			result.M[2][0] = -M[2][0];
			result.M[2][1] = -M[2][1];
			result.M[2][2] = -M[2][2];
			result.M[2][3] = -M[2][3];

			result.M[3][0] = -M[3][0];
			result.M[3][1] = -M[3][1];
			result.M[3][2] = -M[3][2];
			result.M[3][3] = -M[3][3];

			return result;
		}

		FORCEINLINE TMatrixNonVectorized<T> operator+(const TMatrixNonVectorized<T>& rhs) const
		{
			TMatrixNonVectorized<T> result(*this);

			result.M[0][0] += rhs.M[0][0];
			result.M[0][1] += rhs.M[0][1];
			result.M[0][2] += rhs.M[0][2];
			result.M[0][3] += rhs.M[0][3];

			result.M[1][0] += rhs.M[1][0];
			result.M[1][1] += rhs.M[1][1];
			result.M[1][2] += rhs.M[1][2];
			result.M[1][3] += rhs.M[1][3];

			result.M[2][0] += rhs.M[2][0];
			result.M[2][1] += rhs.M[2][1];
			result.M[2][2] += rhs.M[2][2];
			result.M[2][3] += rhs.M[2][3];

			result.M[3][0] += rhs.M[3][0];
			result.M[3][1] += rhs.M[3][1];
			result.M[3][2] += rhs.M[3][2];
			result.M[3][3] += rhs.M[3][3];

			return result;
		}

		template <typename T2>
		FORCEINLINE TMatrixNonVectorized<T> operator+(const T2& rhs) const
		{
			TMatrixNonVectorized<T> result(*this);

			result.M[0][0] += rhs.M[0][0];
			result.M[0][1] += rhs.M[0][1];
			result.M[0][2] += rhs.M[0][2];
			result.M[0][3] += rhs.M[0][3];

			result.M[1][0] += rhs.M[1][0];
			result.M[1][1] += rhs.M[1][1];
			result.M[1][2] += rhs.M[1][2];
			result.M[1][3] += rhs.M[1][3];

			result.M[2][0] += rhs.M[2][0];
			result.M[2][1] += rhs.M[2][1];
			result.M[2][2] += rhs.M[2][2];
			result.M[2][3] += rhs.M[2][3];

			result.M[3][0] += rhs.M[3][0];
			result.M[3][1] += rhs.M[3][1];
			result.M[3][2] += rhs.M[3][2];
			result.M[3][3] += rhs.M[3][3];
			return TMatrixNonVectorized<T>;
		}

		template <typename T2>
		FORCEINLINE TMatrixNonVectorized<T> operator-(const T2& rhs) const
		{
			return *this + (-rhs);
		}

		template <typename T2>
		FORCEINLINE TMatrixNonVectorized<T> operator*(const T2& rhs) const
		{
			TMatrixNonVectorized<T> result(*this);

			result.M[0][0] *= rhs;
			result.M[0][1] *= rhs;
			result.M[0][2] *= rhs;
			result.M[0][3] *= rhs;

			result.M[1][0] *= rhs;
			result.M[1][1] *= rhs;
			result.M[1][2] *= rhs;
			result.M[1][3] *= rhs;

			result.M[2][0] *= rhs;
			result.M[2][1] *= rhs;
			result.M[2][2] *= rhs;
			result.M[2][3] *= rhs;

			result.M[3][0] *= rhs;
			result.M[3][1] *= rhs;
			result.M[3][2] *= rhs;
			result.M[3][3] *= rhs;

			return result;
		}

		FORCEINLINE TMatrixNonVectorized<T> operator*(const TMatrixNonVectorized<T>& rhs) const
		{
			TMatrixNonVectorized<T> result(*this);

			for (int32 i = 0; i < 4; ++i)
			{
				for (int32 j = 0; j < 4; ++j)
				{
					result.M[i][j] = M[i][0] * rhs.M[0][j] + M[i][1] * rhs.M[1][j] + M[i][2] * rhs.M[2][j] + M[i][3] * rhs.M[3][j];
				}
			}

			return result;
		}

		FORCEINLINE TMatrixNonVectorized<T>&  Transpose()
		{
			TMatrixNonVectorized<T> m(*this);
			for (int32 i = 0; i < 4; ++i)
			{
				for (int32 j = 0; j < 4; ++j)
				{
					M[i][j] = m.M[j][i];
				}
			}

			return *this;
		}

		FORCEINLINE TMatrixNonVectorized<T>& Normalize3x3()
		{
			for (uint32 i = 0; i < 3; ++i)
			{
				TVec3NonVectorized<T> v;
				memcpy(&v, &M[i][0], sizeof(v));
				v.Normalize();
				memcpy(&M[i][0], &(v), sizeof(v));
			}

			return *this;
		}

		FORCEINLINE TVec3NonVectorized<T> GetRightVector() const
		{
			return TVec3NonVectorized<T>(M[0][0], M[0][1], M[0][2]);
		}

		FORCEINLINE TVec3NonVectorized<T> GetUpVector() const
		{
			return TVec3NonVectorized<T>(M[1][0], M[1][1], M[1][2]);
		}

		FORCEINLINE TVec3NonVectorized<T> GetForwardVector() const
		{
			return TVec3NonVectorized<T>(M[2][0], M[2][1], M[2][2]);
		}
	};

	template <typename T>
	FORCEINLINE void from_json(const FJson& j, TMatrixNonVectorized<T>&  matrix)
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

	template <typename T>
	FORCEINLINE void to_json(FJson& j, const TMatrixNonVectorized<T>&  matrix)
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

#ifdef TYPEDEF_DECL_MATRIX
#error "TYPEDEF_DECL_MATRIX already defined somewhere else"
#endif

typedef TMatrixNonVectorized<float> FFloat4x4;
typedef TMatrixNonVectorized<double> FDouble4x4;

#define TYPEDEF_DECL_MATRIX

}