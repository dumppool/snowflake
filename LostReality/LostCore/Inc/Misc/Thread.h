/*
* file Thread.h
*
* author luoxw
* date 2017/10/19
*
*/

#pragma once
#include "Math/Average.h"

namespace LostCore
{
	class FThread;
	class ITickable
	{
	public:
		virtual ~ITickable() {}
		virtual void Tick() = 0;
	};

	typedef map<int32, ITickable*> FTickableObjects;

	enum class ECondition : uint8
	{
		UnDefined,
		CollectStatics,
		BeginFrame,
	};

	template <typename T>
	class TSynchronizer
	{
	public:
		explicit TSynchronizer(int32 num);
		~TSynchronizer();

		double SyncCommit();
		double SyncRead(T* obj);

		T& Ref();

	private:
		atomic<bool> bWaitingForRead;
		atomic<int32> ReadIndex;
		atomic<int32> WriteIndex;
		vector<T> Objects;
	};

	class ITask
	{
	public:
		virtual ~ITask() {}
		virtual bool Initialize() = 0;
		virtual void Tick() = 0;
		virtual void Destroy() = 0;
		virtual bool IsThreadPrivate() const = 0;
		virtual bool IsLoop() const = 0;
	};

	class FProcessUnique : public ITask
	{
	public:
		static FORCEINLINE FProcessUnique* Get()
		{
			return SInstance;
		}

		// 只有exe模块可以初始化.
		static FORCEINLINE void StaticInitialize()
		{
			SInstance = new FProcessUnique;
			SIsOriginal = true;
		}

		static FORCEINLINE void StaticDestroy()
		{
			assert(SIsOriginal);
			SAFE_DELETE(SInstance);
			SIsOriginal = false;
		}

		// dll模块只能使用SetInstance接受exe的静态实例.
		static FORCEINLINE void SetInstance(FProcessUnique* instance)
		{
			SInstance = instance;
			SIsOriginal = false;
		}

	public:
		FORCEINLINE FProcessUnique();
		FORCEINLINE virtual ~FProcessUnique() override;

		// Inherited via ITickTask
		FORCEINLINE virtual bool Initialize() override;
		FORCEINLINE virtual void Tick() override;
		FORCEINLINE virtual void Destroy() override;
		FORCEINLINE virtual bool IsThreadPrivate() const override;
		FORCEINLINE virtual bool IsLoop() const override;

		FORCEINLINE void AddThread(FThread* t);
		FORCEINLINE void RemoveThread(FThread* t);
		FORCEINLINE FThread* GetCurrentThread();

		template <typename T>
		FORCEINLINE T* GetSingleton(int32 index);

	private:
	
		map<thread::id, FThread*> ThreadMap;
		mutex ThreadMapMutex;

		typedef function<void()> FCmd;
		FCommandQueue<FCmd> Commands;

		FThread* GuardThread;
		FTickableObjects Singletons;
		mutex SingletonsMutex;

		static bool SIsOriginal;
		static FProcessUnique* SInstance;

	};

	template <typename T, int32 ClassIndex>
	class TProcessUniqueSingleton : public ITickable
	{
	public:
		static const int32 SClassIndex = ClassIndex;
		static T* Get()
		{
			return FProcessUnique::Get()->GetSingleton<T>(SClassIndex);
		}
	};

	class FThread
	{
	public:

		FORCEINLINE FThread();
		FORCEINLINE FThread(ITask* task, const string& name, uint32 affinityMask = 0xff);
		FORCEINLINE virtual ~FThread();

		FORCEINLINE thread::id GetId() const;
		FORCEINLINE string GetName() const;
		FORCEINLINE double GetFrameSec() const;
		FORCEINLINE double GetFrameSecAvg() const;
		FORCEINLINE ITickable* GetSingleton(int32 index);
		FORCEINLINE void AddSingleton(int32 index, ITickable* singleton);
		FORCEINLINE void SetAffinity(uint32 mask);
		FORCEINLINE uint32 GetAffinity() const;
		FORCEINLINE ITask* GetPayload();
		FORCEINLINE string GetFrameInfo() const;

