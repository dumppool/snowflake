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

	struct FConstantBuffer
	{
		FConstantBuffer(int32 byteWidth, bool dynamic, int32 slot)
			: ByteWidth(byteWidth)
			, bDynamic(dynamic)
			, Slot(slot)
			, Buffer(nullptr)
		{
		}

		~FConstantBuffer()
		{
			Buffer = nullptr;
		}

		bool Initialize(const TRefCountPtr<ID3D11Device>& device);
		void UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void* buf, int32 sz);
		void Bind(const TRefCountPtr<ID3D11DeviceContext>& cxt);
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
		int32		Slot;
		TRefCountPtr<ID3D11Buffer> Buffer;
	};

	/*
	cbuffer ViewProject : register(b0)
	{
		float ScreenWidth;
		float ScreenHeight;
		float ScreenWidthRcp;
		float ScreenHeightRcp;
		float4x4 ViewProject;
	};
	*/
	struct FConstantBufferRegister0 : public FConstantBuffer
	{
		struct FParam
		{
			float ScreenWidth;
			float ScreenHeight;
			float ScreenWidthRcp;
			float ScreenHeightRcp;
			LostCore::FMatrix ViewProject;
		};

		FConstantBufferRegister0() : FConstantBuffer(sizeof(FParam), false, 0)
		{
		}

		void UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void* buf, int32 sz);
	};

	struct FConstantBufferMatrix : public FConstantBuffer
	{
		struct FParam
		{
			LostCore::FMatrix Mat;
		};

		FConstantBufferMatrix() : FConstantBuffer(sizeof(FParam), false, 1)
		{
		}

		FConstantBufferMatrix(bool dynamic, int32 slot) : FConstantBuffer(sizeof(FParam), dynamic, slot)
		{
		}

		void UpdateBuffer(const TRefCountPtr<ID3D11DeviceContext>& cxt, const void* buf, int32 sz);
	};

	struct FConstantBufferUIRect : public FConstantBuffer
	{
		FConstantBufferUIRect() : FConstantBuffer(sizeof(float) * 8, false, 1)
		{
		}

		FConstantBufferUIRect(bool dynamic, int32 slot) : FConstantBuffer(sizeof(float) * 8, dynamic, slot)
		{
		}
	};
}