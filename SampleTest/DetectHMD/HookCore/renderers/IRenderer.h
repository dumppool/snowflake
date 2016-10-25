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
		virtual bool Startup() = 0;
		virtual void Shutdown() = 0;
		virtual bool IsConnected() = 0;

		virtual bool OnPresent_Direct3D11(IDXGISwapChain* swapChain) = 0;
		virtual bool OnPresent_Direct3D9(IDirect3DDevice9* device) = 0;

		virtual const std::string GetDeviceString() const { return "unknown"; }
		virtual std::string GetKeyString() const { return "unknown"; }

		//virtual bool GetDeviceRecommendSize(uint32& Width, uint32& Height) = 0;
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