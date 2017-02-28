/*
* file Material.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

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
		bool LoadShader(FRenderContext* rc, const char* path, uint32 idMask);

		TRefCountPtr<ID3D11InputLayout> GetInputLayout()
		{
			return IL;
		}

		template<typename T> bool GetShader(uint32 idMask, TRefCountPtr<T>& shader)
		{
			if (idMask == SShaderID_Vertex && VS.IsValid())
			{
				shader = VS;
				return true;
			}
			else if (idMask == SShaderID_Pixel && PS.IsValid())
			{
				shader = PS;
				return true;
			}
			else
			{
				return false;
			}
		}

	private:
		TRefCountPtr<ID3D11VertexShader>		VS;
		TRefCountPtr<ID3D11PixelShader>			PS;
		TRefCountPtr<ID3D11InputLayout>			IL;
	};

	class FMaterial : public LostCore::IMaterial
	{
	public:

		// Í¨¹ý IMaterial ¼Ì³Ð
		virtual void Draw(LostCore::IRenderContext * rc, float sec) override;
		virtual bool LoadShader(const char* path) override;
		virtual void UpdateMatrix_World(const FMatrix& mat) override;

	private:
		FMaterialShader*				MaterialShader;
		TRefCountPtr<ID3D11Buffer>		World;
	};
}