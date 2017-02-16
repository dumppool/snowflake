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
	class FMaterial : public LostCore::IMaterial
	{
	public:

		// Í¨¹ý IMaterial ¼Ì³Ð
		virtual void Tick(float sec) override;
		virtual void Draw(float sec, LostCore::IRenderContext * rc) override;
		virtual void * GetRHI() override;
		virtual void SetVertexShader(LostCore::IShader * shader) override;
		virtual const LostCore::IShader * GetVertexShader() const override;
		virtual void SetPixelShader(LostCore::IShader * shader) override;
		virtual const LostCore::IShader * GetPixelShader() const override;

	private:
		//TRefCountPtr<ID3D11VertexShader>		VertexShader;
		//TRefCountPtr<ID3D11PixelShader>			PixelShader;
		LostCore::IShader* VertexShader;
		LostCore::IShader* PixelShader;
	};
}