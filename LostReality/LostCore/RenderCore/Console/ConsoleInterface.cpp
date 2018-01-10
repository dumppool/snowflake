/*
* file ConsoleInterface.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#include "stdafx.h"
#include "ConsoleInterface.h"

using namespace LostCore;


LostCore::IConsole::IConsole()
{
	IConsole::AddConsole(this);
}

LostCore::IConsole::~IConsole()
{
	IConsole::DelConsole(this);
}

vector<string> LostCore::IConsole::GetConsoleNames()
{
	vector<string> result;
	for (auto item : SConsoles)
	{
		auto names = item->GetPageNames();
		result.insert(result.end(), names.begin(), names.end());
	}

	return result;
}

void LostCore::IConsole::DisplayConsole(const string& name)
{
	for (auto item : SConsoles)
	{
		item->DisplayPage(name);
	}
}

void LostCore::IConsole::HideAll()
{
	for (auto item : SConsoles)
	{
		item->DisplayPage(SInvalid);
	}
}

void LostCore::IConsole::RefreshConsole()
{
	for (auto item : SConsoles)
	{
		item->Refresh();
	}
}

void LostCore::IConsole::RecordConsole()
{
	for (auto item : SConsoles)
	{
		item->Record();
	}
}

void LostCore::IConsole::AddConsole(IConsole* console)
{
	SConsoles.push_back(console);
}

void LostCore::IConsole::DelConsole(IConsole* console)
{
	for (auto it = SConsoles.begin(); it != SConsoles.end(); ++it)
	{
		if (*it == console)
		{
			SConsoles.erase(it);
			break;
		}
	}
}

void LostCore::IConsole::Clear()
{
	SConsoles.clear();
}

std::vector<IConsole*> LostCore::IConsole::SConsoles;
std::string LostCore::IConsole::SInvalid = "Empty";
