/*
* file MemoryCounterConsole.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#pragma once
#include "ConsoleInterface.h"

namespace LostCore
{
	class FRect;
	class FTextSheet;

	class FMemoryCounterConsole : public IConsole
	{
	public:
		FMemoryCounterConsole();
		~FMemoryCounterConsole();

		virtual vector<string> GetPageNames() const override;
		virtual void Refresh() override;
		virtual void Record() override;
		virtual void DisplayPage(const string& name) override;

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

