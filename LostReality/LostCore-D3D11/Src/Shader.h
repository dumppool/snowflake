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
	class FShader
	{
	public:
		virtual void SetShaderParameter(uint32 slot, LostCore::IShaderParameter * sp);
		virtual void SetShaderResource(uint32 slot, LostCore::ITexture * tex);

	protected:
		std::map<uint32, FShaderParameter*> Parameters;
		std::map<uint32, FTexture2D*> ShaderResources;
	};

	class FVertexShader : public FShader
	{
	public:

	private:
		ID3D11InputLayout* IL;
	};

	class FPixelShader : public FShader
	{

	};
}