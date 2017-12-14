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
			memcpy(buf.data(), &result, sizeof(result));
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
			memcpy(buf.data(), &result, sizeof(result));
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
			memcpy(buf.data(), &result, sizeof(result));
		}
	};

	struct FRectParameter
	{
		// 相对父面板的位置，单位，像素
		FFloat2 Offset;

		// 大小尺寸，像素
		FFloat2 Size;

		// 相对父面板的缩放，(0.f, 1.f]
		FFloat2 Scale;

		FRectParameter(const FFloat2 offset, const FFloat2& size, const FFloat2& scale)
			: Size(size)
			, Offset(offset)
			, Scale(scale)
		{
		}

		FRectParameter()
			: Size(0.f, 0.f)
			, Offset(0.f, 0.f)
			, Scale(1.f, 1.f) 
		{
		}

		void GetBuffer(FBuf& buf) const
		{
			buf.resize(GetAlignedSize(sizeof(FRectParameter), 16));
			memcpy(buf.data(), this, sizeof(FRectParameter));
		}
	};

	struct FTextileParameter
	{
		FFloat2 Offset;
		FFloat2 Scale;

		FTextileParameter(const FFloat2& offset, const FFloat2& scale)
			: Offset(offset)
			, Scale(scale)
		{
		}

		FTextileParameter()
			: Offset(0.0f, 0.0f)
			, Scale(1.0f, 1.0f)
		{}

		void GetBuffer(FBuf& buf) const 
		{
			buf.resize(GetAlignedSize(sizeof(FTextileParameter), 16));
			memcpy(buf.data(), this, sizeof(FTextileParameter));
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
			memcpy(buf.data(), this, sizeof(FCustomParameter));
		}
	};
}

