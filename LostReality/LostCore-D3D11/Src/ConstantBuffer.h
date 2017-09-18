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

	struct FConstantBuffer : public LostCore::IConstantBuffer
	{
		FConstantBuffer()
		{
		}

		virtual ~FConstantBuffer() override
		{
			Buffer = nullptr;
		}

		virtual bool Initialize(LostCore::IRenderContext * rc, int32 byteWidth, bool dynamic) override;
		virtual void UpdateBuffer(LostCore::IRenderContext * rc, const void* buf, int32 sz) override;
		virtual void Bind(LostCore::IRenderContext* rc, int32 slot);

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
	};
}