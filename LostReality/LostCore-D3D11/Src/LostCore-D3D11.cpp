/*
* file LostCore-D3D11.cpp
*
* author luoxw
* date 2017/01/18
*
*
*/

#include "stdafx.h"

using namespace D3D11;

EReturnCode CreateRenderContext(LostCore::EContextID id, LostCore::IRenderContext** context)
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
		obj = new FRenderContext(id);
		if (obj == nullptr)
		{
			return SErrorOutOfMemory;
		}
		else
		{
			assert(id == obj->GetContextID());
			*context = obj;
			return SSuccess;
		}
	}
	default:
		return SErrorInternalError;
	}
}

EReturnCode DestroyRenderContext(LostCore::IRenderContext * context)
{
	if (context != nullptr)
	{
		delete context;
	}

	return SSuccess;
}

#include "PrimitiveGroup.h"

EReturnCode CreatePrimitiveGroup(LostCore::IPrimitiveGroup ** pg)
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

EReturnCode DestroyPrimitiveGroup(LostCore::IPrimitiveGroup * pg)
{
	if (pg != nullptr)
	{
		delete pg;
	}

	return SSuccess;
}

#include "ConstantBuffer.h"

EReturnCode CreateConstantBuffer(LostCore::IConstantBuffer** cb)
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

EReturnCode DestroyConstantBuffer(LostCore::IConstantBuffer * cb)
{
	if (cb != nullptr)
	{
		delete cb;
	}

	return SSuccess;
}

#include "Material.h"

EReturnCode CreateMaterial(LostCore::IMaterial** material)
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

EReturnCode DestroyMaterial(LostCore::IMaterial * material)
{
	if (material != nullptr)
	{
		delete material;
	}

	return SSuccess;
}

#include "GdiFont.h"

EReturnCode CreateGdiFont(LostCore::IFontInterface ** font)
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

EReturnCode DestroyGdiFont(LostCore::IFontInterface * font)
{
	if (font != nullptr)
	{
		delete font;
	}

	return SSuccess;
}