	protected:
		// Thread线程执行.
		FORCEINLINE virtual void Run();
		FORCEINLINE void Destroy();

	private:

		ITask* Task;
		string Name;
		bool bRunning;
		TAverage<double, 30> TickSeconds;
		uint32 AffinityMask;

		FTickableObjects IndexedSingletonMap;
		thread Thread;
	};

	FORCEINLINE int32 GetThreadId(thread::id id)
	{
		stringstream ss;
		ss << id;
		return stoi(ss.str());
	}

	FProcessUnique::FProcessUnique()
		: Commands(true)
		, GuardThread(new FThread(this, "Guard"))
	{
	}
	
	FProcessUnique::~FProcessUnique()
	{
		SAFE_DELETE(GuardThread);
	}
	
	void FProcessUnique::AddThread(FThread * t)
	{
		lock_guard<mutex> lck(ThreadMapMutex);
		assert(t != nullptr);
		ThreadMap[t->GetId()] = t;
	}
	
	void FProcessUnique::RemoveThread(FThread * t)
	{
		lock_guard<mutex> lck(ThreadMapMutex);
		assert(ThreadMap.find(t->GetId()) != ThreadMap.end());
		ThreadMap.erase(t->GetId());
	}
	
	FThread * FProcessUnique::GetCurrentThread()
	{
		lock_guard<mutex> lck(ThreadMapMutex);
		auto id = this_thread::get_id();
		assert(ThreadMap.find(id) != ThreadMap.end());
		return ThreadMap.find(this_thread::get_id())->second;
	}

	template <typename T>
	T* FProcessUnique::GetSingleton(int32 index)
	{
		auto it = Singletons.find(index);
		if (it == Singletons.end())
		{
			lock_guard<mutex> lck(SingletonsMutex);
			it = Singletons.find(index);
			if (it == Singletons.end())
			{
				it = Singletons.insert(FTickableObjects::value_type(index, new T)).first;
			}

			assert(it != Singletons.end());
			return dynamic_cast<T*>(it->second);
		}

		return dynamic_cast<T*>(it->second);
	}

	bool FProcessUnique::Initialize()
	{
		return true;
	}

	void FProcessUnique::Tick()
	{
		FCmd cmd;
		while (Commands.Pop(cmd))
		{
			cmd();
		}
	}

	void FProcessUnique::Destroy()
	{
		// 忽略线程之间的调用关系，直接干掉线程及线程任务对象很不明智
		for (auto& item : ThreadMap)
		{
			throw exception("Survivor thread found");
			SAFE_DELETE(item.second);
		}
		ThreadMap.clear();

		for (auto& item : Singletons)
		{
			SAFE_DELETE(item.second);
		}
		Singletons.clear();

		assert(ThreadMap.empty());
	}

	bool FProcessUnique::IsThreadPrivate() const
	{
		return false;
	}

	bool FProcessUnique::IsLoop() const
	{
		return true;
	}

	FThread::FThread()
		: Name("unnamed")
		, Task(nullptr)
	{
	}

	FThread::FThread(ITask * task, const string & name, uint32 affinityMask)
		: Task(task)
		, Name(name)
		, AffinityMask(affinityMask)
		, bRunning(true)
	{
		if (task != nullptr)
		{
			Thread = thread([&]() {Run(); });
		}
	}

	FThread::~FThread()
	{
		bRunning = false;
		if (Thread.joinable())
		{
			Thread.join();
		}
	}

	thread::id FThread::GetId() const
	{
		return Thread.get_id();
	}

	string FThread::GetName() const
	{
		return Name;
	}

	double FThread::GetFrameSec() const
	{
		return TickSeconds.GetLast();
	}

	double FThread::GetFrameSecAvg() const
	{
		return TickSeconds.GetAverage();
	}

	ITickable * FThread::GetSingleton(int32 index)
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

	FORCEINLINE void FThread::AddSingleton(int32 index, ITickable* singleton)
	{
		assert(this_thread::get_id() == Thread.get_id());
		assert(IndexedSingletonMap.find(index) == IndexedSingletonMap.end());
		IndexedSingletonMap[index] = singleton;
		LVDEBUG("AddSingleton", "thread: %d, class: %d, 0x%08x.", GetThreadId(this_thread::get_id()), index, singleton);
	}

