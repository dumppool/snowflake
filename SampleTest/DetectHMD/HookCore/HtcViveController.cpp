/*
* file HtcViveController.cpp
*
* author luoxw
* date 2016/12/19
*
*
*/

#include "HookCorePCH.h"
#include "HtcViveController.h"

using namespace openvrcon;

FControllerState::FControllerState() :
PacketNum(0),
TouchPadXAnalog(0.0f),
TouchPadYAnalog(0.0f),
TriggerAnalog(0.0f),
ForceFeedbackValue(0.0f)
{
	memset(ButtonStates, 0, sizeof(bool) * (int)EControllerButton::TotalButtonCount);
	memset(NextRepeatTime, 0, sizeof(double) * (int)EControllerButton::TotalButtonCount);
}

FHtcViveController::FHtcViveController() : CallbackObject(nullptr)
{
}

FHtcViveController::~FHtcViveController()
{
}

void FHtcViveController::InstallController(IVRControllerCallback * callback)
{
}

void FHtcViveController::UninstallController(IVRControllerCallback * callback)
{
}

void FHtcViveController::Update()
{
	vr::IVRSystem* sys = nullptr;
	if (CallbackObject == nullptr || (sys = CallbackObject->GetSystem()) == nullptr)
	{
		return;
	}


}
