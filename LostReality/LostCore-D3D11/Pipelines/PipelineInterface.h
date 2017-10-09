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

		virtual void CommitPrimitiveGroup(FPrimitiveGroup* pg) = 0;
		virtual void CommitBuffer(FConstantBuffer* buf) = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual void Render() = 0;
	};
}