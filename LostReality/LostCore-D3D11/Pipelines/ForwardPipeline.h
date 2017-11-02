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
		virtual void CommitPrimitiveGroup(FPrimitiveGroup* pg) override;
		virtual void CommitBuffer(FConstantBuffer* buf) override;
		virtual void CommitShaderResource(FTexture2D* tex) override;
		virtual void BeginFrame() override;
		virtual void RenderFrame() override;
		virtual void EndFrame() override;

	protected:

		FRenderObject Committing;

		typedef map<ERenderOrder, vector<FRenderObject>, FRenderOrderComparison> FMap;
		FMap RenderObjects;
	};
}