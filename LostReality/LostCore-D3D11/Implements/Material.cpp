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
#include "Implements/Texture.h"
#include "Buffers/VertexDef.h"
#include "States/DepthStencilStateDef.h"

using namespace D3D11;
using namespace LostCore;

D3D11::FMaterial::FMaterial()
{}

D3D11::FMaterial::~FMaterial()
{
}

void D3D11::FMaterial::Bind(LostCore::IRenderContext * rc)
{
	const char* head = "D3D11::FMaterial::Draw";
	auto cxt = FRenderContext::GetDeviceContext(head);
	if (!cxt.IsValid())
	{
		return;
	}

	TRefCountPtr<ID3D11VertexShader> vs;
	TRefCountPtr<ID3D11PixelShader> ps;
	TRefCountPtr<ID3D11InputLayout> il;
	if (vs.IsValid() && ps.IsValid() && il.IsValid())
	{
		cxt->VSSetShader(vs.GetReference(), nullptr, 0);
		cxt->IASetInputLayout(il.GetReference());
		cxt->PSSetShader(ps.GetReference(), nullptr, 0);

		cxt->HSSetShader(nullptr, nullptr, 0);
		cxt->DSSetShader(nullptr, nullptr, 0);
		cxt->GSSetShader(nullptr, nullptr, 0);
		cxt->CSSetShader(nullptr, nullptr, 0);

		for (auto it = Textures.begin(); it != Textures.end(); ++it)
		{
			if (it->second != nullptr)
			{
				auto srv = it->second->GetShaderResourceRHI().GetReference();
				auto smp = it->second->GetSamplerRHI().GetReference();
				cxt->PSSetShaderResources(it->first, 1, &srv);
				cxt->PSSetSamplers(it->first, 1, &smp);
			}
		}
	}

	cxt->OMSetDepthStencilState(FDepthStencilStateMap::Get()->GetState(DepthStencilName), 0);
}

bool D3D11::FMaterial::InitializeShader(LostCore::IRenderContext * rc, const char * path)
{
	const char* head = "D3D11::FMaterial::InitializeMaterial";

	FJson config;
	if (!LostCore::FDirectoryHelper::Get()->GetMaterialJson(path, config))
	{
		return false;
	}

	return false;
}

bool D3D11::FMaterial::Initialize(LostCore::IRenderContext * rc, const char * path)
{
	return InitializeShader(rc, path);
}

void D3D11::FMaterial::UpdateTexture(LostCore::IRenderContext * rc, LostCore::ITexture * tex, int32 slot)
{
	Textures[slot] = (FTexture2D*)tex;
}

void D3D11::FMaterial::SetDepthStencilState(const char * name)
{
	DepthStencilName = name;
}

