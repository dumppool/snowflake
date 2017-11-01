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
	class LostCore::IPrimitiveGroup;

	class IPipeline
	{
	public:
		virtual ~IPipeline() {}

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual EPipeline GetEnum() const = 0;

		virtual void CommitPrimitiveGroup(const FPrimitiveGroupPtr& pg) = 0;
		virtual void CommitBuffer(const FConstantBufferPtr& buf) = 0;
		virtual void CommitShaderResource(const FTexture2DPtr& tex) = 0;
		virtual void FinishCommit() = 0;

		virtual void BeginFrame() = 0;
		virtual void RenderFrame() = 0;
		virtual void EndFrame() = 0;
	};
}