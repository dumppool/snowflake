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
		virtual void Commit() override;
		virtual void SetVertexElement(uint32 flags) override;
		virtual uint32 GetVertexElement() const override;
		virtual void SetRenderOrder(ERenderOrder ro) override;
		virtual ERenderOrder GetRenderOrder() const override;
		virtual bool ConstructVB(const void * buf, uint32 bytes, uint32 stride, bool bDynamic) override;
		virtual bool ConstructIB(const void * buf, uint32 bytes, uint32 stride, bool bDynamic) override;
		virtual void SetTopology(LostCore::EPrimitiveTopology topo) override;
		virtual void UpdateVB(const void* buf, uint32 bytes) override;

		void Draw();

	private:
		uint32 VertexElement;

		TRefCountPtr<ID3D11Buffer>			VertexBuffer;
		uint32 VertexBufferSlot;
		uint32 VertexBufferNum;
		uint32 VertexStride;
		uint32 VertexBufferOffset;
		uint32 VertexCount;
		bool bIsVBDynamic;

		TRefCountPtr<ID3D11Buffer>			IndexBuffer;
		uint32 IndexBufferOffset;
		uint32 IndexCount;
		DXGI_FORMAT IndexFormat;
		bool bIsIBDynamic;

		ERenderOrder RenderOrder;
		D3D11_PRIMITIVE_TOPOLOGY Topology;
	};
}