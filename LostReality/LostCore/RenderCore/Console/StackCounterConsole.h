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
	class FTextSheet;

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
		FTextSheet* Sheet;
		bool bRecordNext;
	};
}

