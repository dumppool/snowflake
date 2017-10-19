/*
* file TextBox.h
*
* author luoxw
* date 2017/10/15
*
*
*/

#pragma once

#include "Inc/BasicGUI.h"

namespace LostCore
{
	class FListBox : public FRect
	{
	public:
		enum class EAlignment : uint8
		{
			Vertical,
			Horizontal,
		};

		FListBox();

		virtual void Update() override;

		void SetAlignment(EAlignment alignment);
		void SetSpace(int32 val);

	private:
		EAlignment Alignment;
		int32 Space;
	};
}

