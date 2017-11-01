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
		FColor128 AmbientColor;
		FColor128 DirectionalLitColor;
		FFloat3 DirectionalLitDir;
		FColor128 PointLitColor;
		FFloat3 PointLitPosition;

		void GetBuffer(FBuf& buf) const
		{
			FGlobalParameter result(*this);
			result.ViewProject.Transpose();
			buf.resize(GetAlignedSize(sizeof(result), 16));
			memcpy(buf.data(), &result, buf.size());
		}
	};

	struct FSingleMatrixParameter
	{
		FFloat4x4 Matrix;

		void GetBuffer(FBuf& buf) const
		{
			FSingleMatrixParameter result(*this);
			result.Matrix.Transpose();
			buf.resize(GetAlignedSize(sizeof(result), 16));
			memcpy(buf.data(), &result, buf.size());
		}
	};

	struct FSkinnedParameter
	{
		FFloat4x4 World;
		array<FFloat4x4, MAX_BONES_PER_BATCH> Bones;

		void GetBuffer(FBuf& buf) const
		{
			FSkinnedParameter result(*this);
			result.World.Transpose();
			for_each(result.Bones.begin(), result.Bones.end(), [](LostCore::FFloat4x4& mat) {mat.Transpose(); });
			buf.resize(GetAlignedSize(sizeof(result), 16));
			memcpy(buf.data(), &result, buf.size());
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

		FRectParameter(const FFloat2& size, const FFloat2 origin, float scale)
			: Size(size)
			, Origin(origin)
			, Scale(scale)
		{
		}

		FRectParameter()
			: Size(0.f, 0.f)
			, Origin(0.f, 0.f)
			, Scale(1.f) 
		{
		}

		void GetBuffer(FBuf& buf) const
		{
			buf.resize(GetAlignedSize(sizeof(FRectParameter), 16));
			memcpy(buf.data(), this, buf.size());
		}
	};

	struct FCustomParameter
	{
		FColor128 Color;

		FCustomParameter()
			: Color(0x0)
		{}

		void GetBuffer(FBuf& buf) const
		{
			buf.resize(GetAlignedSize(sizeof(FCustomParameter), 16));
			memcpy(buf.data(), this, buf.size());
		}
	};
}

