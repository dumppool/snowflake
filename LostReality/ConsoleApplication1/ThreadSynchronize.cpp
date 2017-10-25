#include "stdafx.h"
#include "ThreadSynchronize.h"

using namespace LostCore;

FThreadSynchronizerSample::FThreadSynchronizerSample()
	: Seq(1)
{
	FProcessUnique::StaticInitialize();
	EntryThread = new FTickThread(this, "FThreadSynchronizerSample");
}

FThreadSynchronizerSample::~FThreadSynchronizerSample()
{
	SAFE_DELETE(EntryThread);
	FProcessUnique::StaticDestroy();
}

bool FThreadSynchronizerSample::Initialize()
{
	cout << "FThreadSynchronizerSample::Initialize " << thread::hardware_concurrency() << endl;
	TimeStamp = LostCore::FPerformanceCounter::GetTimeStamp();
	const int32 num = 2;
	const int32 end = 10000;
	const int32 step = end / num;
	int32 rb = 0;
	int32 re = 0;
	int32 affinityMaskOffset = 0;
	while (re < end)
	{
		rb = re;
		re += step;
		if (re > end)
		{
			re = end;
		}

		auto p = new FCountPrimeNumsTask(rb, re);
		Tasks.push_back(p);
		Workers.push_back(new LostCore::FThread(p, "temp"));
		affinityMaskOffset = (affinityMaskOffset + 1) % 4;
	}

	return true;
}

void FThreadSynchronizerSample::Tick()
{
}

void FThreadSynchronizerSample::Destroy()
{
	cout << "Num of workers: " << Workers.size() << endl;

	for (auto& item : Workers)
	{
		SAFE_DELETE(item);
	}
	Workers.clear();

	int32 result = 0;
	for (auto& item : Tasks)
	{
		cout << "Time cost of worker thread: " << item->Past << endl;
		result += item->Result;

		SAFE_DELETE(item);
	}
	Tasks.clear();

	cout << "Result: " << result << endl;
	cout << "Time cost of all: " << LostCore::FPerformanceCounter::GetSeconds(TimeStamp) << endl;
	cout << "FThreadSynchronizerSample::Destroy" << endl;
}

void FCountPrimeNumsTask::Execute()
{
	Result = RangeEnd - RangeBegin;
	this_thread::sleep_for(chrono::milliseconds(Result));
	//Result = CountPrimeNums(RangeBegin, RangeEnd);
}

void FCountPrimeNumsTask::OnFinished(double sec)
{
	Past = sec;
}

bool FCountPrimeNumsTask::IsThreadPrivate()
{
	return false;
}
