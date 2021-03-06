/*
* file PipelineInterface.h
*
* author luoxw
* date 2017/09/21
*
*
*/

#pragma once

#include "RenderObject.h"

namespace D3D11
{
	class LostCore::IPrimitive;

	class IPipeline
	{
	public:
		virtual ~IPipeline() {}

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual EPipeline GetEnum() const = 0;

		virtual void CommitPrimitiveGroup(FPrimitiveGroup* pg) = 0;
		virtual void CommitBuffer(FConstantBuffer* buf) = 0;
		virtual void CommitShaderResource(FTexture2D* tex) = 0;
		virtual void CommitInstancingData(FInstancingData* buf) = 0;

		virtual void BeginFrame() = 0;
		virtual void RenderFrame() = 0;
		virtual void EndFrame() = 0;
	};
}