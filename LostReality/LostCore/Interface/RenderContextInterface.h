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

	INLINE const char* GetContextDesc(EContextID id)
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

	class IRenderContext
	{
	public:
		virtual ~IRenderContext() {}

		virtual bool Init(HWND wnd, bool bWindowed, int32 width, int32 height) = 0;
		virtual void Fini() = 0;

		virtual bool EnableShadeModel(EShadeModel sm) = 0;
		virtual EShadeModel GetShadeModel() const = 0;

		virtual FFloat4x4 GetViewProjectMatrix() const = 0;
		virtual void SetViewProjectMatrix(const FFloat4x4& vp) = 0;

		virtual EContextID GetContextID() const = 0;
		virtual const char* GetContextString() const = 0;

		virtual void BeginFrame(float sec) = 0;
		virtual void EndFrame(float sec) = 0;

		virtual float GetWidth() const = 0;
		virtual float GetHeight() const = 0;
	};
}