/*
* file ConsoleInterface.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#pragma once

namespace LostCore
{
	class IConsole
	{
	public:
		IConsole();
		virtual ~IConsole();

		virtual vector<string> GetPageNames() const = 0;
		virtual void Refresh() = 0;
		virtual void Record() = 0;
		virtual void DisplayPage(const string& name) = 0;

	public:
		static vector<string> GetConsoleNames();
		static void DisplayConsole(const string& name);
		static void HideAll();
		static void RefreshConsole();
		static void RecordConsole();
		static void AddConsole(IConsole* console);
		static void DelConsole(IConsole* console);
		static void Clear();

		static vector<IConsole*> SConsoles;
		static string SInvalid;
	};

}

