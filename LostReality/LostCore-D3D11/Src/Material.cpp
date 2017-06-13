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
#include "VertexDef.h"

using namespace LostCore;
using namespace std;

bool D3D11::FMaterialShader::LoadShader(LostCore::IRenderContext * rc, const string& path,
	uint32 idMask, const string& entry, const string& target, const string& vertexName)
{
	const char* head = "D3D11::FMaterialShader::LoadShader";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(rc, head);
	if (!device.IsValid())
	{
		return false;
	}

	ShaderPath = path;

	UINT flag1 = 0;
	UINT flag2 = 0;
	TRefCountPtr<ID3DBlob> shaderBlob;
	TRefCountPtr<ID3DBlob> errorBlob;

#ifdef _DEBUG
	flag1 |= D3DCOMPILE_DEBUG;
#endif

	std::string absPath;
	if (!FDirectoryHelper::Get()->GetShaderAbsolutePath(path, absPath))
	{
		LVERR(head, "cant find shader file: %s", path);
		return false;
	}

	auto absPathW = UTF8ToWide(absPath);
	HRESULT hr = D3DCompileFromFile(absPathW.c_str(), nullptr, nullptr, entry.c_str(), target.c_str(),
		flag1, flag2, shaderBlob.GetInitReference(), errorBlob.GetInitReference());

	if (FAILED(hr) || !shaderBlob.IsValid())
	{
		LVERR(head, "compile %s(%s, %s) failed: %s", path.c_str(), entry.c_str(), target.c_str(),
			errorBlob.IsValid() ? errorBlob->GetBufferPointer() : "[empty]");
		return false;
	}
	else
	{
		LVMSG(head, "compile %s(%s, %s) succeeded", path.c_str(), entry.c_str(), target.c_str());
	}

	auto desc = FInputElementDescMap::Get()->GetDesc(vertexName);
	if (idMask == SShaderID_Vertex && desc.first != nullptr)
	{				
		hr = device->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
			nullptr, VS.GetInitReference());
		if (FAILED(hr))
		{
			LVERR(head, "create vertex shader(%s) failed: 0x%08x(%d)", path.c_str(), hr, hr);
			return false;
		}

		hr = device->CreateInputLayout(desc.first, desc.second, shaderBlob->GetBufferPointer(),
			shaderBlob->GetBufferSize(), IL.GetInitReference());
		if (FAILED(hr))
		{
			LVERR(head, "create input layout(%s) failed: 0x%08x(%d)", path.c_str(), hr, hr);
			return false;
		}
	}
	else if (idMask == SShaderID_Pixel)
	{
		hr = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(),
			nullptr, PS.GetInitReference());
		if (FAILED(hr))
		{
			LVERR(head, "create pixel shader(%s) failed: 0x08x(%d)", path, hr, hr);
			return false;
		}
	}

	return true;
}

bool D3D11::FMaterialShader::Initialize(LostCore::IRenderContext * rc, const FJson & config)
{
	const char* head = "D3D11::FMaterialShader::InitializeMaterialShader";

	Reset();

	FJson::const_iterator itVS = config.end();
	FJson::const_iterator itVertex = config.end();

	bool bSuccess = true;

	for (FJson::const_iterator it = config.begin(); it != config.end(); ++it)
	{
		if (it.key().compare("vertex") == 0)
		{
			itVertex = it;
		}
		else if (it.key().compare("vs") == 0)
		{
			itVS = it;
		}
		else if (it.key().compare("ps") == 0)
		{
			bSuccess &= LoadShader(rc, it.value()["path"], SShaderID_Pixel, it.value()["entry"], it.value()["target"]);
		}

		if (itVS != config.end() && itVertex != config.end())
		{
			bSuccess &= LoadShader(rc, itVS.value()["path"], SShaderID_Vertex, itVS.value()["entry"], itVS.value()["target"], itVertex.value());
			itVS = itVertex = config.end();
		}
	}

	return bSuccess;
}

void D3D11::FMaterialShader::Reset()
{
	VS = nullptr;
	PS = nullptr;
	IL = nullptr;
}
