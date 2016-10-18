#include "HookCorePCH.h"
#include "IVRDevice.h"

using namespace lostvr;

VRDeviceModule::VRDeviceModule() : CurrentDevice(nullptr)
{
	//Candidates.push_back()
}

VRDeviceModule::~VRDeviceModule()
{
	for (auto Device : Candidates)
	{
		delete Device;
		Device = nullptr;
	}

	delete CurrentDevice;
	CurrentDevice = nullptr;
}

bool VRDeviceModule::IsConnected()
{
	return CurrentDevice != nullptr;
}

bool VRDeviceModule::Startup()
{
	for (auto Device : Candidates)
	{
		if (Device->InitializeVR())
		{

		}
	}

	return false;
}

void VRDeviceModule::Shutdown()
{

}

IVRDevice* VRDeviceModule::GetVRDevice()
{
	return nullptr;
}

