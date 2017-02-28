/*
* file Material.cpp
*
* author luoxw
* date 2017/02/14
*
*
*/

#include "stdafx.h"
#include "Material.h"
#include "RenderContext.h"

using namespace LostCore;

bool D3D11::FMaterialShader::LoadShader(FRenderContext * rc, const char * path, uint32 idMask)
{
	const char* head = "D3D11::FMaterialShader::LoadShader";
	ID3D11DeviceContext* cxt = FRenderContext::GetDeviceContext(rc, head);
	if (cxt == nullptr)
	{
		return false;
	}

	EShadeModel sm = rc->GetShadeModel();
	if ((idMask & SShaderID_Vertex) == 1)
	{
		if (sm == EShadeModel::SM5)
		{
		}
	}

	return false;
}


void D3D11::FMaterial::Draw(IRenderContext * rc, float sec)
{

	if (!VertexShader.IsValid())
	{
		return;
	}

	cxt->VSSetShader
}

bool D3D11::FMaterial::LoadShader(const char * path)
{
	return false;
}

void D3D11::FMaterial::UpdateMatrix_World(const FMatrix& mat)
{

}