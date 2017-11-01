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
	class FPrimitiveGroup : public LostCore::IPrimitiveGroup, public enable_shared_from_this<FPrimitiveGroup>
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
		virtual void ConstructVB(const FBuf& buf, uint32 stride, bool bDynamic) override;
		virtual void ConstructIB(const FBuf& buf, uint32 stride, bool bDynamic) override;
		virtual void SetTopology(LostCore::EPrimitiveTopology topo) override;
		virtual void UpdateVB(const FBuf& buf) override;

		void Draw();

	private:
		void ExecConstructVB(const FBuf& buf, uint32 stride, bool bDynamic);
		void ExecConstructIB(const FBuf& buf, uint32 stride, bool bDynamic);
		void ExecUpdateVB(const FBuf& buf);

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

	typedef shared_ptr<FPrimitiveGroup> FPrimitiveGroupPtr;
	typedef weak_ptr<FPrimitiveGroup> FPrimitiveGroupWeakPtr;
}