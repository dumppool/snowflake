/*
* file ShaderManager.h
*
* author luoxw
* date 2017/09/22
*
*
*/

#include "stdafx.h"
#include "ShaderManager.h"
#include "Buffers/VertexDef.h"

using namespace D3D11;
using namespace LostCore;

static const string SShaderBlobs = "blobs.fxb";
static const string SShaderMainFX = "Main.fx";
static const string SShaderConstantFX = "Constants.fx";
static const string SShaderVsMain = "VsMain";
static const string SShaderPsMain = "PsMain";

D3D11::FShaderManager::FShaderManager()
{
	Load(nullptr);
}

D3D11::FShaderManager::~FShaderManager()
{
	Save();
	Destroy();
}

void D3D11::FShaderManager::Destroy()
{
	for (auto it : ShaderMap)
	{
		SAFE_DELETE(it.second);
	}

	KeyBlobs.clear();
	ShaderMap.clear();
}

FShaderObject * D3D11::FShaderManager::GetShader(LostCore::IRenderContext* rc, const FShaderKey & key)
{
	auto it = ShaderMap.find(key);
	if (it != ShaderMap.end())
	{
		return it->second;
	}

	return CreateShaderObject(GetBlobs(key));
}

void D3D11::FShaderManager::Save()
{
	// 新数据覆盖旧数据是有必要的，另外还要防止Save在销毁后的调用导致空数据覆盖有价值的数据.
	if (KeyBlobs.empty())
	{
		return;
	}

	string blobsUrl;
	FDirectoryHelper::Get()->GetShaderBlobAbsolutePath(SShaderBlobs, blobsUrl);

	FBinaryIO stream;
	stream << KeyBlobs;
	stream.WriteToFile(blobsUrl);
}

void D3D11::FShaderManager::Load(LostCore::IRenderContext* rc)
{
	string blobsUrl;
	FDirectoryHelper::Get()->GetShaderBlobAbsolutePath(SShaderBlobs, blobsUrl);

	FBinaryIO stream;
	if (stream.ReadFromFile(blobsUrl))
	{
		stream >> KeyBlobs;
	}
}

set<FShaderKeyBlobs>::const_iterator D3D11::FShaderManager::GetBlobs(const FShaderKey & key)
{
	auto blobIt = KeyBlobs.find(FShaderKeyBlobs(key));
	if (blobIt == KeyBlobs.end())
	{
		blobIt = Compile(key);
	}

	return KeyBlobs.end();
}

