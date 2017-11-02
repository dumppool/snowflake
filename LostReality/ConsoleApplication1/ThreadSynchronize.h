
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


class FSyncGuest : public LostCore::ITickTask
{
public:
	class FCmd
	{
	public:
		typedef function<void(void*)> FBody;

		FCmd() {}
		FCmd(void* p, const FBody& body)
			: Ptr(p), Body(body)
		{
		}

		void Exec()
		{
			Body(Ptr);
		}

	private:
		void* Ptr;
		FBody Body;
	};
	//typedef function<void()> FCmd;

	static FSyncGuest*& Get()
	{
		static FSyncGuest* SPtr = nullptr;
		return SPtr;
	}

	FSyncGuest();
	~FSyncGuest();

	double GetData(int32* data);
	void PushMsg(string& buf);
	void PushCommand(const FCmd& cmd);
	void FinishCommand();

	// Inherited via ITickTask
	virtual bool Initialize() override;
	virtual void Tick() override;
	virtual void Destroy() override;

private:
	LostCore::FTickThread* Thread;
	LostCore::TSynchronizer<int32> Data;
	LostCore::TSynchronizer<LostCore::FCommandQueue<FCmd>> Cmds;
};

class FObj11;
class FSyncHost : public LostCore::ITickTask
{
	LostCore::FTickThread* Thread;
	FSyncGuest* Guest;
	vector<string> Messages;
	vector<FObj11*> Pending;

public:

	FSyncHost();
	~FSyncHost();

	// Inherited via ITickTask
	virtual bool Initialize() override;
	virtual void Tick() override;
	virtual void Destroy() override;

};

class FSyncSample
{
public:
	explicit FSyncSample(int32 sec);
	~FSyncSample();
};
class FObj11
{
public:

	class FObjSlowInitialize
	{
	public:
		FObjSlowInitialize();
	};


	FObj11();

	void DoSth(const string& sth);

private:
	FObjSlowInitialize SlowInit;
	int32 Mem;

private:
	friend void ExecDoSth(void* p, const string& sth);
	friend void ExecDummy(void* p);
};