/*
* file PipelineInterface.h
*
* author luoxw
* date 2017/09/21
*
*
*/

#pragma once

namespace D3D11
{
	class IPipeline
	{
	public:
		virtual ~IPipeline() {}

		virtual void AddRenderObject() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void RenderOpcity() = 0;
		virtual void RenderMasked() = 0;
		virtual void RenderTranslucent() = 0;
		virtual void RenderPostProcess() = 0;
	};
}