set<FShaderKeyBlobs>::const_iterator D3D11::FShaderManager::Compile(const FShaderKey & key)
{
	const char* head = "FShaderManager::Compile";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(nullptr, head);
	if (!device.IsValid())
	{
		return KeyBlobs.end();
	}

	FShaderKeyBlobs keyBlobs;
	keyBlobs.Key = key;

	UINT flag1 = 0;
	UINT flag2 = 0;
	TRefCountPtr<ID3DBlob> shaderBlob;
	TRefCountPtr<ID3DBlob> errorBlob;

#ifdef _DEBUG
	flag1 |= D3DCOMPILE_DEBUG;
#endif

	static vector<D3D_SHADER_MACRO> SConstMacros;
	if (SConstMacros.size() == 0)
	{
		SConstMacros.push_back({ NAME_VERTEX_TEXCOORD0, to_string(VERTEX_TEXCOORD0).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_NORMAL, to_string(VERTEX_NORMAL).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_TANGENT, to_string(VERTEX_TANGENT).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_COLOR, to_string(VERTEX_COLOR).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_SKIN, to_string(VERTEX_SKIN).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_TEXCOORD1, to_string(VERTEX_TEXCOORD1).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_COORDINATE2D, to_string(VERTEX_COORDINATE2D).c_str() });
		SConstMacros.push_back({ NAME_MAX_BONES, to_string(MAX_BONES_PER_BATCH).c_str() });
	}

	vector<D3D_SHADER_MACRO> macros;
	// Vertex & Pixel
	{
		std::string url;
		if (!FDirectoryHelper::Get()->GetShaderCodeAbsolutePath(SShaderMainFX, url))
		{
			LVERR(head, "cant find shader file: %s", SShaderMainFX);
			return KeyBlobs.end();
		}

		macros = SConstMacros;
		macros.push_back({ NAME_LIT_MODE, to_string((uint8)key.LitMode).c_str() });
		macros.push_back({ NAME_VERTEX_FLAGS, to_string(key.VertexElement).c_str() });
		macros.push_back({ NAME_CUSTOM_BUFFER, to_string(key.CustomBufferVersion).c_str() });
		macros.push_back({ nullptr, nullptr });

		auto urlw = UTF8ToWide(url);
		HRESULT hr = D3DCompileFromFile(urlw.c_str(), macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SShaderVsMain.c_str(), "vs_5_0", flag1, flag2, shaderBlob.GetInitReference(), errorBlob.GetInitReference());

		if (FAILED(hr) || !shaderBlob.IsValid())
		{
			LVERR(head, "compile %s(%s, %s) failed: %s", SShaderMainFX.c_str(), SShaderVsMain.c_str(),
				"vs_5_0", errorBlob.IsValid() ? errorBlob->GetBufferPointer() : "[empty]");
			return KeyBlobs.end();
		}

		auto& vsBlob = keyBlobs.Blobs[EShaderID::Vertex];
		vsBlob.reserve(shaderBlob->GetBufferSize());
		memcpy(vsBlob.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

		if (FAILED(hr = D3DCompileFromFile(urlw.c_str(), macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SShaderPsMain.c_str(), "ps_5_0", flag1, flag2, shaderBlob.GetInitReference(), errorBlob.GetInitReference())))
		{
			LVERR(head, "compile %s(%s, %s) failed: %s", SShaderMainFX.c_str(), SShaderPsMain.c_str(),
				"ps_5_0", errorBlob.IsValid() ? errorBlob->GetBufferPointer() : "[empty]");
			return KeyBlobs.end();
		}

		auto& psBlob = keyBlobs.Blobs[EShaderID::Pixel];
		psBlob.reserve(shaderBlob->GetBufferSize());
		memcpy(psBlob.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
	}

	auto it = KeyBlobs.insert(keyBlobs);
	LVMSG(head, "inserting new compiled blob %s%s", keyBlobs.ToString().c_str(), it.second ? "." : " failed!");

	return it.first;
}

FShaderObject* D3D11::FShaderManager::CreateShaderObject(set<FShaderKeyBlobs>::const_iterator blobIt)
{
	const char* head = "FShaderManager::CreateShaderObject";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(nullptr, head);
	if (!device.IsValid())
	{
		return nullptr;
	}

	if (blobIt == KeyBlobs.end())
	{
		return nullptr;
	}

	auto shaderObj = new FShaderObject;

	auto& vsBlob = blobIt->Blobs.find(EShaderID::Vertex)->second;
	if (FAILED(device->CreateVertexShader(vsBlob.data(), vsBlob.capacity(), nullptr, shaderObj->VS.GetInitReference())))
	{
		LVERR(head, "Failed to create verter shader with %s", blobIt->ToString().c_str());
		SAFE_DELETE(shaderObj);
		return nullptr;
	}

	auto inputLayoutDesc = FInputElementDescMap::Get()->GetDesc(blobIt->Key.VertexElement);
	if (FAILED(device->CreateInputLayout(inputLayoutDesc.data(), inputLayoutDesc.size(), 
		vsBlob.data(), vsBlob.capacity(), shaderObj->IL.GetInitReference())))
	{
		LVERR(head, "Failed to create input layout with %s", blobIt->ToString().c_str());
		SAFE_DELETE(shaderObj);
		return nullptr;
	}

	auto& psBlob = blobIt->Blobs.find(EShaderID::Pixel)->second;
	if (FAILED(device->CreatePixelShader(psBlob.data(), psBlob.capacity(), nullptr, shaderObj->PS.GetInitReference())))
	{
		LVERR(head, "Failed to create pixel shader with %s", blobIt->ToString().c_str());
		SAFE_DELETE(shaderObj);
		return nullptr;
	}

	ShaderMap[blobIt->Key] = shaderObj;
	LVMSG(head, "inserting shader %s", blobIt->ToString().c_str());
	return shaderObj;
}

D3D11::FShaderKey::FShaderKey()
	: LitMode(ELightingMode::UnLit)
	, VertexElement(0)
	, CustomBufferVersion(0)
	, CodeVersion(0) // TODO: Should be read from shader code.
{}

bool D3D11::FShaderKey::operator==(const FShaderKey& rhs) const
{
	return LitMode == rhs.LitMode &&
		VertexElement == rhs.VertexElement &&
		CustomBufferVersion == rhs.CustomBufferVersion &&
		CodeVersion == rhs.CodeVersion;
}

bool D3D11::FShaderKey::operator<(const FShaderKey& rhs) const
{
	if (CodeVersion < rhs.CodeVersion)
	{
		return true;
	}
	else if (CodeVersion == rhs.CodeVersion)
	{
		if ((uint8)LitMode < (uint8)rhs.LitMode)
		{
			return true;
		}
		else if (LitMode == rhs.LitMode)
		{
			if (VertexElement < rhs.VertexElement)
			{
				return true;
			}
			else if (VertexElement == rhs.VertexElement)
			{
				if (CustomBufferVersion < rhs.CustomBufferVersion)
				{
					return true;
				}
			}
		}
	}

	return false;
}

D3D11::FShaderKeyBlobs::FShaderKeyBlobs() : FShaderKeyBlobs(FShaderKey())
{
}

D3D11::FShaderKeyBlobs::FShaderKeyBlobs(const FShaderKey & key) : Key(key)
{
	Blobs[EShaderID::Vertex] = vector<uint8>(0);
	Blobs[EShaderID::Hull] = vector<uint8>(0);
	Blobs[EShaderID::Domain] = vector<uint8>(0);
	Blobs[EShaderID::Geometry] = vector<uint8>(0);
	Blobs[EShaderID::Compute] = vector<uint8>(0);
	Blobs[EShaderID::Pixel] = vector<uint8>(0);
}

bool D3D11::FShaderKeyBlobs::operator<(const FShaderKeyBlobs& rhs) const
{
	return Key < rhs.Key;
}

string D3D11::FShaderKeyBlobs::ToString() const
{
	static const char* SShaderString[] = {
		"Vertex",
		"Hull",
		"Domain",
		"Geometry",
		"Compute",
		"Pixel", };

	string shaderInfo;
	for (auto& it : Blobs)
	{
		char info[32];
		memset(info, 0, 32);
		snprintf(info, 31, "-%s-%dB", SShaderString[(uint8)it.first], it.second.size());
		shaderInfo.append(info);
	}

	char result[64];
	memset(result, 0, 64);
	snprintf(result, 63, "lit%d-ve%d-buf%d-code%d-%s", (uint8)Key.LitMode, 
		Key.VertexElement, Key.CustomBufferVersion, Key.CodeVersion, shaderInfo.c_str());

	return result;
}
