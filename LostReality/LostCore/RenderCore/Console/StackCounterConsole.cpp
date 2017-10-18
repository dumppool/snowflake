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
#include "UserInterface/TextBox.h"
#include "UserInterface/ListBox.h"

using namespace LostCore;

LostCore::FStackCounterConsole::FStackCounterConsole()
{
}

LostCore::FStackCounterConsole::~FStackCounterConsole()
{
	Destroy();
}

void LostCore::FStackCounterConsole::Initialize(FRect* parent)
{
	Panel = new FListBox;
	Panel->SetOriginLocal(FFloat2(0.0f, 0.0f));
	Panel->SetSize(parent->GetSize());
	parent->AddChild(Panel);
}

void LostCore::FStackCounterConsole::FinishCounting()
{
	FStackCounterManager::Get()->Finish();
	auto content = FStackCounterManager::Get()->GetDisplayContent();
	for (auto item : content)
	{
		auto temp = UTF8ToWide(item);
		auto tb = AllocTextBox();
		tb->SetText(temp);
		Panel->AddChild(tb);
	}
}

void LostCore::FStackCounterConsole::FinishDisplay()
{
	FStackCounterManager::Get()->EndFrame();
	for (auto item : OnlineText)
	{
		item->Detach();
		OfflineText.push_back(item);
	}

	OnlineText.clear();
}

FTextBox * LostCore::FStackCounterConsole::AllocTextBox()
{
	FTextBox* tb = nullptr;
	if (OfflineText.size() == 0)
	{
		tb = new FTextBox;
	}
	else
	{
		tb = OfflineText.back();
		OfflineText.pop_back();
	}

	OnlineText.push_back(tb);
	return tb;
}

void LostCore::FStackCounterConsole::Destroy()
{
	for (auto item : OnlineText)
	{
		item->Detach();
		SAFE_DELETE(item);
	}

	OnlineText.clear();

	for (auto item : OfflineText)
	{
		SAFE_DELETE(item);
	}

	OfflineText.clear();

	Panel->Detach();
	SAFE_DELETE(Panel);
}
