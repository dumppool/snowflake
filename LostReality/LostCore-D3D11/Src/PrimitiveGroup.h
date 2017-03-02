/*
* file PrimitiveGroup.h
*
* author luoxw
* date 2017/02/10
*
*
*/

#pragma once

namespace D3D11
{
	class FPrimitiveGroup : public LostCore::IPrimitiveGroup
	{
	public:
		FPrimitiveGroup();
		virtual ~FPrimitiveGroup() override;

		// Í¨¹ý IPrimitiveGroup ¼Ì³Ð
		virtual void Draw(LostCore::IRenderContext * rc, float sec) override;
		virtual bool ConstructVB(LostCore::IRenderContext* rc, const void * buf, uint32 bytes, uint32 stride, bool bDynamic) override;
		virtual bool ConstructIB(LostCore::IRenderContext* rc, const void * buf, uint32 bytes, uint32 stride, bool bDynamic) override;
		virtual void SetMaterial(LostCore::IMaterial * mat) override;
		virtual const LostCore::IMaterial * GetMaterial() const override;

	private:
		TRefCountPtr<ID3D11Buffer>			VertexBuffer;
		TRefCountPtr<ID3D11Buffer>			IndexBuffer;
		uint32 VertexBufferSlot;
		uint32 VertexBufferNum;
		uint32 VertexStride;
		uint32 VertexBufferOffset;
		uint32 VertexCount;
		DXGI_FORMAT IndexFormat;
		uint32 IndexBufferOffset;
		uint32 IndexCount;

		LostCore::IMaterial* Material;
	};
}