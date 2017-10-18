/*
* file TextBox.h
*
* author luoxw
* date 2017/10/15
*
*
*/

#pragma once

#include "FontInterface.h"
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

		virtual void Commit() override;

	private:
		EAlignment Alignment;
	};
}

