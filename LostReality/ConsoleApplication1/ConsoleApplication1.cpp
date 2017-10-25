// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ThreadSynchronize.h"

struct _A
{
	const char* Name;
	const char* Definition;
};

#define MACRO_A "a"
#define MACRO_128 (1<<7)

void Test0()
{
	string temp;
	auto str1 = to_string(MACRO_128);
	char c[4];
	snprintf(c, 3, to_string(1).c_str());
	_A a = { MACRO_A, to_string(1).c_str() };
	auto b = (temp = to_string(MACRO_128)).data();
	auto b2 = (temp = to_string(MACRO_128)).c_str();
	cout << b << b2 << endl;
}

void Test1()
{
	thread t;
	this_thread::get_id();
	cout << sizeof(void*) << endl;

	string ss;
	cout << string(4, 'h') << endl;
}

void Test2()
{
	int a[] = { 7, 8, 9, 24 };

	auto func = [](int arg, int al) ->int
	{
		return ((arg+al-1) & ~(al-1));
	};

	for (auto item : a)
	{
		cout << func(item, 8) << endl;
	}
}

void Test3()
{
	int a[] = { 0,1 };
	string output;
	auto task0 = [&]()
	{
		while (1)
		{
			if (a[0] != 0)
			{
				output.append("0a ");
				output.append("0b ");
				output.append("0c ");
				output.append("0d ");
				output.append("0e ");
				output.append("0f ");
				break;
			}
		}
	};

	auto task1 = [&]()
	{
		while (1)
		{
			if (a[0] != 0)
			{
				output.append("1a ");
				output.append("1b ");
				output.append("1c ");
				output.append("1d ");
				output.append("1e ");
				output.append("1f ");
				break;
			}
		}
	};

	thread threads[] = { thread(task0), thread(task1) };

	a[0] = 1;

	for (auto& item : threads)
	{
		if (item.joinable())
		{
			item.join();
		}
	}

	cout << "seq: " << string(output) << endl;
}

void Test4()
{
	time_t t = ::time(0);
	tm curr;
	::localtime_s(&curr, &t);
	char cdate[1024];
	snprintf(cdate, 1023, "%d/%d/%d %d:%d:%d", 1900 + curr.tm_year, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);
}

struct FThreadWork
{
	bool bRunning;
	thread T;

	FThreadWork()
	{
		bRunning = true;
		T = thread([=]() {Run(); });
	}

	~FThreadWork()
	{
		bRunning = false;
		if (T.joinable())
		{
			T.join();
		}
	}

	void Run()
	{
		cout << "Thread started." << endl;
		while (bRunning)
		{
			this_thread::sleep_for(chrono::milliseconds(10));
		}

		cout << "Thread ended." << endl;
	}
};

// Thread shutdown
void Test5()
{
	auto t = new FThreadWork;
	delete t;
}

struct FCondition
{
	mutex Mutex;
	condition_variable CondVariable;

	void WaitForInvoke()
	{
		unique_lock<mutex> lck(Mutex);
		CondVariable.wait(lck);
	}

	void Invoke()
	{
		CondVariable.notify_all();
	}
};

struct FConditionalThread
{
	thread T;
	bool bInvoke;
	FCondition Self;
	FCondition* Master;

	explicit FConditionalThread(FCondition* master) : Master(master), bInvoke(false)
	{
		T = thread([=]() {Run(); });
	}

	~FConditionalThread()
	{
		cout << "Prepare to release: " << T.get_id() << endl;
		if (T.joinable())
		{
			T.join();
		}
	}

	void Run()
	{
		while (!bInvoke && Master == nullptr)
		{
			this_thread::sleep_for(chrono::milliseconds(10));
		}

		if (Master != nullptr)
		{
			Master->WaitForInvoke();
			cout << "Got invoked: " << T.get_id() << endl;
		}
		else
		{
			cout << "Prepare to Invoke: " << T.get_id() << endl;
			Self.Invoke();
		}
	}
};

// Thread synchronize
void Test6()
{
	FConditionalThread Master(nullptr);
	this_thread::sleep_for(chrono::milliseconds(10));

	const int numSlaves = 10;
	FConditionalThread* Slaves[numSlaves];
	for (int i = 0; i < numSlaves; ++i)
	{
		Slaves[i] = new FConditionalThread(&(Master.Self));
	}

	this_thread::sleep_for(chrono::milliseconds(10));
	Master.bInvoke = true;

	for (int i = 0; i < numSlaves; ++i)
	{
		delete Slaves[i];
	}
}

// Frame condition thread
void Test7()
{
	FThreadSynchronizerSample sample;
}

int main()
{
	cout << "***************************************" << endl;
	Test7();
	cout << "***************************************" << endl;

	while (1)
	{
		this_thread::sleep_for(chrono::seconds(10));
		;
	}

    return 0;
}

