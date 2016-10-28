#pragma once

#include <map>

namespace lostvr
{
	typedef int EEye;

	class IVRDevice
	{
	public:
		IVRDevice();

		//virtual ~IVRDevice() = 0;

		//************************************
		// Method:    Startup 连接设备，多次尝试连接设备是调用者的责任
		// FullName:  lostvr::IVRDevice::Startup
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool Startup() = 0;

		//************************************
		// Method:    Shutdown 断开连接，重置内部状态
		// FullName:  lostvr::IVRDevice::Shutdown
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		//************************************
		virtual void Shutdown() = 0;

		//************************************
		// Method:    IsConnected 是否连接VR设备
		// FullName:  lostvr::IVRDevice::IsConnected
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool IsConnected() = 0;

		//************************************
		// Method:    OnPresent_Direct3D11 IDXGISwapChain::Present的调用入口
		// FullName:  lostvr::IVRDevice::OnPresent_Direct3D11
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: IDXGISwapChain * swapChain
		//************************************
		virtual bool OnPresent_Direct3D11(IDXGISwapChain* swapChain) = 0;

		//************************************
		// Method:    OnPresent_Direct3D9 IDirect3DDevice9::Present的调用入口,
		//			  以及其他能提供IDirect3DDevice9对象的调用入口
		// FullName:  lostvr::IVRDevice::OnPresent_Direct3D9
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: IDirect3DDevice9 * device
		//************************************
		virtual bool OnPresent_Direct3D9(IDirect3DDevice9* device) = 0;

		virtual const std::string GetDeviceString() const { return "unknown"; }
		virtual std::string GetKeyString() const { return "[unknown]"; }

	};

	class LostVR
	{
	public:
		static LostVR* Get()
		{
			static LostVR Inst;
			return &Inst;
		}

		LostVR();
		~LostVR();

		void RegisterVRDevice(const std::string& InKey, IVRDevice* InInst);

		IVRDevice* GetDevice();
		IVRDevice* GetDevice(const std::string& InKey) const;

		void OnPresent_Direct3D11(IDXGISwapChain* swapChain);
		void OnPresent_Direct3D9(IDirect3DDevice9* device);

	protected:
		typedef std::map<const std::string, IVRDevice*> KeyDeviceMap;
		KeyDeviceMap VRDevices;

		bool bAllowConnect;
	};
}