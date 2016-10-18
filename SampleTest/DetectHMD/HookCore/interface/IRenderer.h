#pragma once

namespace lostvr
{
	class IRenderer
	{
	public:
		//************************************
		// Method:    Init 初始化，D3D11版本
		// FullName:  lostvr::IRenderer::Init
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: IDXGISwapChain * InSwapChain
		//************************************
		virtual bool Init(IDXGISwapChain* InSwapChain) {}

		//************************************
		// Method:    Fini 重置所有成员
		// FullName:  lostvr::IRenderer::Fini
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		//************************************
		virtual void Fini() = 0;

		//************************************
		// Method:    OnPresent 渲染更新VR设备显示所需的资源
		// FullName:  lostvr::IRenderer::OnPresent
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: IDXGISwapChain * InSwapChain
		//************************************
		virtual void OnPresent(IDXGISwapChain* InSwapChain) = 0;

		virtual std::string GetRendererID() const = 0;
	};
}