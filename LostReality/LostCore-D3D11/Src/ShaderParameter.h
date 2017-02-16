/*
* file ShaderParameter.h
*
* author luoxw
* date 2017/02/14
*
*
*/

#pragma once

namespace D3D11
{
	class FShaderParameter : public LostCore::IShaderParameter
	{
	public:
		FShaderParameter();
		virtual ~FShaderParameter() override;

		virtual bool Construct(LostCore::IRenderContext* rc, uint32 bytes, bool bDynamic) override;
		virtual void UpdateBuffer(LostCore::IRenderContext* rc, const void* buf, uint32 bytes) override;

	protected:
		TRefCountPtr<ID3D11Buffer> Buffer;
		uint32 ByteWidth;
	};
}