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

using namespace LostCore;

void D3D11::FMaterial::Tick(float sec)
{
}

void D3D11::FMaterial::Draw(float sec, IRenderContext * rc)
{
	ID3D11DeviceContext* cxt = static_cast<ID3D11DeviceContext*>(rc->GetContextRHI());
	if (cxt == nullptr)
	{
		return;
	}

	if (VertexShader != nullptr)
	{
		VertexShader->Draw(sec, rc);
	}

	if (PixelShader != nullptr)
	{
		PixelShader->Draw(sec, rc);
	}
}

void * D3D11::FMaterial::GetRHI()
{
	return nullptr;
}

void D3D11::FMaterial::SetVertexShader(IShader * shader)
{
	VertexShader = shader;
}

const IShader * D3D11::FMaterial::GetVertexShader() const
{
	return VertexShader;
}

void D3D11::FMaterial::SetPixelShader(IShader * shader)
{
	PixelShader = shader;
}

const IShader * D3D11::FMaterial::GetPixelShader() const
{
	return PixelShader;
}
