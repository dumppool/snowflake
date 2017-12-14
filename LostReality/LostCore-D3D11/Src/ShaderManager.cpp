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
	Load();
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

FShaderObject * D3D11::FShaderManager::GetShader(const FShaderKey & key)
{
	auto it = ShaderMap.find(key);
	if (it != ShaderMap.end())
	{
		return it->second;
	}

	return CreateShaderObject(GetBlobs(key));
}

void D3D11::FShaderManager::Bind(const FShaderKey & key)
{
	const char* head = "FShaderManager::Bind";
	auto cxt = FRenderContext::GetDeviceContext(head);

	auto obj = GetShader(key);
	if (obj == nullptr)
	{
		LVERR(head, "GetShader(%s) failed.", key.ToString().c_str());
		return;
	}

	if (obj->VS.IsValid() && obj->IL.IsValid())
	{
		cxt->VSSetShader(obj->VS.GetReference(), nullptr, 0);
		cxt->IASetInputLayout(obj->IL.GetReference());
	}

	if (obj->PS.IsValid())
	{
		cxt->PSSetShader(obj->PS.GetReference(), nullptr, 0);
	}
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

void D3D11::FShaderManager::Load()
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

	return blobIt;
}

set<FShaderKeyBlobs>::const_iterator D3D11::FShaderManager::Compile(const FShaderKey & key)
{
	const char* head = "FShaderManager::Compile";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
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

	static vector<string> temp(20);
	static vector<D3D_SHADER_MACRO> SConstMacros;
	if (SConstMacros.size() == 0)
	{
		SConstMacros.push_back({ NAME_VERTEX_TEXCOORD0, (temp[0] = to_string(VERTEX_TEXCOORD0)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_NORMAL, (temp[1] = to_string(VERTEX_NORMAL)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_TANGENT, (temp[2] = to_string(VERTEX_TANGENT)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_COLOR, (temp[3] = to_string(VERTEX_COLOR)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_SKIN, (temp[4] = to_string(VERTEX_SKIN)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_TEXCOORD1, (temp[5] = to_string(VERTEX_TEXCOORD1)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_COORDINATE3D, (temp[6] = to_string(VERTEX_COORDINATE3D)).c_str() });
		SConstMacros.push_back({ NAME_VERTEX_COORDINATE2D, (temp[7] = to_string(VERTEX_COORDINATE2D)).c_str() });
		SConstMacros.push_back({ NAME_MAX_BONES, (temp[8] = to_string(MAX_BONES_PER_BATCH)).c_str() });
		SConstMacros.push_back({ NAME_INSTANCE_TRANSFORM3D, (temp[9] = to_string(INSTANCE_TRANSFORM3D)).c_str() });
		SConstMacros.push_back({ NAME_INSTANCE_TRANSFORM2D, (temp[10] = to_string(INSTANCE_TRANSFORM2D)).c_str() });
		SConstMacros.push_back({ NAME_INSTANCE_TEXTILE, (temp[11] = to_string(INSTANCE_TEXTILE)).c_str() });
	}

	vector<D3D_SHADER_MACRO> macros;
	vector<string> temp2(10);
	// Vertex & Pixel
	{
		std::string url;
		if (!FDirectoryHelper::Get()->GetShaderCodeAbsolutePath(SShaderMainFX, url))
		{
			LVERR(head, "cant find shader file: %s", SShaderMainFX.c_str());
			return KeyBlobs.end();
		}

		macros = SConstMacros;
		macros.push_back({ NAME_LIT_MODE, (temp2[0] = to_string((uint8)key.LitMode)).c_str() });
		macros.push_back({ NAME_VERTEX_FLAGS, (temp2[1] = to_string(key.VertexElement)).c_str() });
		macros.push_back({ NAME_CUSTOM_BUFFER, (temp2[2] = to_string(key.CustomBufferVersion)).c_str() });
		macros.push_back({ nullptr, nullptr });

		auto urlw = UTF8ToWide(url);
		HRESULT hr = D3DCompileFromFile(urlw.c_str(), macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SShaderVsMain.c_str(), "vs_5_0", flag1, flag2, shaderBlob.GetInitReference(), errorBlob.GetInitReference());

		if (FAILED(hr) || !shaderBlob.IsValid())
		{
			string macroString;
			for (auto item : macros)
			{
				if (item.Name != nullptr)
				{
					macroString.append(item.Name).append(":").append(item.Definition).append(", ");
				}
			}

			LVERR(head, "compile %s(%s, %s) failed: macro: %s, %s.", SShaderMainFX.c_str(), SShaderVsMain.c_str(),
				"vs_5_0", macroString.c_str(), errorBlob.IsValid() ? errorBlob->GetBufferPointer() : "[empty]");
			return KeyBlobs.end();
		}
		
		auto& vsBlob = keyBlobs.Blobs[EShaderID::Vertex];
		vsBlob.resize(shaderBlob->GetBufferSize());
		memcpy(vsBlob.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

		if (FAILED(hr = D3DCompileFromFile(urlw.c_str(), macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
			SShaderPsMain.c_str(), "ps_5_0", flag1, flag2, shaderBlob.GetInitReference(), errorBlob.GetInitReference())))
		{
			LVERR(head, "compile %s(%s, %s) failed: %s", SShaderMainFX.c_str(), SShaderPsMain.c_str(),
				"ps_5_0", errorBlob.IsValid() ? errorBlob->GetBufferPointer() : "[empty]");
			return KeyBlobs.end();
		}

		auto& psBlob = keyBlobs.Blobs[EShaderID::Pixel];
		psBlob.resize(shaderBlob->GetBufferSize());
		memcpy(psBlob.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

		pair<set<FShaderKeyBlobs>::iterator, bool> it = KeyBlobs.insert(keyBlobs);
		LVMSG(head, "insert new compiled blob %s%s", keyBlobs.ToString().c_str(), it.second ? "." : " failed!");
		return it.first;
	}

	return KeyBlobs.end();
}

FShaderObject* D3D11::FShaderManager::CreateShaderObject(set<FShaderKeyBlobs>::const_iterator blobIt)
{
	const char* head = "FShaderManager::CreateShaderObject";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);

	if (blobIt == KeyBlobs.end())
	{
		return nullptr;
	}

	auto shaderObj = new FShaderObject;

	auto& vsBlob = blobIt->Blobs.find(EShaderID::Vertex)->second;
	if (FAILED(device->CreateVertexShader(vsBlob.data(), vsBlob.size(), nullptr, shaderObj->VS.GetInitReference())))
	{
		LVERR(head, "Failed to create verter shader with %s", blobIt->ToString().c_str());
		SAFE_DELETE(shaderObj);
		return nullptr;
	}

	shaderObj->IL = CreateInputLayout(vsBlob, blobIt->Key);
	if (!shaderObj->IL.IsValid())
	{
		LVERR(head, "Failed to create input layout with %s", blobIt->ToString().c_str());
		return nullptr;
	}

	auto& psBlob = blobIt->Blobs.find(EShaderID::Pixel)->second;
	if (FAILED(device->CreatePixelShader(psBlob.data(), psBlob.size(), nullptr, shaderObj->PS.GetInitReference())))
	{
		LVERR(head, "Failed to create pixel shader with %s", blobIt->ToString().c_str());
		SAFE_DELETE(shaderObj);
		return nullptr;
	}

	ShaderMap[blobIt->Key] = shaderObj;
	return shaderObj;
}

TRefCountPtr<ID3D11InputLayout> D3D11::FShaderManager::CreateInputLayout(const FBuf& vsBlob, const FShaderKey & key)
{
	const char* head = "FShaderManager::CreateInputLayout";
	TRefCountPtr<ID3D11Device> device = FRenderContext::GetDevice(head);
	auto desc = FInputElementDescMap::Get()->GetDesc(key.VertexElement);

	TRefCountPtr<ID3D11InputLayout> result;
	device->CreateInputLayout(
		desc.data(),
		desc.size(), 
		vsBlob.data(),
		vsBlob.size(),
		result.GetInitReference());

	return result;
}

D3D11::FShaderKey::FShaderKey()
	: LitMode(ELightingMode::UnLit)
	, VertexElement(0)
	, CustomBufferVersion(0)
	, CodeVersion(0) // TODO: Should be read from shader code.
{}

//bool D3D11::FShaderKey::operator==(const FShaderKey& rhs) const
//{
//	return LitMode == rhs.LitMode &&
//		VertexElement == rhs.VertexElement &&
//		CustomBufferVersion == rhs.CustomBufferVersion &&
//		CodeVersion == rhs.CodeVersion;
//}

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

string D3D11::FShaderKey::ToString() const
{
	char result[128];
	memset(result, 0, 128);
	snprintf(result, 127,
		"lit%d-ve%d-buf%d-ver%d",
		(uint8)LitMode,
		VertexElement,
		CustomBufferVersion, 
		CodeVersion);

	return result;
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
		char info[128];
		memset(info, 0, 128);
		snprintf(info, 127, "-%s-%dB", SShaderString[(uint8)it.first], it.second.size());
		shaderInfo.append(info);
	}

	return Key.ToString().append(shaderInfo);
}
