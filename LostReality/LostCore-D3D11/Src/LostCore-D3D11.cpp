/*
* file LostCore-D3D11.cpp
*
* author luoxw
* date 2017/01/18
*
*
*/

#include "stdafx.h"
#include "Buffers/ConstantBuffer.h"
#include "Implements/PrimitiveGroup.h"
#include "Implements/Material.h"

using namespace D3D11;

EReturnCode D3D11::CreateRenderContext(LostCore::EContextID id, LostCore::IRenderContext** context)
{
	if (context == nullptr)
	{
		return SErrorInvalidParameters;
	}

	LostCore::IRenderContext* obj = nullptr;
	switch (id)
	{
	case LostCore::EContextID::D3D11_DXGI0:
	case LostCore::EContextID::D3D11_DXGI1:
	{
		obj = FRenderContext::Get();
		if (obj->Initialize(id))
		{
			return SSuccess;
		}
		else
		{
			return SErrorInternalError;
		}
	}
	default:
		return SErrorInvalidParameters;
	}
}

EReturnCode D3D11::DestroyRenderContext(LostCore::IRenderContext * context)
{
	if (context != nullptr)
	{
		delete context;
	}

	return SSuccess;
}


EReturnCode D3D11::CreatePrimitiveGroup(LostCore::IPrimitiveGroup ** pg)
{
	if (pg == nullptr)
	{
		return SErrorInvalidParameters;
	}

	auto obj = new FPrimitiveGroup;
	if (obj == nullptr)
	{
		return SErrorOutOfMemory;
	}

	*pg = obj;
	return SSuccess;
}

EReturnCode D3D11::DestroyPrimitiveGroup(LostCore::IPrimitiveGroup * pg)
{
	if (pg != nullptr)
	{
		delete pg;
	}

	return SSuccess;
}


EReturnCode D3D11::CreateConstantBuffer(LostCore::IConstantBuffer** cb)
{
	if (cb == nullptr)
	{
		return SErrorInvalidParameters;
	}

	auto obj = new FConstantBuffer;
	if (obj == nullptr)
	{
		return SErrorOutOfMemory;
	}

	*cb = obj;
	return SSuccess;
}

EReturnCode D3D11::DestroyConstantBuffer(LostCore::IConstantBuffer * cb)
{
	if (cb != nullptr)
	{
		delete cb;
	}

	return SSuccess;
}


EReturnCode D3D11::CreateMaterial(LostCore::IMaterial** material)
{
	if (material == nullptr)
	{
		return SErrorInvalidParameters;
	}

	auto obj = new FMaterial;
	if (obj == nullptr)
	{
		return SErrorOutOfMemory;
	}

	*material = obj;
	return SSuccess;
}

EReturnCode D3D11::DestroyMaterial(LostCore::IMaterial * material)
{
	if (material != nullptr)
	{
		delete material;
	}

	return SSuccess;
}

#include "GdiFont.h"

EReturnCode D3D11::CreateGdiFont(LostCore::IFontInterface ** font)
{
	if (font == nullptr)
	{
		return SErrorInvalidParameters;
	}

	auto obj = new FGdiFont;
	if (obj == nullptr)
	{
		return SErrorOutOfMemory;
	}

	*font = obj;
	return SSuccess;
}

EReturnCode D3D11::DestroyGdiFont(LostCore::IFontInterface * font)
{
	if (font != nullptr)
	{
		delete font;
	}

	return SSuccess;
}
