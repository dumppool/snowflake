/*
* file InstancingData.h
*
* author luoxw
* date 2017/12/03
*
*
*/

#pragma once

namespace D3D11
{
	class FInstancingData : public LostCore::IInstancingData
	{
	public:
		MEMORY_ALLOC(FInstancingData);

		FInstancingData();
		FInstancingData(const FInstancingData& rhs) = delete;
		FInstancingData(FInstancingData&& rhs) = delete;
		virtual ~FInstancingData() override;

		// Í¨¹ý IPrimitive¼Ì³Ð
		virtual void Commit() override;
		virtual void SetVertexElement(uint32 flags) override;
		virtual void Update(const void* buf, uint32 sz, uint32 numInstances) override;

		uint32 GetFlags() const;
		TRefCountPtr<ID3D11Buffer> GetBuffer() const;
		uint32 GetNumInstances() const;
		uint32 GetStride() const;

	private:
		uint32 Flags;
		TRefCountPtr<ID3D11Buffer> Buffer;
		uint32 BufferSize;
		uint32 NumInstances;
		uint32 Stride;

	private:
		static void ExecConstructBuffer(void* p, const FBuf& buf);
		static void ExecCommit(void* p);
		static void ExecSetVertexElement(void* p, uint32 flags);
		static void ExecUpdate(void* p, const FBuf& buf, uint32 numInstances);
	};
}