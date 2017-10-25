/*
* file ConstantBuffer.h
*
* author luoxw
* date 2017/06/13
*
*
*/

#pragma once

namespace D3D11
{
	/********************************************************
	Constant buffer
	*/

	class FConstantBuffer : public LostCore::IConstantBuffer
	{
	public:
		FConstantBuffer();
		virtual ~FConstantBuffer() override;

		virtual bool Initialize(int32 byteWidth, bool dynamic) override;
		virtual void UpdateBuffer(const FBufFast& buf) override;
		virtual void Commit() override;

		virtual void SetShaderSlot(int32 slot) override;
		virtual int32 GetShaderSlot() const override;

		virtual void SetShaderFlags(int32 flags) override;
		virtual int32 GetShaderFlags() const override;

		void Bind();

		int32 GetByteWidth() const
		{
			return ByteWidth;
		}

		TRefCountPtr<ID3D11Buffer> GetBufferRHI()
		{
			return Buffer;
		}

	private:
		int32		ByteWidth;
		bool		bDynamic;
		TRefCountPtr<ID3D11Buffer> Buffer;
		int32		ShaderSlot;
		int32		ShaderFlags;
	};
}