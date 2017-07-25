/*
* file Transform2.h
*
* author luoxw
* date 2017/06/21
*
*
*/

#pragma once

#include "Vector3.h"
#include "Quat.h"
#include "Matrix.h"

namespace LostCore
{
	class FTransformNonVectorized
	{
	protected:
		FQuat Orientation;
		FFloat3 Translation;
		FFloat3 Scale;

	public:
		INLINE FTransformNonVectorized()
			: Orientation(FQuat::GetIdentity())
			, Translation(FFloat3::GetZero())
			, Scale(1.f, 1.f, 1.f)
		{
		}

		INLINE explicit FTransformNonVectorized(const FFloat3& translation)
			: Orientation(FQuat::GetIdentity())
			, Translation(translation)
			, Scale(1.f, 1.f, 1.f)
		{
		}

		INLINE explicit FTransformNonVectorized(const FQuat& orientation)
			: Orientation(orientation)
			, Translation(FFloat3::GetZero())
			, Scale(1.f, 1.f, 1.f)
		{
		}

		INLINE FTransformNonVectorized(const FQuat& orientation, const FFloat3& translation, const FFloat3& scale = FFloat3(1.f, 1.f, 1.f))
			: Orientation(orientation)
			, Translation(translation)
			, Scale(scale)
		{
		}

		INLINE FTransformNonVectorized GetInversed() const
		{
			FQuat orientation = Orientation.GetInversed();
			FFloat3 scale = FFloat3::GetReciprocal(Scale);
			FFloat3 translation(orientation.RotateVector(scale * -Translation));
			return FTransformNonVectorized(orientation, translation, scale);
		}

		INLINE FTransformNonVectorized operator*(float value) const
		{
			return FTransformNonVectorized(Orientation*value, Translation*value, Scale*value);
		}

		INLINE FTransformNonVectorized& operator*=(float value)
		{
			Orientation *= value;
			Translation *= value;
			Scale *= value;
			return *this;
		}

		INLINE FTransformNonVectorized operator*(const FTransformNonVectorized& transform) const
		{
			return FTransformNonVectorized(
				transform.Orientation * Orientation,
				transform.Scale * Scale,
				transform.Orientation.RotateVector(transform.Scale * Translation) + transform.Translation
			);
		}

		INLINE FTransformNonVectorized& operator*=(const FTransformNonVectorized& transform)
		{
			*this = (*this) * transform;
			return *this;
		}

		INLINE FFloat3 TranformVector(const FFloat3& vec) const
		{
			return (Orientation.RotateVector(vec*Scale));
		}

		INLINE FFloat3 TransformPosition(const FFloat3& vec) const
		{
			return (Orientation.RotateVector(vec*Scale) + Translation);
		}

		INLINE FFloat4x4 ToMatrix(bool withScale = true) const
		{
			FFloat4x4 matrix;
			FFloat3 scale(withScale ? Scale : FFloat3(1.f, 1.f, 1.f));

			matrix.M[3][0] = Translation.X;
			matrix.M[3][1] = Translation.Y;
			matrix.M[3][2] = Translation.Z;

			const float x2 = Orientation.X + Orientation.X;
			const float y2 = Orientation.Y + Orientation.Y;
			const float z2 = Orientation.Z + Orientation.Z;
			{
				const float xx2 = Orientation.X * x2;
				const float yy2 = Orientation.Y * y2;
				const float zz2 = Orientation.Z * z2;
				matrix.M[0][0] = (1.f - (yy2 + zz2)) * scale.X;
				matrix.M[1][1] = (1.f - (xx2 + zz2)) * scale.Y;
				matrix.M[2][2] = (1.f - (xx2 + yy2)) * scale.Z;
			}
			{
				const float yz2 = Orientation.Y * z2;
				const float wx2 = Orientation.W * x2;
				matrix.M[2][1] = (yz2 + wx2) * scale.Z;
				matrix.M[1][2] = (yz2 - wx2) * scale.Y;
			}
			{
				const float xy2 = Orientation.X * y2;
				const float wz2 = Orientation.W * z2;
				matrix.M[1][0] = (xy2 + wz2) * scale.Y;
				matrix.M[0][1] = (xy2 - wz2) * scale.X;
			}
			{
				const float xz2 = Orientation.X * z2;
				const float wy2 = Orientation.W * y2;
				matrix.M[2][0] = (xz2 - wy2) * scale.Z;
				matrix.M[0][2] = (xz2 + wy2) * scale.X;
			}

			matrix.M[0][3] = 0.f;
			matrix.M[1][3] = 0.f;
			matrix.M[2][3] = 0.f;
			matrix.M[3][3] = 1.f;

			return matrix;
		}

		INLINE FFloat4x4 ToMatrixNoScale() const
		{
			return ToMatrix(false);
		}

		INLINE FTransformNonVectorized& FromMatrix(const FFloat4x4& matrix)
		{
			Scale = matrix.GetScale();
			Orientation = matrix.GetOrientation();
			Translation = matrix.GetOrigin();
			return *this;
		}

	public:
		INLINE static FTransformNonVectorized GetIdentity()
		{
			return FTransformNonVectorized();
		}
	};

#ifdef TYPEDEF_DECL_FTRANSFORM
#error "FTransform already defined somewhere else"
#else
	typedef FTransformNonVectorized FTransform;
	#define TYPEDEF_DECL_FTRANSFORM
#endif
}