#include "stdafx.h"
#include "ThreadSynchronize.h"

using namespace LostCore;

static const int32 SFlushNum = 1000;

FThreadSynchronizerSample::FThreadSynchronizerSample()
	: Seq(1)
{
	FProcessUnique::StaticInitialize();
	EntryThread = new FThread(this, "FThreadSynchronizerSample");
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

FSyncGuest::FSyncGuest()
	: Thread(new FThread(this, "SyncProducer"))
	, Data(1)
	, Cmds(1)
{
}

FSyncGuest::~FSyncGuest()
{
	SAFE_DELETE(Thread);
}

double FSyncGuest::GetData(int32 * data)
{
	return Data.SyncRead(data);
}

void FSyncGuest::PushMsg(string& buf)
{
	//Cmds.Push([=]() {
	//	cout << buf.data() << endl;
	//});
}

void FSyncGuest::PushCommand(const FCmd & cmd)
{
	Cmds.Ref().Push(cmd);
}

void FSyncGuest::FinishCommand()
{
	auto sec = Cmds.SyncCommit();
	cout << "SyncCommit waited for " << sec*1000 << " ms" << endl;
}

bool FSyncGuest::Initialize()
{
	cout << "FSyncProducer::Initialize" << endl;
	return true;
}

void FSyncGuest::Tick()
{
	//auto r = rand();
	//Data.Ref() = r;
	//auto past = Data.SyncCommit();
	//SIntSet0.insert(r);
	//cout << "Write data: " << past * 1000 << "ms, " << r << endl;
	FCommandQueue<FCmd> cmds;
	auto sec = Cmds.SyncRead(&cmds);
	cout << "SyncRead waited for " << sec * 1000 << " ms" << endl;

	auto ts = FPerformanceCounter::GetTimeStamp();
	FCmd cmd;
	while (cmds.Pop(cmd))
	{
		cmd.Exec();
	}
	cout << "FSyncGuest::Tick cost ms " << FPerformanceCounter::GetSeconds(ts) * 1000 << endl;
}

void FSyncGuest::Destroy()
{
	cout << "FSyncProducer::Destroy" << endl;
}

FSyncHost::FSyncHost()
	: Thread(new FThread(this, "FSyncConsumer"))
	, Guest(new FSyncGuest)
{
	FSyncGuest::Get() = Guest;
}

FSyncHost::~FSyncHost()
{
	FSyncGuest::Get() = nullptr;
	SAFE_DELETE(Thread);
	SAFE_DELETE(Guest);
}

bool FSyncHost::Initialize()
{
	cout << "FSyncConsumer::Initialize" << endl;
	Messages.push_back("Hello world.");
	Messages.push_back("Commit message in consumer thread.");
	Messages.push_back("When commit, messages will be captured before pushed into producer's command queue.");
	Messages.push_back("In producer thread, command will be popped and output one by one.");

	for (int32 i = 0; i < SFlushNum; i++)
	{
		Pending.push_back(new FObj11);
	}
	
	return true;
}

void FSyncHost::Tick()
{
	//int32 val;
	//auto past = Guest->GetData(&val);
	//SIntSet1.insert(val);
	//cout << "Get data: " << past*1000 << "ms, " << val << endl;

	// 顺序一致性测试
	//if (Messages.empty())
	//	return;


	//Guest->PushMsg(Messages[0]);
	//Messages.erase(Messages.begin());

	// 对象初始化完整性测试
	auto ts = FPerformanceCounter::GetTimeStamp();
	for (int32 i = 0; i < SFlushNum; i++)
	{
		Pending[i]->DoSth(string("haha ").append(to_string(i)));
	}
	cout << endl;
	cout << "FSyncHost::Tick cost ms " << FPerformanceCounter::GetSeconds(ts) * 1000 << endl;
	Guest->FinishCommand();

}

void FSyncHost::Destroy()
{
	for (auto item : Pending)
	{
		delete (FObj11*)item;
	}
	Pending.clear();
	cout << "FSyncConsumer::Destroy" << endl;
}

FSyncSample::FSyncSample(int32 sec)
{
	FProcessUnique::StaticInitialize();
	FSyncHost consumer;

	this_thread::sleep_for(chrono::seconds(sec));
	cout << "Time's UP" << endl;
}

FSyncSample::~FSyncSample()
{
	FProcessUnique::StaticDestroy();
}

//FSyncGuest::FCmd::FHandler FObj11::Obj11Handler = [](void* p, const FSyncGuest::FCmd::FBody& body) {body((FObj11*)p); };

FObj11::FObj11()
	: Mem(1)
	, SlowInit()
{
	Mem = 999;
}

void FObj11::DoSth(const string& sth)
{
	if (FSyncGuest::Get() != nullptr)
	{
		FSyncGuest::Get()->PushCommand(FSyncGuest::FCmd(
			(void*)this,
			//bind(&ExecDoSth, placeholders::_1, sth)
			&ExecDummy
		));
	}
}

FObj11::FObjSlowInitialize::FObjSlowInitialize()
{
	this_thread::sleep_for(chrono::milliseconds(1));
}

void ExecDoSth(void * p, const string & sth)
{
	auto pthis = (FObj11*)p;
	assert(pthis->Mem == 999);
	//cout << sth << endl;
}

void ExecDummy(void* p)
{

}
