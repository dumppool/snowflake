/*
* file ForwardPipeline.h
*
* author luoxw
* date 2017/09/21
*
*
*/

#include "PipelineInterface.h"

namespace D3D11
{
	class FPrimitiveGroup;

	class FForwardPipeline : public IPipeline
	{
	public:
		FForwardPipeline();

		// Í¨¹ý IPipeline ¼Ì³Ð
		virtual ~FForwardPipeline() override;

		virtual void Initialize() override;
		virtual void Destroy() override;
		virtual EPipeline GetEnum() const override;
		virtual void CommitPrimitiveGroup(const FPrimitiveGroupPtr& pg) override;
		virtual void CommitBuffer(const FConstantBufferPtr& buf) override;
		virtual void CommitShaderResource(const FTexture2DPtr& tex) override;
		virtual void FinishCommit() override;
		virtual void BeginFrame() override;
		virtual void RenderFrame() override;
		virtual void EndFrame() override;

	protected:

		FRenderObject Committing;

		typedef map<ERenderOrder, vector<FRenderObject>, FRenderOrderComparison> FMap;
		LostCore::TSynchronizer<FMap> RenderObjects;
	};
}