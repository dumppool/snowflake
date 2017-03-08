/*
* file LostCore-D3D11.cpp
*
* author luoxw
* date 2017/01/18
*
*
*/

#include "stdafx.h"
#include "LostCore-D3D11.h"

using namespace D3D11;

// ���ǵ���������һ��ʾ��
LOSTCORED3D11_API int nLostCoreD3D11=0;

// ���ǵ���������һ��ʾ����
LOSTCORED3D11_API int fnLostCoreD3D11(void)
{
    return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� LostCore-D3D11.h
CLostCoreD3D11::CLostCoreD3D11()
{
    return;
}

#include "RenderContext.h"

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

EReturnCode D3D11::DestroyRenderContext(LostCore::IRenderContext * context)
{
	if (context != nullptr)
	{
		delete context;
	}

	return SSuccess;
}

#include "PrimitiveGroup.h"

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

LOSTCORED3D11_API EReturnCode D3D11::DestroyPrimitiveGroup(LostCore::IPrimitiveGroup * pg)
{
	if (pg != nullptr)
	{
		delete pg;
	}

	return SSuccess;
}

#include "Material.h"

LOSTCORED3D11_API EReturnCode D3D11::CreateMaterial(LostCore::IMaterial ** material)
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

LOSTCORED3D11_API EReturnCode D3D11::DestroyMaterial(LostCore::IMaterial * material)
{
	if (material != nullptr)
	{
		delete material;
	}

	return SSuccess;
}