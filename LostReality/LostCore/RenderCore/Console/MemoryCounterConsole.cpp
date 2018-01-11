/*
* file MemoryCounterConsole.cpp
*
* author luoxw
* date 2018/01/01
*
*
*/

#include "stdafx.h"
#include "MemoryCounterConsole.h"
#include "RenderCore/UserInterface/TextSheet.h"

using namespace LostCore;

static FMemoryCounterConsole SObj;
static string SName("Memory");

LostCore::FMemoryCounterConsole::FMemoryCounterConsole()
	: bInitialized(false)
	, Sheet(nullptr)
	, bRecordNext(false)
	, ActivePageName(IConsole::SInvalid)
{

}

LostCore::FMemoryCounterConsole::~FMemoryCounterConsole()
{
	Destroy();
}

std::vector<std::string> LostCore::FMemoryCounterConsole::GetPageNames() const
{
	return vector<string>({ SName });
}

void LostCore::FMemoryCounterConsole::Refresh()
{
	if (!EnsureInitialized())
	{
		return;
	}

	if (ActivePageName != SName)
	{
		return;
	}

	auto head = FMemoryCounterManager::GetInfoHeader();
	Sheet->SetHeader(head);

	auto info = FMemoryCounterManager::Get()->GetMemoryUsage();
	for (auto& item : info)
	{
		vector<string> str;
		str.push_back(item.first);
		str.push_back(to_string(std::get<0>(item.second)));
		str.push_back(to_string(std::get<1>(item.second)));
		Sheet->AddRow(str);
	}
}

void LostCore::FMemoryCounterConsole::Record()
{
	if (!EnsureInitialized())
	{
		return;
	}

	ofstream csv;
	string url("Mem-"), ext(".csv"), output;
	FDirectoryHelper::Get()->GetSpecifiedAbsolutePath("Profile", url.append(GetNowStr(true)).append(ext), output);
	csv.open(output);

	auto head = FMemoryCounterManager::GetInfoHeader();
	for (auto& item : head)
	{
		csv << item << ",";
	}
	csv << "\n";

	auto info = FMemoryCounterManager::Get()->GetMemoryUsage();
	for (auto& item : info)
	{
		csv << item.first << ","
			<< std::get<0>(item.second) << "," 
			<< std::get<1>(item.second) << "\n";
	}

	csv.close();
}

void LostCore::FMemoryCounterConsole::DisplayPage(const string& name)
{
	ActivePageName = name;
}

void LostCore::FMemoryCounterConsole::Initialize(FRect* parent)
{
	ActivePageName = IConsole::SInvalid;
	Sheet = new FTextSheet;
	Sheet->Initialize();
	parent->AddChild(Sheet);
	bInitialized = true;
}

void LostCore::FMemoryCounterConsole::Destroy()
{
	bInitialized = false;
	Sheet->Detach();
	SAFE_DELETE(Sheet);
}

bool LostCore::FMemoryCounterConsole::EnsureInitialized()
{
	if (!bInitialized && FGUI::Get() != nullptr)
	{
		Initialize(FGUI::Get()->GetRoot());
	}

	return bInitialized;
}
