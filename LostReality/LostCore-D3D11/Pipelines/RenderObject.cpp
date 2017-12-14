/*
* file RenderObject.cpp
*
* author luoxw
* date 2017/10/09
*
*
*/

#include "stdafx.h"
#include "RenderObject.h"

D3D11::FRenderObject::FRenderObject()
{
	Reset();
}

void D3D11::FRenderObject::Reset()
{
	PrimitiveGroup = nullptr;
	ConstantBuffers.clear();
	ShaderResources.clear();
	InstancingDatas.clear();
}

uint32 D3D11::FRenderObject::GetVertexFlags() const
{
	uint32 flags = PrimitiveGroup->GetFlags();
	for (auto item : InstancingDatas)
	{
		flags |= item->GetFlags();
	}

	return flags;
}
