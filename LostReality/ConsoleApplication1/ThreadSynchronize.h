
#pragma once

#include "targetver.h"

static int32 CountPrimeNums(int32 rangeBegin, int32 rangeEnd)
{
	int32 count = 0;
	for (int32 n = rangeBegin; n < rangeEnd; n++)
	{
		for (int32 i = 2; i < n; i++)
		{
			if ((n%i) == 0)
			{
				count++;
				break;
			}
		}
	}

	return count;
}

struct FCountPrimeNumsTask : public LostCore::IPayload
{
	int32 Result;
	int32 RangeBegin;
	int32 RangeEnd;
	double Past;

	//FCountPrimeNumsTask() {}
	FCountPrimeNumsTask(int32 rb, int32 re)
		: RangeBegin(rb)
		, RangeEnd(re)
		, Result(-1)
	{
	}

	// Inherited via IPayload
	virtual void Execute() override;
	virtual void OnFinished(double sec) override;
	virtual bool IsThreadPrivate() override;
};

class FThreadSynchronizerSample : public LostCore::ITickTask
{
public:
	FThreadSynchronizerSample();
	~FThreadSynchronizerSample();

	// Inherited via ITickTask
	virtual bool Initialize() override;
	virtual void Tick() override;
	virtual void Destroy() override;

private:
	LostCore::FTickThread* EntryThread;
	LostCore::TSynchronizer<vector<int>> Seq;
	vector<FCountPrimeNumsTask*> Tasks;
	vector<LostCore::FThread*> Workers;
	LARGE_INTEGER TimeStamp;
};