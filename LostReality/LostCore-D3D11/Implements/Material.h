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
		FMaterial();

		virtual ~FMaterial() override;

		// Í¨¹ý IMaterial ¼Ì³Ð
		virtual void Bind(LostCore::IRenderContext * rc) override;
		virtual bool Initialize(LostCore::IRenderContext * rc, const char* path) override;
		//virtual void UpdateTexture(LostCore::IRenderContext* rc, LostCore::ITextureSet* tex, int32 slot);
		virtual void SetDepthStencilState(const char* name) override;

		bool InitializeShader(LostCore::IRenderContext * rc, const char* path);

	private:
		std::map<int32, FTexture2D*> Textures;
		string DepthStencilName;
	};
}