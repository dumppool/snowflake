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
	class FForwardPipeline : public IPipeline
	{
	public:
		// Í¨¹ý IPipeline ¼Ì³Ð
		virtual ~FForwardPipeline() override;

		virtual void AddRenderObject() override;
		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void RenderOpcity() override;
		virtual void RenderMasked() override;
		virtual void RenderTranslucent() override;
		virtual void RenderPostProcess() override;
	};
}