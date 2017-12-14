/*
* file PrimitiveGroupRequest.h
*
* author luoxw
* date 2017/11/01
*
*
*/

#pragma once

namespace D3D11
{
	class FPrimitiveGroup : public LostCore::IPrimitive
	{
	public:
		FPrimitiveGroup();
		FPrimitiveGroup(const FPrimitiveGroup& rhs);
		FPrimitiveGroup(const FPrimitiveGroup&& ref);
		virtual ~FPrimitiveGroup() override;

		// Í¨¹ý IPrimitive¼Ì³Ð
		virtual void Commit() override;
		virtual void SetVertexElement(uint32 flags) override;
		virtual uint32 GetFlags() const override;
		virtual void SetRenderOrder(ERenderOrder ro) override;
		virtual ERenderOrder GetRenderOrder() const override;
		virtual void ConstructVB(const void* buf, uint32 sz, uint32 stride, bool dynamic) override;
		virtual void ConstructIB(const FBuf& buf, uint32 stride, bool dynamic) override;
		virtual void SetTopology(LostCore::EPrimitiveTopology topo) override;
		virtual void UpdateVB(const void* buf, uint32 sz, uint32 stride) override;

		void Draw(const vector<FInstancingData*>& batch);

		TRefCountPtr<ID3D11Buffer> GetVertexBuffer();

	private:
		uint32 Flags;

		TRefCountPtr<ID3D11Buffer>			VertexBuffer;
		uint32 Stride;
		uint32 Count;
		bool bIsVBDynamic;

		TRefCountPtr<ID3D11Buffer>			IndexBuffer;
		uint32 IndexCount;
		DXGI_FORMAT IndexFormat;
		bool bIsIBDynamic;

		ERenderOrder RenderOrder;
		D3D11_PRIMITIVE_TOPOLOGY Topology;

	private:
		static void ExecCommit(void* p);
		static void ExecConstructVB(void* p, const FBuf& buf, uint32 stride, bool bDynamic);
		static void ExecConstructIB(void* p, const FBuf& buf, uint32 stride, bool bDynamic);
		static void ExecUpdateVB(void* p, const FBuf& buf, uint32 stride);
	};
}