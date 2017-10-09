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
}
