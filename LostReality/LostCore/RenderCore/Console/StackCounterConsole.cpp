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

LostCore::FStackCounterConsole::FStackCounterConsole()
	: Sheet(nullptr)
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
	Sheet = new FTextSheet;
	Sheet->Initialize();
	parent->AddChild(Sheet);
}

void LostCore::FStackCounterConsole::FinishCounting()
{
	FStackCounterManager::Get()->Finish();

	static FStackCounterRequest SCounter("FStackCounterConsole::FinishCounting");
	FScopedStackCounterRequest scopedCounter(SCounter);

	char head[128];
	memset(head, 0, 128);
	auto frameTime = FGlobalHandler::Get()->GetFrameTime();
	snprintf(head, 127, "\t\t%.1fFPS %.2fms", 1.0f / frameTime, 1000 * frameTime);
	string caption = string("Thread: ").append(FProcessUnique::Get()->GetCurrentThread()->GetName());
	caption.append(head);


	auto content = FStackCounterManager::Get()->GetDisplayContent();
	auto header = FStackCounter::GetDescHeader();
	Sheet->SetCaption(caption);
	Sheet->SetHeader(header);
	Sheet->AddRows(content);

	if (bRecordNext)
	{
		ofstream stream;
		string url("Statics-"), ext(".csv"), output;
		FDirectoryHelper::Get()->GetSpecifiedAbsolutePath("Profile", url.append(GetNowStr(true)).append(ext), output);
		stream.open(output);
		stream << caption << "\n";

		for (auto& item : header)
		{
			stream << item << ",";
		}
		stream << "\n";

		for (auto& item : content)
		{
			for (auto& sub : item)
			{
				stream << sub << ",";
			}
			stream << "\n";
		}
		//stream << "Thread: " << FProcessUnique::Get()->GetCurrentThread()->GetName() << "\n";
		//stream << "FrameTime: " << to_string(1000 * frameTime) << "ms, FPS: " << to_string(1 / frameTime) << "\n";
		bRecordNext = false;
		stream.close();
	}
}

void LostCore::FStackCounterConsole::FinishDisplay()
{
}

void LostCore::FStackCounterConsole::Record()
{
	bRecordNext = true;
}

void LostCore::FStackCounterConsole::Destroy()
{
}
