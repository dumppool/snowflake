#include "stdafx.h"
#include "CommandBinding.h"

using namespace LostCore;

FCommandBindingSample::FCommandBindingSample()
{
	FCBParent p("Kevin");
	FCBChild c("Mike");

	auto b1 = bind(&FCBParent::Method1, &p, " Kevin binding FCBParent::Method1");
	auto b2 = bind(&FCBParent::Method1, &c, " Mike binding FCBParent::Method1");
	auto b3 = bind(&FCBChild::Method1, &c, " Mike binding FCBChild::Method1");
	auto b4 = bind(&FCBChild::Method2, &c, " Mike binding FCBChild::Method2");

	b1();
	b2();
	b3();
	b4();

	auto bb1 = bind(&FCBParent::Method1, placeholders::_1, " haha");
	function<void(FCBParent*)> f1 = bb1;
	//bb1(&p);
	f1(&p);
}

FCommandBindingSample::~FCommandBindingSample()
{
}

FCBParent::FCBParent(const string & name)
	: Name(name)
{
}

void FCBParent::Method1(const string& input)
{
	cout << string("FCBParent::Method1 ").append(Name).append(input) << endl;
}

void FCBParent::Method2(const string& input)
{
	cout << string("FCBParent::Method2 ").append(Name).append(input) << endl;
}

FCBChild::FCBChild(const string & name)
	: FCBParent("FCBChild")
	, Name(name)
{
}

void FCBChild::Method1(const string& input)
{
	cout << string("FCBChild::Method1 ").append(Name).append(input) << endl;
}

FCBObjectRequest::FCBObjectRequest()
	: Id(FCBObjectAllocator::Get()->RequestAlloc())
{
}

FCBObjectRequest::~FCBObjectRequest()
{
	FCBObjectAllocator::Get()->RequestDealloc(Id);
}

void FCBObjectRequest::Request1(const string & input)
{
	cout << "Request1 " << Id << " " << input << endl;
	PushCommand([=]()
	{
		FCBObjectAllocator::Get()->GetObj(Id)->Response1(input);
	});
}

void FCBObjectRequest::Request2(const string & input)
{
	cout << "Request2 " << Id << " " << input << endl;
	PushCommand([=]()
	{
		FCBObjectAllocator::Get()->GetObj(Id)->Response2(input);
	});
}

void FCBObjectRequest::Request3(const string & input)
{
	cout << "Request3 " << Id << " " << input << endl;
	PushCommand([=]()
	{
		FCBObjectAllocator::Get()->GetObj(Id)->Response2(input);
	});
}

FCBObject::FCBObject()
{
}

FCBObject::~FCBObject()
{
}

void FCBObject::Response1(const string & input)
{
	cout << "Response1 " << input << endl;
}

void FCBObject::Response2(const string & input)
{
	cout << "Response2 " << input << endl;
}

void FCBObject::Response3(const string & input)
{
	cout << "Response3 " << input << endl;
}

FCBObjectAllocator::FCBObjectAllocator()
	: LastAllocatedId(1)
{
}

FCBObjectAllocator::~FCBObjectAllocator()
{
	assert(AllocatedObjects.empty());
}

int32 FCBObjectAllocator::RequestAlloc()
{
	if (LastAllocatedId < SMaxAllocId)
	{
		return ++LastAllocatedId;
	}

	assert(!UnusedIds.empty());
	auto id = UnusedIds.back();
	UnusedIds.pop_back();
	Allocating.push_back(id);
	return id;
}

void FCBObjectAllocator::RequestDealloc(int32 id)
{
	Deallocating.push_back(id);
}

void FCBObjectAllocator::FlushAllocating()
{
	for (auto id : Allocating)
	{
		auto p = new FCBObject;
		AllocatedObjects[id] = p;
	}

	Allocating.clear();
}

void FCBObjectAllocator::FlushDeallocating()
{
	for (auto id : Deallocating)
	{
		assert(AllocatedObjects.find(id) != AllocatedObjects.end());
		delete AllocatedObjects[id];
		UnusedIds.push_back(id);
	}

	Deallocating.clear();
}

FCBObject * FCBObjectAllocator::GetObj(int32 id)
{
	auto it = AllocatedObjects.find(id);
	assert(it != AllocatedObjects.end());
	return it->second;
}
