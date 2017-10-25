/*
* file RenderContextInterface.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

#include "Math/Matrix.h"
#include "Math/Color.h"

namespace LostCore
{
	class ITexture;

	enum class EContextID : uint8
	{
		Undefined,
		D3D11_DXGI0,
		D3D11_DXGI1,
	};

	enum class EShadeModel : uint8
	{
		Undefined,
		SM5,
	};

	FORCEINLINE const char* GetContextDesc(EContextID id)
	{
		switch (id)
		{
		case EContextID::D3D11_DXGI0:
			return "d3d11 dxgi 0";
		case EContextID::D3D11_DXGI1:
			return "d3d11 dxgi 1";
		default:
			return "[unknown]";
		}
	}

	struct FExtendParam
	{
		FColor128 Color;
	};

	class IRenderContext
	{
	public:
		virtual ~IRenderContext() {}

		virtual bool Initialize(LostCore::EContextID id) = 0;
		virtual bool InitializeScreen(HWND wnd, bool bWindowed, int32 width, int32 height) = 0;
		virtual void BeginFrame() = 0;
		//virtual void RenderFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void SetViewProjectMatrix(const FFloat4x4& vp) = 0;
	};
}