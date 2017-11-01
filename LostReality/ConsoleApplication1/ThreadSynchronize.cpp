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

static set<int32> SIntSet0;
static set<int32> SIntSet1;

FSyncProducer::FSyncProducer()
	: Thread(new FTickThread(this, "SyncProducer"))
	, Data(1)
	, Cmds(true)
{
}

FSyncProducer::~FSyncProducer()
{
	SAFE_DELETE(Thread);
}

double FSyncProducer::GetData(int32 * data)
{
	return Data.SyncRead(data);
}

void FSyncProducer::PushMsg(string& buf)
{
	Cmds.Push([=]() {
		cout << buf.data() << endl;
	});
}

bool FSyncProducer::Initialize()
{
	cout << "FSyncProducer::Initialize" << endl;
	return true;
}

void FSyncProducer::Tick()
{
	//auto r = rand();
	//Data.Ref() = r;
	//auto past = Data.SyncCommit();
	//SIntSet0.insert(r);
	//cout << "Write data: " << past * 1000 << "ms, " << r << endl;
	function<void()> cmd;
	while (Cmds.Pop(cmd))
	{
		cmd();
	}
}

void FSyncProducer::Destroy()
{
	cout << "FSyncProducer::Destroy" << endl;
}

FSyncConsumer::FSyncConsumer()
	: Thread(new FTickThread(this, "FSyncConsumer"))
	, Producer(new FSyncProducer)
{
}

FSyncConsumer::~FSyncConsumer()
{
	SAFE_DELETE(Thread);
	SAFE_DELETE(Producer);
}

bool FSyncConsumer::Initialize()
{
	cout << "FSyncConsumer::Initialize" << endl;
	Messages.push_back("Hello world.");
	Messages.push_back("Commit message in consumer thread.");
	Messages.push_back("When commit, messages will be captured before pushed into producer's command queue.");
	Messages.push_back("In producer thread, command will be popped and output one by one.");
	return true;
}

void FSyncConsumer::Tick()
{
	//int32 val;
	//auto past = Producer->GetData(&val);
	//SIntSet1.insert(val);
	//cout << "Get data: " << past*1000 << "ms, " << val << endl;
	if (Messages.empty())
		return;


	Producer->PushMsg(Messages[0]);
	Messages.erase(Messages.begin());
}

void FSyncConsumer::Destroy()
{
	cout << "FSyncConsumer::Destroy" << endl;
}

FSyncSample::FSyncSample(int32 sec)
{
	FProcessUnique::StaticInitialize();
	FSyncConsumer consumer;

	this_thread::sleep_for(chrono::seconds(sec));
	cout << "Time's UP" << endl;
}

FSyncSample::~FSyncSample()
{
	FProcessUnique::StaticDestroy();
}
