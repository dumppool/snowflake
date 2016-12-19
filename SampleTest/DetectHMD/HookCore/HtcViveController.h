/*
* file HtcViveController.h
*
* author luoxw
* date 2016/12/19
*
*
*/

#pragma once

#include "openvr-lib/headers/openvr.h"

namespace openvrcon
{
	enum class EControllerHand : uint8
	{
		Left,
		Right,
		TotalHandCount,
	};

	enum class EControllerButton : uint8
	{
		System,
		ApplicationMenu,
		TouchPadPress,
		TouchPadTouch,
		Trigger,
		Grip,
		TouchPadUp,
		TouchPadDown,
		TouchPadLeft,
		TouchPadRight,

		/** Max number of controller buttons.  Must be < 256 */
		TotalButtonCount,
	};

	struct FControllerState
	{
		/** Which hand this controller is representing */
		//EControllerHand Hand;

		/** If packet num matches that on your prior call, then the controller state hasn't been changed since
		* your last call and there is no need to process it. */
		uint32 PacketNum;

		/** touchpad analog values */
		float TouchPadXAnalog;
		float TouchPadYAnalog;

		/** trigger analog value */
		float TriggerAnalog;

		/** Last frame's button states, so we only send events on edges */
		bool ButtonStates[(int32)EControllerButton::TotalButtonCount];

		/** Next time a repeat event should be generated for each button */
		double NextRepeatTime[(int32)EControllerButton::TotalButtonCount];

		/** Value for force feedback on this controller hand */
		float ForceFeedbackValue;

		FControllerState();
	};

	class IVRControllerCallback
	{
	public:
		virtual vr::IVRSystem* GetSystem() = 0;
		virtual void OnAction(EControllerHand hand, EControllerButton id, bool released) = 0;
		virtual void OnAxis(EControllerHand hand, EControllerButton id, float value) = 0;
	};

	class FHtcViveController
	{
	public:
		FHtcViveController();
		~FHtcViveController();

		void InstallController(IVRControllerCallback* callback);
		void UninstallController(IVRControllerCallback* callback);

		void Update();

	protected:
		FControllerState			ControllerStats[(int32)EControllerHand::TotalHandCount];
		IVRControllerCallback*		CallbackObject;
	};
}