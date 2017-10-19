/*
* file Thread.h
*
* author luoxw
* date 2017/10/19
*
*/

#pragma once

namespace LostCore
{
	typedef map<int32, void*> FIndexedAddress;

	class FThread;
	class FProcessUnique
	{
		static FProcessUnique* SInstance;

	public:
		static FORCEINLINE FProcessUnique* Get()
		{
			return SInstance;
		}

		static FORCEINLINE void Initialize()
		{
			SInstance = new FProcessUnique;
		}

		static FORCEINLINE void Destroy()
		{
			SAFE_DELETE(SInstance);
		}

		static FORCEINLINE void SetInstance(FProcessUnique* instance)
		{
			SInstance = instance;
		}

	private:
		map<thread::id, FThread*> ThreadMap;

	public:
		FORCEINLINE FProcessUnique();
		FORCEINLINE ~FProcessUnique();

		FORCEINLINE void AddThread(FThread* t);
		FORCEINLINE void RemoveThread(FThread* t);
		FORCEINLINE FThread* GetThread();
	};

	class FThread
	{
		thread Thread;
		string Name;
		function<void()> Payload;
		FIndexedAddress IndexedSingletonMap;

	public:
		FORCEINLINE FThread(function<void()> func, const string& name);
		FORCEINLINE ~FThread();

		FORCEINLINE void Run();
		FORCEINLINE thread::id GetId() const;
		FORCEINLINE string GetName() const;
		FORCEINLINE void* GetSingleton(int32 index);
		FORCEINLINE void AddSingleton(int32 index, void* singleton);
	};

	FProcessUnique::FProcessUnique()
	{
		ThreadMap.clear();
	}
	
	FProcessUnique::~FProcessUnique()
	{
		assert(ThreadMap.empty());
	}
	
	void FProcessUnique::AddThread(FThread * t)
	{
		assert(t != nullptr);
		ThreadMap[t->GetId()] = t;
	}
	
	void FProcessUnique::RemoveThread(FThread * t)
	{
		assert(ThreadMap.find(t->GetId()) != ThreadMap.end());
		ThreadMap.erase(t->GetId());
	}
	
	FThread * FProcessUnique::GetThread()
	{
		auto id = this_thread::get_id();
		assert(ThreadMap.find(id) != ThreadMap.end());
		assert(ThreadMap.find(this_thread::get_id()) != ThreadMap.end());
		return ThreadMap.find(this_thread::get_id())->second;
	}

	FThread::FThread(function<void()> func, const string& name)
		: Payload(func)
		, Name(name)
	{
		IndexedSingletonMap.clear();
		Thread = thread([=]() {Run(); });
	}

	FThread::~FThread()
	{
		if (Thread.joinable())
		{
			Thread.join();
		}
	}
	
	void FThread::Run()
	{
		this_thread::sleep_for(chrono::milliseconds(1));
		FProcessUnique::Get()->AddThread(this);
		Payload();

		// 线程结束,释放本地单例.
		for (auto item : IndexedSingletonMap)
		{
			assert(item.second != nullptr);
			SAFE_DELETE(item.second);
		}

		IndexedSingletonMap.clear();
		FProcessUnique::Get()->RemoveThread(this);
	}
	
	thread::id FThread::GetId() const
	{
		return Thread.get_id();
	}
	
	string FThread::GetName() const
	{
		return Name;
	}
	
	void * FThread::GetSingleton(int32 index)
	{
		assert(this_thread::get_id() == Thread.get_id());
		auto it = IndexedSingletonMap.find(index);
		if (it != IndexedSingletonMap.end())
		{
			return it->second;
		}
		else
		{
			return nullptr;
		}
	}
	
	void FThread::AddSingleton(int32 index, void * singleton)
	{
		assert(this_thread::get_id() == Thread.get_id());
		assert(IndexedSingletonMap.find(index) == IndexedSingletonMap.end());
		IndexedSingletonMap[index] = singleton;
	}
}