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
	class FThread;

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

	class ITickTask
	{
	public:
		virtual bool Initialize() = 0;
		virtual void Tick() = 0;
		virtual void Destroy() = 0;
	};

	class FProcessUnique : public ITickTask
	{
	public:
		static FORCEINLINE FProcessUnique* Get()
		{
			return SInstance;
		}

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

		static FORCEINLINE void SetInstance(FProcessUnique* instance)
		{
			SInstance = instance;
			SIsOriginal = false;
		}

	public:
		FORCEINLINE FProcessUnique();
		FORCEINLINE ~FProcessUnique();

		FORCEINLINE void AddThread(FThread* t);
		FORCEINLINE void RemoveThread(FThread* t);
		FORCEINLINE FThread* GetCurrentThread();

		// Inherited via ITickTask
		FORCEINLINE virtual bool Initialize() override;
		FORCEINLINE virtual void Tick() override;
		FORCEINLINE virtual void Destroy() override;

	private:
	
		map<thread::id, FThread*> ThreadMap;
		mutex ThreadMapMutex;

		typedef function<void()> FCmd;
		FCommandQueue<FCmd> Commands;

		FThread* EntryThread;

		static bool SIsOriginal;
		static FProcessUnique* SInstance;

	};

	class IPayload
	{
	public:
		virtual void Execute() = 0;
		virtual void OnFinished(double sec) = 0;
		virtual bool IsThreadPrivate() = 0;
	};

	class FThread
	{
		class FPrivate : public IPayload
		{
		public:
			FORCEINLINE explicit FPrivate(const function<void()>& payload);

			// Inherited via IPayload
			FORCEINLINE virtual void Execute() override;
			FORCEINLINE virtual void OnFinished(double sec) override;
			FORCEINLINE virtual bool IsThreadPrivate() override;

		private:
			function<void()> Payload;
		};

	public:
		FORCEINLINE FThread();
		FORCEINLINE FThread(IPayload* task, const string& name, uint32 affinityMask = 0xff);
		FORCEINLINE FThread(const function<void()>& task, const string& name, uint32 affinityMask = 0xff);
		FORCEINLINE virtual ~FThread();

		FORCEINLINE virtual thread::id GetId() const;
		FORCEINLINE virtual string GetName() const;
		FORCEINLINE virtual void SetTickSeconds(double sec);
		FORCEINLINE virtual double GetTickSeconds() const;
		FORCEINLINE virtual void* GetSingleton(int32 index);
		FORCEINLINE virtual void AddSingleton(int32 index, void* singleton);
		FORCEINLINE virtual void SetAffinity(uint32 mask);
		FORCEINLINE virtual uint32 GetAffinity() const;
		FORCEINLINE virtual IPayload* GetPayload();

	protected:
		// Thread线程执行.
		FORCEINLINE void Run();
		FORCEINLINE void Destroy();

	private:
		typedef map<int32, void*> FIndexedAddress;

		IPayload* Payload;
		string Name;
		bool bRunning;
		FIndexedAddress IndexedSingletonMap;
		double TickSeconds;
		uint32 AffinityMask;

	protected:
		thread Thread;
	};

	class FTickThread : public FThread
	{
	public:
		FORCEINLINE FTickThread(ITickTask* task, const string& name, uint32 affinityMask = 0xff);
		FORCEINLINE virtual ~FTickThread() override;

	private:
		// Thread线程执行.
		FORCEINLINE void RunTick();

	private:

		ITickTask* Task;
		bool bRunning;
	};

	FORCEINLINE int32 GetThreadId(thread::id id)
	{
		stringstream ss;
		ss << id;
		return stoi(ss.str());
	}

	FProcessUnique::FProcessUnique()
		: Commands(true)
		, EntryThread(new FTickThread(this, "FProcessUnique"))
	{
	}
	
	FProcessUnique::~FProcessUnique()
	{
		SAFE_DELETE(EntryThread);
		assert(ThreadMap.empty());
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
	}

	FThread::FPrivate::FPrivate(const function<void()>& payload)
		: Payload(payload)
	{}

	void FThread::FPrivate::Execute()
	{
		assert(Payload != nullptr);
		Payload();
	}

	void FThread::FPrivate::OnFinished(double sec)
	{
	}

	bool FThread::FPrivate::IsThreadPrivate()
	{
		return true;
	}

	FThread::FThread()
		: Name("unnamed")
		, Payload(nullptr)
	{
	}

	FThread::FThread(IPayload * task, const string & name, uint32 affinityMask)
		: Payload(task)
		, Name(name)
		, AffinityMask(affinityMask)
	{
		if (task != nullptr)
		{
			Thread = thread([&]() {Run(); });
		}
	}

	FThread::FThread(const function<void()>& task, const string& name, uint32 affinityMask)
		: Name(name)
		, AffinityMask(affinityMask)
	{
		if (task != nullptr)
		{
			Payload = new FPrivate(task);
			this_thread::yield();
			Thread = thread([&]() {Run(); });
		}
	}

	FThread::~FThread()
	{
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

	void FThread::SetTickSeconds(double sec)
	{
		TickSeconds = sec;
	}

	double FThread::GetTickSeconds() const
	{
		return TickSeconds;
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

	IPayload * FThread::GetPayload()
	{
		return Payload;
	}

	void FThread::Run()
	{
		auto timeStamp = FPerformanceCounter::GetTimeStamp();

		assert(Payload != nullptr);
		this_thread::sleep_for(chrono::milliseconds(1));
		SetAffinity(AffinityMask);
		FProcessUnique::Get()->AddThread(this);

		Payload->Execute();

		Destroy();

		TickSeconds = FPerformanceCounter::GetSeconds(timeStamp);
		Payload->OnFinished(TickSeconds);

		if (Payload->IsThreadPrivate())
		{
			SAFE_DELETE(Payload);
		}

		Payload = nullptr;
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

	FTickThread::FTickThread(ITickTask* task, const string& name, uint32 affinityMask)
		: FThread(nullptr, name, affinityMask)
		, Task(task)
		, bRunning(true)
	{
		if (task != nullptr)
		{
			Thread = thread([&]() {RunTick(); });
		}
	}

	FTickThread::~FTickThread()
	{
		bRunning = false;
	}
	
	void FTickThread::RunTick()
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
		while (bRunning)
		{
			SetTickSeconds(FPerformanceCounter::GetSeconds(timeStamp));

			timeStamp = FPerformanceCounter::GetTimeStamp();
			Task->Tick();
			this_thread::sleep_for(chrono::microseconds(10));
		}

		Task->Destroy();
		Task = nullptr;

		Destroy();
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

		// 更新写索引,通知可能已经阻塞的线程.
		WriteIndex = (WriteIndex + 1) % Objects.size();

		// 如果写索引指向不可写入,等待其他线程同步读取后通知,确保写索引总是可写入.		
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

		// 如果读索引指向不可读取,等待别的线程同步提交后通知.
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