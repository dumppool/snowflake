/*
* file Shader.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

#include "ShaderParameter.h"
#include "Texture.h"

namespace D3D11
{
	class FShader : public LostCore::IShader
	{
	public:

		// Í¨¹ý IShader ¼Ì³Ð
		virtual void Tick(float sec) override;
		virtual void Draw(float sec, LostCore::IRenderContext * rc) override;
		virtual void * GetRHI() override;
		virtual void SetShaderParameter(uint32 slot, LostCore::IShaderParameter * sp) override;
		virtual void SetShaderResource(uint32 slot, LostCore::ITexture * tex) override;

	protected:
		virtual void BindShaderResources(LostCore::IRenderContext * rc, void* srvs, int32 count);

	protected:
		std::map<uint32, FShaderParameter*> Parameters;
		std::map<uint32, FTexture2D*> ShaderResources;
	};
}