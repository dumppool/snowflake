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
#include "RenderCore/UserInterface/TextBox.h"
#include "RenderCore/UserInterface/ListBox.h"

using namespace LostCore;

LostCore::FStackCounterConsole::FStackCounterConsole()
	: Panel0(nullptr)
	, Panel1(nullptr)
	, Panel2(nullptr)
	, bRecordNext(false)
{
}

LostCore::FStackCounterConsole::~FStackCounterConsole()
{
	Destroy();
}

void LostCore::FStackCounterConsole::Initialize(FRect* parent)
{
	auto parentSize = parent->GetSize();

	Panel0 = new FListBox;
	Panel0->SetOffsetLocal(FFloat2(0.0f, 0.0f));
	Panel0->SetAlignment(FListBox::EAlignment::Horizontal);
	Panel0->SetSpace(10);
	parent->AddChild(Panel0);

	Panel1 = new FListBox;
	Panel1->SetAlignment(FListBox::EAlignment::Vertical);
	Panel1->SetAutoUpdateHeight(false);
	Panel1->SetSize(FFloat2(parentSize.X*0.3f, parentSize.Y));
	Panel0->AddChild(Panel1);

	Panel2 = new FListBox;
	Panel2->SetAlignment(FListBox::EAlignment::Vertical);
	Panel2->SetAutoUpdateHeight(false);
	Panel2->SetSize(FFloat2(parentSize.X*0.6f, parentSize.Y));
	Panel0->AddChild(Panel2);
}

void LostCore::FStackCounterConsole::FinishCounting()
{
	FStackCounterManager::Get()->Finish();

	//return;
	static FStackCounterRequest SCounter("FStackCounterConsole::FinishCounting");
	FScopedStackCounterRequest scopedCounter(SCounter);

	auto tb = AllocTextBox();
	tb->SetText(UTF8ToWide(string("Thread: ").append(FProcessUnique::Get()->GetCurrentThread()->GetName())));
	Panel1->AddChild(tb);

	char head[128];
	memset(head, 0, 128);
	auto frameTime = FGlobalHandler::Get()->GetFrameTime();
	snprintf(head, 127, "%.1fFPS %.2fms", 1.0f / frameTime, 1000 * frameTime);
	tb = AllocTextBox();
	tb->SetText(UTF8ToWide(head));
	Panel2->AddChild(tb);

	ofstream stream;
	if (bRecordNext)
	{
		string url("StackStatics-"), ext(".csv"), output;
		FDirectoryHelper::Get()->GetSpecifiedAbsolutePath("Profile", url.append(GetNowStr(true)).append(ext), output);
		stream.open(output);
		stream << "Thread: " << FProcessUnique::Get()->GetCurrentThread()->GetName() << "\n";
		stream << "FrameTime: " << to_string(1000 * frameTime) << "ms, FPS: " << to_string(1/frameTime) << "\n";
	}

	auto content = FStackCounterManager::Get()->GetDisplayContent();
	for (auto item : content)
	{
		auto tb1 = AllocTextBox();
		tb1->SetText(UTF8ToWide(item[0]));
		Panel1->AddChild(tb1);

		auto tb2 = AllocTextBox();
		tb2->SetText(UTF8ToWide(item[1]));
		Panel2->AddChild(tb2);

		if (bRecordNext)
		{
			stream << item[0] << "," << item[1] << "\n";
		}
	}

	if (bRecordNext)
	{
		bRecordNext = false;
		stream.close();
	}
}

void LostCore::FStackCounterConsole::FinishDisplay()
{
	for (auto item : OnlineText)
	{
		item->Detach();
		OfflineText.push_back(item);
	}

	OnlineText.clear();
}

void LostCore::FStackCounterConsole::Record()
{
	bRecordNext = true;
}

FTextBox * LostCore::FStackCounterConsole::AllocTextBox()
{
	FTextBox* tb = nullptr;
	if (OfflineText.empty())
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
		SAFE_DELETE(item);
	}

	OnlineText.clear();

	for (auto item : OfflineText)
	{
		SAFE_DELETE(item);
	}

	OfflineText.clear();

	SAFE_DELETE(Panel2);
	SAFE_DELETE(Panel1);
	SAFE_DELETE(Panel0);
}
