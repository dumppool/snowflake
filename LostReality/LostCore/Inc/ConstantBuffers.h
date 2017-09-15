/*
* file ConstantBuffers.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

namespace LostCore
{
	struct FGlobalParameter
	{
		float ScreenWidth;
		float ScreenHeight;
		float ScreenWidthRcp;
		float ScreenHeightRcp;
		LostCore::FFloat4x4 ViewProject;

		FGlobalParameter GetBuffer() const
		{
			return *this;
		}
	};

	struct FSingleMatrixParameter
	{
		FFloat4x4 Matrix;

		FSingleMatrixParameter GetBuffer() const
		{
			FSingleMatrixParameter result(*this);
			result.Matrix.Transpose();
			return result;
		}
	};

	struct FSkinnedParameter
	{
		FFloat4x4 World;
		array<FFloat4x4, MAX_BONES_PER_BATCH> Bones;

		FSkinnedParameter GetBuffer() const
		{
			FSkinnedParameter result(*this);
			result.World.Transpose();
			for_each(result.Bones.begin(), result.Bones.end(), [](LostCore::FFloat4x4& mat) {mat.Transpose(); });
			return result;
		}
	};

	struct FRectParameter
	{
		// ��С�ߴ磬����
		FFloat2 Size;

		// ��Ը�����λ�ã���λ������
		FFloat2 Origin;

		// ��Ը��������ţ�(0.f, 1.f]
		float Scale;

		FFloat3 Unused;

		FRectParameter()
			: Size(0.f, 0.f)
			, Origin(0.f, 0.f)
			, Scale(1.f) 
		{
		}

		FRectParameter GetBuffer() const
		{
			return *this;
		}
	};
}

