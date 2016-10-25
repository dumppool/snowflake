#include "HookCorePCH.h"
#include "IRenderer.h"

using namespace lostvr;

LostVR::LostVR() : bAllowConnect(true)
{
	VRDevices.clear();
}

LostVR::~LostVR()
{
	for (auto dev : VRDevices)
	{
		if (dev.second != nullptr)
		{
			delete dev.second;
			dev.second = nullptr;
		}
	}

	VRDevices.clear();
}

void LostVR::RegisterVRDevice(const std::string& InKey, IVRDevice* InInst)
{
	auto it = VRDevices.find(InKey);
	if (VRDevices.end() != it)
	{
		LVMSG("LostVR::RegisterVRDevice", "unexpected registration on duplicate key: %s", InKey.c_str());
	}

	VRDevices[InKey] = InInst;
}

IVRDevice* LostVR::GetDevice()
{
	for (auto dev : VRDevices)
	{
		if (dev.second->IsConnected())
		{
			return dev.second;
		}

		if (bAllowConnect && dev.second->Startup())
		{
			bAllowConnect = false;
			LVMSG("LostVR::GetDevice", "connected to vr device: %s", dev.second->GetDeviceString().c_str());
			return dev.second;
		}
	}

	return nullptr;
}

IVRDevice* LostVR::GetDevice(const std::string& InKey) const
{
	auto it = VRDevices.find(InKey);
	if (it == VRDevices.end())
	{
		return nullptr;
	}

	return it->second;
}

void LostVR::OnPresent_Direct3D11(IDXGISwapChain* swapChain)
{
	auto dev = GetDevice();
	if (dev == nullptr)
	{
		bAllowConnect = true;
		return;
	}

	dev->OnPresent_Direct3D11(swapChain);
}

void LostVR::OnPresent_Direct3D9(IDirect3DDevice9* device)
{
	auto dev = GetDevice();
	if (dev == nullptr)
	{
		bAllowConnect = true;
		return;
	}

	dev->OnPresent_Direct3D9(device);
}

IVRDevice::IVRDevice()
{
	LostVR::Get()->RegisterVRDevice(GetKeyString(), this);
}
