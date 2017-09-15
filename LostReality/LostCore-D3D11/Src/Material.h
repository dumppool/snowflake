/*
* file Material.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

#include "DepthStencilStateDef.h"
#include "ConstantBuffer.h"

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

	class FMaterial : public LostCore::IMaterial
	{
	public:
		FMaterial();

		virtual ~FMaterial() override;

		// Í¨¹ý IMaterial ¼Ì³Ð
		virtual void Bind(LostCore::IRenderContext * rc) override;
		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) override;
		virtual void UpdateTexture(LostCore::IRenderContext* rc, LostCore::ITexture* tex, int32 slot);
		virtual void SetDepthStencilState(const char* name) override;

		bool InitializeShader(LostCore::IRenderContext * rc, const char* path);

	protected:
		FMaterialShader* GetMaterialShader();

	private:
		FMaterialShader* MaterialShader;
		std::map<int32, FTexture2D*> Textures;
		string DepthStencilName;
	};
}