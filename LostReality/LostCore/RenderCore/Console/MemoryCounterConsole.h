/*
* file MemoryCounterConsole.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#pragma once

namespace LostCore
{
	class FRect;
	class FTextSheet;

	class FMemoryCounterConsole
	{
	public:
		FMemoryCounterConsole();
		~FMemoryCounterConsole();

		void Initialize(FRect* parent);
		void Destroy();
		void Collect();
		void Record();

	private:
		FTextSheet* Sheet;
		bool bRecordNext;
	};
}

