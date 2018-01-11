/*
* file StackCounterConsole.cpp
*
* author luoxw
* date 2017/10/15
*
*
*/

#include "stdafx.h"
#include "StackCounterConsole.h"
#include "RenderCore/UserInterface/FontProvider.h"
#include "RenderCore/UserInterface/TextSheet.h"

using namespace LostCore;

static FStackCounterConsole SObj;

LostCore::FStackCounterConsole::FStackCounterConsole()
	: bInitialized(false)
	, Sheet(nullptr)
	, bRecordNext(false)
	, ActivePageName(IConsole::SInvalid)
{
}

LostCore::FStackCounterConsole::~FStackCounterConsole()
{
	Destroy();
}

std::vector<std::string> LostCore::FStackCounterConsole::GetPageNames() const
{
	return FStackCounterCollector::Get()->GetStackNames();
}

void LostCore::FStackCounterConsole::Initialize(FRect* parent)
{
	ActivePageName = IConsole::SInvalid;
	Sheet = new FTextSheet;
	Sheet->Initialize();
	parent->AddChild(Sheet);
	bInitialized = true;
}

void LostCore::FStackCounterConsole::Refresh()
{
	if (!EnsureInitialized())
	{
		return;
	}

	if (ActivePageName == IConsole::SInvalid)
	{
		return;
	}

	static FStackCounterRequest SCounter("FStackCounterConsole::FinishCounting");
	FScopedStackCounterRequest scopedCounter(SCounter);

	auto info = FStackCounterCollector::Get()->GetStackInfo(ActivePageName);
	auto header = FStackCounter::GetInfoHeader();
	Sheet->SetCaption(info.first);
	Sheet->SetHeader(header);
	Sheet->AddRows(info.second);
}

void LostCore::FStackCounterConsole::Record()
{
	if (!EnsureInitialized())
	{
		return;
	}

	ofstream csv;
	string url("Stack-"), ext(".csv"), output;
	FDirectoryHelper::Get()->GetSpecifiedAbsolutePath("Profile", url.append(GetNowStr(true)).append(ext), output);
	csv.open(output);

	auto pageNames = FStackCounterCollector::Get()->GetStackNames();
	for (auto& name : pageNames)
	{
		auto info = FStackCounterCollector::Get()->GetStackInfo(name);
		csv << info.first << "\n";

		auto header = FStackCounter::GetInfoHeader();
		for (auto& item : header)
		{
			csv << item << ",";
		}
		csv << "\n";

		for (auto& item : info.second)
		{
			for (auto& sub : item)
			{
				csv << sub << ",";
			}
			csv << "\n";
		}
	}

	csv.close();
}

void LostCore::FStackCounterConsole::DisplayPage(const string& name)
{
	if (!EnsureInitialized())
	{
		return;
	}

	auto names = GetPageNames();
	if (find(names.begin(), names.end(), name) == names.end())
	{
		ActivePageName = IConsole::SInvalid;
	}
	else
	{
		ActivePageName = name;
	}
}

void LostCore::FStackCounterConsole::Destroy()
{
	bInitialized = false;
	Sheet->Detach();
	SAFE_DELETE(Sheet);
}

bool LostCore::FStackCounterConsole::EnsureInitialized()
{
	if (!bInitialized && FGUI::Get())
	{
		Initialize(FGUI::Get()->GetRoot());
	}

	return bInitialized;
}

