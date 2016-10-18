#pragma once

namespace lostvr
{
	class IRenderer
	{
	public:
		//************************************
		// Method:    Init ��ʼ����D3D11�汾
		// FullName:  lostvr::IRenderer::Init
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: IDXGISwapChain * InSwapChain
		//************************************
		virtual bool Init(IDXGISwapChain* InSwapChain) {}

		//************************************
		// Method:    Fini �������г�Ա
		// FullName:  lostvr::IRenderer::Fini
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		//************************************
		virtual void Fini() = 0;

		//************************************
		// Method:    OnPresent ��Ⱦ����VR�豸��ʾ�������Դ
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