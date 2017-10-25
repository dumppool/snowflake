/*
* file StackCounterConsole.h
*
* author luoxw
* date 2017/10/15
*
*
*/

#pragma once

namespace LostCore
{
	class FListBox;
	class FTextBox;

	class FStackCounterConsole
	{
	public:
		FStackCounterConsole();
		~FStackCounterConsole();

		void Initialize(FRect* parent);
		void Destroy();

		void FinishCounting();
		void FinishDisplay();

		void Record();

	private:
		FTextBox* AllocTextBox();

		FListBox* Panel0;
		FListBox* Panel1;
		FListBox* Panel2;

		vector<FTextBox*> OnlineText;
		vector<FTextBox*> OfflineText;

		bool bRecordNext;
	};
}

