// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "ThreadSynchronize.h"
#include "CommandBinding.h"

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

void Test5()
{
	char str[] = "hello world.";
	vector<uint8> buf;

	buf.reserve(sizeof(str));
	memcpy(buf.data(), str, sizeof(str));

	auto func = [&]()
	{
		vector<uint8> _buf;
		_buf.reserve(buf.capacity());
		memcpy(_buf.data(), buf.data(), buf.capacity());
		cout << _buf.data() << endl;
	};

	func();
}

void Test8()
{
	uint8 curr = 32, last = 127;
	string initial;
	initial.resize(last - curr);
	for_each(initial.begin(), initial.end(), [&](char& elem)
	{
		elem = char(curr++);
	});

	cout << initial << endl;

	curr = 32;
	wstring initial2;
	initial2.resize(last - curr);
	for_each(initial2.begin(), initial2.end(), [&](wchar_t& elem)
	{
		elem = wchar_t(curr++);
	});

	wcout << initial2 << endl;
}

struct FStruct9 : public enable_shared_from_this<FStruct9>
{
	int32 Num;
	//FStruct9() { cout << "ctor: " << Num << endl; }
	explicit FStruct9(int32 num) :Num(num) { cout << "explicit ctor: " << Num << endl; }
	~FStruct9()
	{
		cout << "dtor: " << Num << endl;
	}

	shared_ptr<FStruct9> GetPtr()
	{
		return shared_from_this();
	}
};

void Test9()
{
	vector<shared_ptr<FStruct9>> ints;
	vector<weak_ptr<FStruct9>> weakInts;
	for (int32 i = 0; i < 10; i++)
	{
		ints.push_back(shared_ptr<FStruct9>(new FStruct9(i)));
		weakInts.push_back(ints.back());
	}

	shared_ptr<FStruct9> p;
	shared_ptr<FStruct9> p2(new FStruct9(100));
	weak_ptr<FStruct9> wp(p2);
	auto wpp = wp.lock();
	cout << "wp: " << (wp.expired() ? "expired " : "available ") << wpp.use_count() << endl;
	p = p2->GetPtr();
	p2 = nullptr;
	p = nullptr;
	cout << "p: " << (p == nullptr ? "empty" : "not empty") << ", p2: " << (p2 == nullptr ? "empty" : "not empty") << endl;

	ints.clear();
	weakInts.clear();
}

void TestSync()
{
	//FCommandBindingSample sample;
	//FSyncSample sample(100);
}

void TestBinding()
{
	FCommandBindingSample sample;
}

void OutputInt32(int32& var)
{
	cout << var << endl;
}

void TestMemoryOrder()
{
}

// Stl set
void Test10()
{
	set<int32> set0({ 0, 1, 2, 3 }), set1({ 0, 1, 2, 3 });
	array<int32, 4> arr0 = { 0,1,2,2 };
	set0.insert(arr0.begin(), arr0.end());
	cout << (set0==set1?"true":"false")<< endl;
}

class F11BaseA
{
public:
	virtual string GetName() { return "BaseA"; }
};

class F11BaseB
{
public:
	virtual string GetName() { return "BaseB"; }
};

class F11Class : public F11BaseA, public F11BaseB
{
};

// ¶à¼Ì³Ðcast
void Test11()
{
	F11Class* p = new F11Class;
	cout << ((F11BaseA*)p)->GetName() << endl;
	cout << ((F11BaseB*)p)->GetName() << endl;
}

struct FObj12
{
	int32 Mem;
	FObj12(){}
	~FObj12()
	{
		Mem = -999;
	}
	explicit FObj12(int32 val) :Mem(val) {}
	FObj12(const FObj12& rhs)
	{
		Mem = rhs.Mem;
		cout << "FObj12(const FObj12& rhs)" << endl;
	}

	FObj12(FObj12&& rhs)
	{
		Mem = rhs.Mem;
		cout << "FObj12(const FObj12&& rhs)" << endl;
	}

	void Output()
	{
		cout << Mem << endl;
	}
};

void RefObj12(FObj12& a)
{
	a.Output();
}

void ValueObj12(FObj12 a)
{
	a.Output();
}

void PointObj12(FObj12* a)
{
	a->Output();
}

// bind
void Test12()
{
	function<void()> func = nullptr;
	{
		FObj12 s0(123);
		func = bind(&ValueObj12, s0);
	}
	func();

	{
		FObj12 s0(456);
		func = bind(&RefObj12, forward<FObj12>(s0));
	}
	func();

	{
		FObj12 s0(789);
		func = bind(&PointObj12, &s0);
	}
	func();
}

#
class F13
{
public:
	void * operator new(uint32 sz)
	{
		cout << "F13 operator new: " << sz << endl;
		return malloc(sz);
	}

	void operator delete(void* p)
	{
		cout << "F13 operator delete: " << sizeof(F13) << endl;
		free(p);
	}

private:
	int32 Mem0;
	double Mem1;
};

int main()
{
	cout << "***************************************" << endl;
	//TestSync();
	//TestBinding();
	//Test12();
	auto p = new F13;
	delete p;
	cout << "***************************************" << endl;

	system("pause");

    return 0;
}

