/*
* file Shader.cpp
*
* author luoxw
* date 2017/02/14
*
*
*/

#include "stdafx.h"
#include "Shader.h"

using namespace LostCore;

void D3D11::FShader::Tick(float sec)
{
}

void D3D11::FShader::Draw(float sec, IRenderContext * rc)
{
	for (auto param : Parameters)
	{
		if (param.second != nullptr)
		{
		}
	}

	int32 count = 0;
	ID3D11ShaderResourceView* srvs[8];
	for (auto element : ShaderResources)
	{
		assert(element.first < 8);
		auto tex = element.second;
		if (tex != nullptr && tex->IsShaderResource())
		{
			++count;
			srvs[element.first] = static_cast<ID3D11ShaderResourceView*>(tex->GetShaderResourceRHI());
		}
		else
		{
			assert(0);
		}
	}

	if (count > 0)
	{
		BindShaderResources(rc, srvs, count);
	}
}

void * D3D11::FShader::GetRHI()
{
	return nullptr;
}

void D3D11::FShader::SetShaderParameter(uint32 slot, IShaderParameter * sp)
{
}

void D3D11::FShader::SetShaderResource(uint32 slot, ITexture * tex)
{
}

void D3D11::FShader::BindShaderResources(IRenderContext * rc, void * srvs, int32 count)
{
}