	void FThread::SetAffinity(uint32 mask)
	{
		AffinityMask = ::SetThreadAffinityMask(Thread.native_handle(), mask);
	}

	uint32 FThread::GetAffinity() const
	{
		return AffinityMask;
	}

	ITask * FThread::GetPayload()
	{
		return Task;
	}

	string FThread::GetFrameInfo() const
	{
		const int32 sz = 256;
		char buf[sz];
		memset(buf, 0, sz);
		auto frameTime = GetFrameSecAvg();
		snprintf(buf, sz-1, "Thread: %s, \t\t%.1fFPS %.2fms", GetName().c_str(), 1.0f / frameTime, 1000 * frameTime);
		return buf;
	}

	void FThread::Run()
	{
		assert(Task != nullptr);
		// 没有sleep,this_thread::get_id为0???
		this_thread::sleep_for(chrono::milliseconds(1));

		SetAffinity(GetAffinity());
		FProcessUnique::Get()->AddThread(this);
		if (!Task->Initialize())
		{
			Destroy();
			return;
		}

		LARGE_INTEGER timeStamp = FPerformanceCounter::GetTimeStamp();

		do
		{
			Task->Tick();
			TickSeconds.Add(FPerformanceCounter::GetSeconds(timeStamp));
			timeStamp = FPerformanceCounter::GetTimeStamp();

			for (auto& item : IndexedSingletonMap)
			{
				item.second->Tick();
			}

			this_thread::sleep_for(chrono::microseconds(10));
		} while (Task->IsLoop() && bRunning);

		Task->Destroy();
		if (Task->IsThreadPrivate())
		{
			SAFE_DELETE(Task);
		}

		Task = nullptr;
		Destroy();
	}

	void FThread::Destroy()
	{
		assert(this_thread::get_id() == Thread.get_id());

		// 线程结束,释放本地单例.
		for (auto item : IndexedSingletonMap)
		{
			assert(item.second != nullptr);
			SAFE_DELETE(item.second);
		}

		IndexedSingletonMap.clear();
		FProcessUnique::Get()->RemoveThread(this);
	}

	template<typename T>
	TSynchronizer<T>::TSynchronizer(int32 num)
		: ReadIndex(-1)
		, WriteIndex(0)
		, Objects(num)
		, bWaitingForRead(false)
	{
	}
	
	template<typename T>
	TSynchronizer<T>::~TSynchronizer()
	{
	}
	
	template<typename T>
	double TSynchronizer<T>::SyncCommit()
	{
		LARGE_INTEGER timeStamp = FPerformanceCounter::GetTimeStamp();

		// 更新初始值.
		if (ReadIndex == -1)
		{
			ReadIndex = 0;
		}

		// 更新写索引.
		WriteIndex = (WriteIndex + 1) % Objects.size();

		// 如果写索引指向不可写入,等待其他线程读取.		
		if (ReadIndex == WriteIndex)
		{
			bWaitingForRead = true;
		}

		while (ReadIndex == WriteIndex && bWaitingForRead)
		{
			this_thread::yield();
		}

		return FPerformanceCounter::GetSeconds(timeStamp);
	}

	template<typename T>
	double TSynchronizer<T>::SyncRead(T * obj)
	{
		LARGE_INTEGER timeStamp = FPerformanceCounter::GetTimeStamp();

		// 如果读索引指向不可读取,等待别的线程提交.
		while (ReadIndex == -1 || (ReadIndex == WriteIndex && !bWaitingForRead))
		{
			this_thread::yield();
		}

		// obj指针非空则赋值.
		if (obj != nullptr)
		{
			*obj = Objects[ReadIndex];
		}

		// 重置
		Objects[ReadIndex] = 0;

		// 更新读索引,通知可能已经阻塞的线程.
		ReadIndex = (ReadIndex + 1) % Objects.size();
		bWaitingForRead = false;
		return FPerformanceCounter::GetSeconds(timeStamp);
	}
	
	template<typename T>
	T & TSynchronizer<T>::Ref()
	{
		return Objects[WriteIndex];
	}
}