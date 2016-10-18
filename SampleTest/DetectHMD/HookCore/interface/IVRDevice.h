#pragma once

#include "IRenderer.h"

namespace lostvr
{
	class IVRDevice
	{
	public:
		virtual bool InitializeVR() = 0;
		virtual void DestroyVR() = 0;
		virtual void Submit(IRenderer* InRenderer, IDXGISwapChain* InSwapChain) = 0;
	};

	class VRDeviceModule
	{
	public:
		VRDeviceModule();
		~VRDeviceModule();

		bool IsConnected();
		bool Startup();
		void Shutdown();
		IVRDevice* GetVRDevice();

		static VRDeviceModule* Get()
		{
			static VRDeviceModule Inst;
			return &Inst;
		}

	protected:
		IVRDevice* CurrentDevice;
		std::vector<IVRDevice*> Candidates;
	};
}