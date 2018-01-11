/*
* file StackCounters.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

namespace LostCore
{
	struct FStackCounterRequest
	{
		string Name;
		int32 RequestId;

		FORCEINLINE FStackCounterRequest(const string& name);
		FORCEINLINE ~FStackCounterRequest();

		FORCEINLINE void Start();
		FORCEINLINE void Stop();
	};

	struct FScopedStackCounterRequest
	{
		FStackCounterRequest* Request;
		FORCEINLINE FScopedStackCounterRequest(FStackCounterRequest& request) : Request(&request)
		{
			Request->Start();
		}

		FORCEINLINE ~FScopedStackCounterRequest()
		{
			Request->Stop();
		}
	};

	struct FStackCounter
	{
		static const int32 SMaxNameLen = 20;
		static vector<string> GetInfoHeader();

		string Name;
		int32 RequestId;

		LARGE_INTEGER Stamp;
		double Past;
		int32 Count;
		int32 Depth;

		bool bUnFold;

		FStackCounter* ParentCounter;
		multimap<int32, FStackCounter*> ChildCounters;

		FORCEINLINE FStackCounter();
		FORCEINLINE ~FStackCounter();

		FORCEINLINE void Start(FStackCounter* parentCounter);
		FORCEINLINE FStackCounter* Stop();

		FORCEINLINE int32 GetDepth() const;
		FORCEINLINE void GetChildCounters(vector<FStackCounter*>& counters) const;
		FORCEINLINE void GetVisibleChildCounters(vector<FStackCounter*>& counters) const;
		FORCEINLINE vector<string> GetDescs(const string& indent) const;

		FORCEINLINE bool IsLeaf() const;
		FORCEINLINE void MergeLeaves();
	};

	typedef pair<string, vector<vector<string>>> FStackInfo;
	class FStackCounterCollector : public TProcessUniqueSingleton<FStackCounterCollector, 2>
	{
	public:
		FORCEINLINE virtual void Tick() override;

		FORCEINLINE string GetName(const string& threadName) const;
		FORCEINLINE void OnNotified(const FStackInfo& info);

		FORCEINLINE vector<string> GetStackNames();
		FORCEINLINE FStackInfo GetStackInfo(const string& name);

	private:
		map<string, FStackInfo> StackInfos;
		mutex StackInfoMutex;
	};

	class FStackCounterManager : public TTlsSingleton<FStackCounterManager, 1>
	{

	public:
		static const int32 SMaxID = (1 << 20);
		static const int32 SRoot = 0;
		static const int32 SOthers = 1;

		FORCEINLINE FStackCounterManager();
		FORCEINLINE virtual ~FStackCounterManager() override;

		FORCEINLINE virtual void Tick() override;

		FORCEINLINE void Start(const FStackCounterRequest& request);
		FORCEINLINE void Stop(const FStackCounterRequest& request);

		FORCEINLINE int32 AllocRequestId();
		FORCEINLINE void DeallocRequestId(int32 id);

		FORCEINLINE FStackCounter* AllocCounter();
		FORCEINLINE void DeallocCounter(FStackCounter* counter);

		FORCEINLINE void Finish();
		FORCEINLINE void RecycleCounters();

	private:

		int32 LastAllocatedID;
		vector<int32> IDPool;

		// Frame data.
		FStackCounter* Current;
		FStackCounter Root;
	};

	FStackCounterRequest::FStackCounterRequest(const string& name)
		: RequestId(FStackCounterManager::Get()->AllocRequestId())
		, Name(name)
	{
		auto tid = this_thread::get_id();
		auto mgrAddr = FStackCounterManager::Get();
		LVDEBUG("FStackCounterRequest::FStackCounterRequest", "thread: %d, manager address: 0x%08x %s.", tid, mgrAddr, Name.c_str());
	}

	FStackCounterRequest::~FStackCounterRequest()
	{
		FStackCounterManager::Get()->DeallocRequestId(RequestId);
	}

	void FStackCounterRequest::Start()
	{
		FStackCounterManager::Get()->Start(*this);
	}

	void FStackCounterRequest::Stop()
	{
		FStackCounterManager::Get()->Stop(*this);
	}

	FStackCounter::FStackCounter() 
		: ParentCounter(nullptr)
		, bUnFold(true)
		, Count(0)
		, Depth(0)
	{
		ChildCounters.clear();
	}

	FStackCounter::~FStackCounter()
	{
	}

	void FStackCounter::Start(FStackCounter* parentCounter)
	{
		Stamp = FPerformanceCounter::GetTimeStamp();
		ChildCounters.clear();
		Count = 1;
		ParentCounter = parentCounter;
		Depth = 1;
		if (ParentCounter != nullptr)
		{
			ParentCounter->ChildCounters.insert(pair<int32, FStackCounter*>(RequestId, this));
			Depth = ParentCounter->GetDepth() + 1;
		}
	}

	FStackCounter* FStackCounter::Stop()
	{
		assert(Stamp.QuadPart != 0);
		Past = FPerformanceCounter::GetSeconds(Stamp);
		return ParentCounter;
	}

	int32 FStackCounter::GetDepth() const
	{
		return Depth;
	}

	void FStackCounter::GetChildCounters(vector<FStackCounter*>& counters) const
	{
		for (auto it = ChildCounters.begin(); it != ChildCounters.end(); it++)
		{
			counters.push_back(it->second);
			it->second->GetChildCounters(counters);
		}
	}

	void FStackCounter::GetVisibleChildCounters(vector<FStackCounter*>& counters) const
	{
		if (bUnFold)
		{
			for (auto item : ChildCounters)
			{
				counters.push_back(item.second);
				item.second->GetVisibleChildCounters(counters);
			}
		}
	}

	FORCEINLINE vector<string> FStackCounter::GetInfoHeader()
	{
		static vector<string> result;
		if (result.empty())
		{
			result.push_back("Name");
			result.push_back("Count");
			result.push_back("Percentage");
			result.push_back("Milliseconds");
			result.push_back("Depth");
		}

		return result;
	}

	vector<string> FStackCounter::GetDescs(const string & indent) const
	{
		string name(indent);
		name.append(string(4 * (GetDepth() - 1), ' ')).append(Name);
		float percentage = Past * 100 / (ParentCounter != nullptr ? ParentCounter->Past : Past);

		vector<string> result;
		result.push_back(name);
		result.push_back(to_string(Count));
		result.push_back(to_string(percentage));
		result.push_back(to_string(Past * 1000));
		result.push_back(to_string(Depth));
		return result;
	}

	bool FStackCounter::IsLeaf() const
	{
		return ChildCounters.empty();
	}

	void FStackCounter::MergeLeaves()
	{
		if (ChildCounters.empty())
		{
			return;
		}

		auto lastIt = ChildCounters.begin();
		auto it = lastIt;
		it++;
		while (it != ChildCounters.end())
		{
			if (it->first != lastIt->first)
			{
				lastIt = it++;
			}
			else if (it->second->IsLeaf() && !lastIt->second->IsLeaf())
			{
				lastIt = it++;
			}
			else if (it->second->IsLeaf() && lastIt->second->IsLeaf())
			{
				lastIt->second->Past += it->second->Past;
				lastIt->second->Count++;
				FStackCounterManager::Get()->DeallocCounter(it->second);
				it = ChildCounters.erase(it);
			}
			else
			{
				it++;
			}
		}

		auto pastAll = 0.0;
		for (auto item : ChildCounters)
		{
			pastAll += item.second->Past;
			item.second->MergeLeaves();
		}

		auto counter = FStackCounterManager::Get()->AllocCounter();
		counter->RequestId = FStackCounterManager::SOthers;
		counter->Name = "Others";
		counter->Past = Past - pastAll;
		counter->ParentCounter = this;
		counter->Depth = GetDepth() + 1;
		ChildCounters.insert(pair<int32, FStackCounter*>(counter->RequestId, counter));
	}

	FStackCounterManager::FStackCounterManager()
		: Current(nullptr)
		, LastAllocatedID(SOthers)
		, IDPool(0)
	{
		Root.Name = "Root";
		Root.RequestId = SRoot;
		Root.Start(nullptr);
		Current = &Root;
	}

	FStackCounterManager::~FStackCounterManager()
	{

	}

	void FStackCounterManager::Tick()
	{
		Finish();
	}

	void FStackCounterManager::RecycleCounters()
	{
		assert(Current == &Root || Current == nullptr);

		vector<FStackCounter*> counters;
		Root.GetChildCounters(counters);
		for (auto item : counters)
		{
			DeallocCounter(item);
		}
	}

	void FStackCounterManager::Start(const FStackCounterRequest& request)
	{
		auto counter = AllocCounter();
		counter->Name = request.Name;
		counter->RequestId = request.RequestId;
		counter->Start(Current);
		Current = counter;
	}

	void FStackCounterManager::Stop(const FStackCounterRequest& request)
	{
		assert(Current != nullptr && Current->RequestId == request.RequestId);
		Current = Current->Stop();
	}

	int32 FStackCounterManager::AllocRequestId()
	{
		// LastAllocatedID增加到SMaxID前,只用LastAllocatedID.
		if (LastAllocatedID < SMaxID)
		{
			return ++LastAllocatedID;
		}

		assert(IDPool.size() > 0);

		auto id = IDPool.back();
		IDPool.pop_back();
		return id;
	}

	void FStackCounterManager::DeallocRequestId(int32 id)
	{
		IDPool.push_back(id);
	}

	FStackCounter * FStackCounterManager::AllocCounter()
	{
		return new FStackCounter;
	}

	void FStackCounterManager::DeallocCounter(FStackCounter * counter)
	{
		SAFE_DELETE(counter);
	}

	void FStackCounterManager::Finish()
	{
		Root.Stop();
		Root.MergeLeaves();

		vector<vector<string>> statistics;
		vector<FStackCounter*> counters;
		Root.GetVisibleChildCounters(counters);

		for (auto item : counters)
		{
			statistics.push_back(item->GetDescs(">> "));
		}

		FStackCounterCollector::Get()->OnNotified(
			FStackInfo(FProcessUnique::Get()->GetCurrentThread()->GetFrameInfo(), statistics));

		RecycleCounters();
		Root.Start(nullptr);
	}

	void FStackCounterCollector::Tick()
	{

	}

	std::string FStackCounterCollector::GetName(const string& threadName) const
	{
		return threadName + " Stack";
	}

	void FStackCounterCollector::OnNotified(const FStackInfo& info)
	{
		lock_guard<mutex> lck(StackInfoMutex);
		auto t = FProcessUnique::Get()->GetCurrentThread();
		auto name = GetName(t->GetName());
		StackInfos[name] = info;
	}

	std::vector<std::string> FStackCounterCollector::GetStackNames()
	{
		vector<string> result;
		for (auto& item : StackInfos)
		{
			result.push_back(item.first);
		}

		return result;
	}

	FStackInfo FStackCounterCollector::GetStackInfo(const string& name)
	{
		lock_guard<mutex> lck(StackInfoMutex);
		auto it = StackInfos.find(name);
		if (it == StackInfos.end())
		{
			return FStackInfo();
		}

		return it->second;
	}
}