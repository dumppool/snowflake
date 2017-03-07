/*
* file Material.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

#include "BufferDef.h"

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
		bool Initialize(LostCore::IRenderContext* rc, const FJson& config);

		INLINE TRefCountPtr<ID3D11InputLayout> GetInputLayout()
		{
			return IL;
		}

		INLINE TRefCountPtr<ID3D11VertexShader> GetVertexShader()
		{
			return VS;
		}

		INLINE TRefCountPtr<ID3D11PixelShader> GetPixelShader()
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
	};

	class FMaterial : public LostCore::IMaterial
	{
	public:
		FMaterial();
		virtual ~FMaterial() override;

		// Í¨¹ý IMaterial ¼Ì³Ð
		virtual void Draw(LostCore::IRenderContext * rc, float sec) override;
		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) override;
		virtual void UpdateMatrix_World(LostCore::IRenderContext * rc, const LostCore::FMatrix& mat) override;

	private:
		FMaterialShader*				MaterialShader;
		FConstantBufferOneMatrix		World;
	};
}