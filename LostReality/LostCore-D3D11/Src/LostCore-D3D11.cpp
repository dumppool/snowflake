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
#include "Implements/GdiFont.h"


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

EReturnCode D3D11::CreatePrimitiveGroup(IPrimitiveGroup** pg)
{
	*pg = (new FPrimitiveGroup);
	return SSuccess;
}

EReturnCode D3D11::DestroyPrimitiveGroup(IPrimitiveGroup* pg)
{
	FRenderContext::Get()->DeallocPrimitiveGroup(pg);
	return SSuccess;
}

EReturnCode D3D11::CreateConstantBuffer(IConstantBuffer** cb)
{
	*cb = (new FConstantBuffer);
	return SSuccess;
}

EReturnCode D3D11::DestroyConstantBuffer(IConstantBuffer* cb)
{
	FRenderContext::Get()->DeallocConstantBuffer(cb);
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

EReturnCode D3D11::CreateGdiFont(LostCore::IFont** font)
{
	*font = (new FGdiFont);
	return SSuccess;
}

EReturnCode D3D11::DestroyGdiFont(LostCore::IFont* font)
{
	FRenderContext::Get()->DeallocGdiFont(font);
	return SSuccess;
}