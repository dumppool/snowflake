#pragma once
#include "interface/IRenderer.h"

namespace lostvr
{
	class MonoscopicRenderer : public IRenderer
	{
	public:
		virtual bool Init(IDXGISwapChain* InSwapChain) override;
		virtual void Fini() override;
		virtual bool OnPresent(IDXGISwapChain* InSwapChain) override;
		virtual std::string GetRendererID() const
		{
			return std::string("Monoscopic");
		}
	};
}