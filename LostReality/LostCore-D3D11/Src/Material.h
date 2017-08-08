/*
* file Material.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

//#include "Constant.h"

namespace D3D11
{
	static const uint32 SShaderID_Vertex = 0x1 << 0;
	static const uint32 SShaderID_Pixel = 0x1 << 1;
	static const uint32 SShaderID_Geometry = 0x1 << 2;
	static const uint32 SShaderID_Hull = 0x1 << 3;
	static const uint32 SShaderID_Domain = 0x1 << 4;
	static const uint32 SShaderID_Compute = 0x1 << 5;

	class FMaterialShader
	{
	public:
		FMaterialShader();
		~FMaterialShader();

		bool Initialize(LostCore::IRenderContext* rc, const FJson& config);

		FORCEINLINE TRefCountPtr<ID3D11InputLayout> GetInputLayout()
		{
			return IL;
		}

		FORCEINLINE TRefCountPtr<ID3D11VertexShader> GetVertexShader()
		{
			return VS;
		}

		FORCEINLINE TRefCountPtr<ID3D11PixelShader> GetPixelShader()
		{
			return PS;
		}

	private:
		// @param vertexName		see Src/BufferDef.h for details
		bool LoadShader(LostCore::IRenderContext* rc, const std::string& path, uint32 idMask,
			const std::string& entry, const std::string& target, const std::string& vertexName = "");

		void Reset();

		TRefCountPtr<ID3D11VertexShader>		VS;
		TRefCountPtr<ID3D11PixelShader>			PS;
		TRefCountPtr<ID3D11InputLayout>			IL;

		std::string								ShaderPath;
	};

	template<typename T>
	class FMaterial : public LostCore::IMaterial
	{
	public:
		FMaterial()
			: MaterialShader(nullptr)
			, Param()
		{}

		virtual ~FMaterial() override
		{
			if (MaterialShader != nullptr)
			{
				delete MaterialShader;
				MaterialShader = nullptr;
			}
		}

		// Í¨¹ý IMaterial ¼Ì³Ð
		virtual void Draw(LostCore::IRenderContext * rc, float sec) override
		{
			const char* head = "D3D11::FMaterial::Draw";
			auto cxt = FRenderContext::GetDeviceContext(rc, head);
			if (!cxt.IsValid())
			{
				return;
			}

			auto shader = GetMaterialShader();
			if (shader == nullptr)
			{
				return;
			}

			TRefCountPtr<ID3D11VertexShader> vs = shader->GetVertexShader();
			TRefCountPtr<ID3D11PixelShader> ps = shader->GetPixelShader();
			TRefCountPtr<ID3D11InputLayout> il = shader->GetInputLayout();
			if (vs.IsValid() && ps.IsValid() && il.IsValid())
			{
				cxt->VSSetShader(vs.GetReference(), nullptr, 0);
				cxt->IASetInputLayout(il.GetReference());
				cxt->PSSetShader(ps.GetReference(), nullptr, 0);

				cxt->HSSetShader(nullptr, nullptr, 0);
				cxt->DSSetShader(nullptr, nullptr, 0);
				cxt->GSSetShader(nullptr, nullptr, 0);
				cxt->CSSetShader(nullptr, nullptr, 0);

				Param.Bind(cxt);

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
		}

		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) override
		{
			const char* head = "D3D11::FMaterial::Initialize";
			auto device = FRenderContext::GetDevice(rc, head);
			if (!device.IsValid())
			{
				return false;
			}

			if (!Param.Initialize(device))
			{
				return false;
			}

			FJson config;
			if (!LostCore::FDirectoryHelper::Get()->GetMaterialJson(path, config))
			{
				return false;
			}

			if (MaterialShader != nullptr)
			{
				delete MaterialShader;
				MaterialShader = nullptr;
			}

			MaterialShader = new FMaterialShader;
			return MaterialShader->Initialize(rc, config);
		}

		virtual void UpdateConstantBuffer(LostCore::IRenderContext* rc, const void* buf, int32 sz) override
		{
			const char* head = "D3D11::FMaterial<T>::UpdateConstantBuffer";
			auto cxt = FRenderContext::GetDeviceContext(rc, head);
			if (!cxt.IsValid())
			{
				return;
			}

			Param.UpdateBuffer(cxt, buf, sz);
		}

		virtual void UpdateTexture(LostCore::IRenderContext* rc, LostCore::ITexture* tex, int32 slot)
		{
			Textures[slot] = (FTexture2D*)tex;
		}

	protected:
		FMaterialShader* GetMaterialShader()
		{
			return MaterialShader;
		}

	private:
		FMaterialShader* MaterialShader;
		T Param;
		std::map<int32, FTexture2D*> Textures;
	};
}