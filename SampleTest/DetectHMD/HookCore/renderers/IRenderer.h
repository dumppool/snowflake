/*
 * file IRenderer.h
 *
 * author luoxw
 * date 2016/11/10
 *
 * 
 */
#pragma once

#include <map>

class FInputEventHandler;

namespace lostvr
{
	typedef int EEye;

	enum class EMovement : uint8
	{
		UnDefined,
		CameraFront,
		CameraBack,
	};

	class IVRDevice
	{
		std::string Key;

	public:
		explicit IVRDevice(const std::string& key);

		//virtual ~IVRDevice() = 0;

		//************************************
		// Method:    Startup �����豸����γ��������豸�ǵ����ߵ�����
		// FullName:  lostvr::IVRDevice::Startup
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool Startup() = 0;

		//************************************
		// Method:    Shutdown �Ͽ����ӣ������ڲ�״̬
		// FullName:  lostvr::IVRDevice::Shutdown
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		//************************************
		virtual void Shutdown() = 0;

		//************************************
		// Method:    IsConnected �Ƿ�����VR�豸
		// FullName:  lostvr::IVRDevice::IsConnected
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		//************************************
		virtual bool IsConnected() = 0;

		//************************************
		// Method:    OnPresent_Direct3D11 IDXGISwapChain::Present�ĵ������
		// FullName:  lostvr::IVRDevice::OnPresent_Direct3D11
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: IDXGISwapChain * swapChain
		//************************************
		virtual bool OnPresent_Direct3D11(IDXGISwapChain* swapChain) = 0;

		//************************************
		// Method:    OnPresent_Direct3D9 IDirect3DDevice9::Present�ĵ������,
		//			  �Լ��������ṩIDirect3DDevice9����ĵ������
		// FullName:  lostvr::IVRDevice::OnPresent_Direct3D9
		// Access:    virtual public 
		// Returns:   bool
		// Qualifier:
		// Parameter: IDirect3DDevice9 * device
		//************************************
		virtual bool OnPresent_Direct3D9(IDirect3DDevice9* device) = 0;

		virtual const std::string GetDeviceString() const { return "unknown"; }
		const std::string& GetKeyString() const { return Key; }

		//************************************
		// Method:    AddMovement
		// FullName:  lostvr::IVRDevice::AddMovement
		// Access:    virtual public 
		// Returns:   void
		// Qualifier:
		// Parameter: EMovement movement
		//************************************
		virtual void AddMovement(EMovement movement) = 0;
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
		void OnPresent_Direct3D9Ex(IDirect3DDevice9Ex* device);

		void SetupInput();

	protected:
		typedef std::map<const std::string, IVRDevice*> KeyDeviceMap;
		KeyDeviceMap VRDevices;

		bool bAllowConnect;

		FInputEventHandler* InputHandler;
	};
}