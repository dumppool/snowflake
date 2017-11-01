/*
* file LostCore-D3D11.cpp
*
* author luoxw
* date 2017/01/18
*
*
*/

#include "stdafx.h"
#include "Implements/ConstantBuffer.h"
#include "Implements/PrimitiveGroup.h"
#include "Implements/Material.h"

using namespace D3D11;
using namespace LostCore;

bool LostCore::FProcessUnique::SIsOriginal = false;
LostCore::FProcessUnique* LostCore::FProcessUnique::SInstance = nullptr;

EReturnCode D3D11::InitializeProcessUnique()
{
	LostCore::FProcessUnique::StaticInitialize();
	return SSuccess;
}

EReturnCode D3D11::DestroyProcessUnique()
{
	LostCore::FProcessUnique::StaticDestroy();
	return SSuccess;
}

EReturnCode D3D11::SetProcessUnique(void* p)
{
	LVMSG("D3D11::SetProcessUnique", "ProcessUnique: 0x%08x", p);
	LostCore::FProcessUnique::SetInstance((LostCore::FProcessUnique*)p);
	return SSuccess;
}

EReturnCode D3D11::CreateRenderContext(LostCore::IRenderContext** context)
{
	if (context == nullptr)
	{
		return SErrorInvalidParameters;
	}

	assert(FRenderContext::Get() == nullptr);
	FRenderContext::Get() = new FRenderContext;
	*context = FRenderContext::Get();
	return SSuccess;
}

EReturnCode D3D11::DestroyRenderContext(LostCore::IRenderContext * context)
{
	assert(FRenderContext::Get() != nullptr);
	SAFE_DELETE(FRenderContext::Get());
	return SSuccess;
}

EReturnCode D3D11::CreatePrimitiveGroup(IPrimitiveGroupPtr& pgPtr)
{
	pgPtr = IPrimitiveGroupPtr(new FPrimitiveGroup);
	return SSuccess;
}

EReturnCode D3D11::CreateConstantBuffer(IConstantBufferPtr& cbPtr)
{
	cbPtr = IConstantBufferPtr(new FConstantBuffer);
	return SSuccess;
}

/*
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
*/

#include "Implements/GdiFont.h"

EReturnCode D3D11::CreateGdiFont(IFontPtr& fontPtr)
{
	fontPtr = IFontPtr(new FGdiFont);
	return SSuccess;
}