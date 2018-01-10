/*
* file StackCounterConsole.h
*
* author luoxw
* date 2017/10/15
*
*
*/

#pragma once
#include "ConsoleInterface.h"

namespace LostCore
{
	class FRect;
	class FTextSheet;

	class FStackCounterConsole : public IConsole
	{
	public:
		FStackCounterConsole();
		virtual ~FStackCounterConsole() override;

		virtual vector<string> GetPageNames() const override;
		virtual void Refresh() override;
		virtual void Record() override;
		virtual void DisplayPage(const string& name)  override;

		void Initialize(FRect* parent);
		void Destroy();

	private:
		bool EnsureInitialized();

		bool bInitialized;
		FTextSheet* Sheet;
		bool bRecordNext;
		string ActivePageName;
	};
}

