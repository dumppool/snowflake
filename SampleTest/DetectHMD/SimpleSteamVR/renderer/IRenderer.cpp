#include "HookCorePCH.h"
#include "IRenderer.h"

using namespace lostvr;

LostVR::LostVR()
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

IVRDevice* LostVR::GetDevice() const
{
	for (auto dev : VRDevices)
	{
		if (dev.second->IsConnected())
		{
			return dev.second;
		}

		if (dev.second->Startup())
		{
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
