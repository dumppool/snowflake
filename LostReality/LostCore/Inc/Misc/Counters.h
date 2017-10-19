/*
* file Counters.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

namespace LostCore
{
	class FPerformanceCounter
	{
	public:
		static FORCEINLINE LARGE_INTEGER GetFreq()
		{
			static LARGE_INTEGER SFreq = { 0, 0 };
			if (SFreq.QuadPart == 0)
			{
				QueryPerformanceFrequency(&SFreq);
			}

			return SFreq;
		}

		static FORCEINLINE LARGE_INTEGER GetStamp()
		{
			LARGE_INTEGER s;
			QueryPerformanceCounter(&s);
			return s;
		}

		static FORCEINLINE double GetSeconds(LONGLONG count)
		{
			return double(count) / FPerformanceCounter::GetFreq().QuadPart;
		}

		static FORCEINLINE double GetSeconds(LARGE_INTEGER start)
		{
			return FPerformanceCounter::GetSeconds(FPerformanceCounter::GetStamp().QuadPart - start.QuadPart);
		}
	};

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
		FORCEINLINE array<string, 2> GetDescs(const string& indent) const;

		FORCEINLINE bool IsLeaf() const;
		FORCEINLINE void MergeLeaves();
	};

	class FStackCounterManager : public FTlsSingletonTemplate<FStackCounterManager>
	{

		int32 LastAllocatedID;
		vector<int32> IDPool;

		int32 MaxNumLines;

		// Frame data.
		FStackCounter* Current;
		FStackCounter Root;
		vector<array<string, 2>> Statics;

	public:
		static const int32 SMaxID = (1 << 20);
		static const int32 SRoot = 0;
		static const int32 SOthers = 1;
		static const int32 SClassIndex = 1;

		FORCEINLINE FStackCounterManager();

		FORCEINLINE void Start(const FStackCounterRequest& request);
		FORCEINLINE void Stop(const FStackCounterRequest& request);

		FORCEINLINE int32 AllocRequestId();
		FORCEINLINE void DeallocRequestId(int32 id);

		FORCEINLINE FStackCounter* AllocCounter();
		FORCEINLINE void DeallocCounter(FStackCounter* counter);

		FORCEINLINE void SetMaxNumLines(int32 num);
		FORCEINLINE void Finish();
		FORCEINLINE void RecycleCounters();
		FORCEINLINE vector<array<string, 2>> GetDisplayContent() const;
	};

	FORCEINLINE FStackCounterRequest::FStackCounterRequest(const string& name)
		: RequestId(FStackCounterManager::Get()->AllocRequestId())
		, Name(name)
	{
		auto tid = this_thread::get_id();
		auto mgrAddr = FStackCounterManager::Get();
		LVMSG("FStackCounterRequest::FStackCounterRequest", "thread id: %d, manager address: 0x%08x.", tid, mgrAddr);
	}

	FORCEINLINE FStackCounterRequest::~FStackCounterRequest()
	{
		FStackCounterManager::Get()->DeallocRequestId(RequestId);
	}

	FORCEINLINE void FStackCounterRequest::Start()
	{
		FStackCounterManager::Get()->Start(*this);
	}

	FORCEINLINE void FStackCounterRequest::Stop()
	{
		FStackCounterManager::Get()->Stop(*this);
	}

	FORCEINLINE FStackCounter::FStackCounter() 
		: ParentCounter(nullptr)
		, bUnFold(true)
		, Count(0)
		, Depth(0)
	{
		ChildCounters.clear();
	}

	FORCEINLINE FStackCounter::~FStackCounter()
	{
	}

	FORCEINLINE void FStackCounter::Start(FStackCounter* parentCounter)
	{
		Stamp = FPerformanceCounter::GetStamp();
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

	FORCEINLINE FStackCounter* FStackCounter::Stop()
	{
		assert(Stamp.QuadPart != 0);
		Past = FPerformanceCounter::GetSeconds(Stamp);
		return ParentCounter;
	}

	FORCEINLINE int32 FStackCounter::GetDepth() const
	{
		return Depth;
	}

	FORCEINLINE void FStackCounter::GetChildCounters(vector<FStackCounter*>& counters) const
	{
		for (auto it = ChildCounters.begin(); it != ChildCounters.end(); it++)
		{
			counters.push_back(it->second);
			it->second->GetChildCounters(counters);
		}
	}

	FORCEINLINE void FStackCounter::GetVisibleChildCounters(vector<FStackCounter*>& counters) const
	{
		if (bUnFold)
		{
			for (auto item : ChildCounters)
			{
				counters.push_back(item.second);
				if (counters.size() == counters.capacity())
				{
					return;
				}

				item.second->GetVisibleChildCounters(counters);
			}
		}
	}

	FORCEINLINE array<string, 2> FStackCounter::GetDescs(const string & indent) const
	{
		string head(indent);
		head.append(string(4*(GetDepth()-1), '.')).append(Name);

		char info[128];
		memset(info, 0, 128);
		float percentage = Past * 100 / (ParentCounter != nullptr ? ParentCounter->Past : Past);
		snprintf(info, 127, "%d %0.2fPercent %0.2fMS %d", Count, percentage, Past*1000, Depth);

		return array<string, 2>({ head, info });
	}

	FORCEINLINE bool FStackCounter::IsLeaf() const
	{
		return ChildCounters.empty();
	}

	FORCEINLINE void FStackCounter::MergeLeaves()
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

	FORCEINLINE FStackCounterManager::FStackCounterManager()
		: Current(nullptr)
		, LastAllocatedID(SOthers)
		, IDPool(0)
		, MaxNumLines(10)
	{
		Root.Name = "Root";
		Root.RequestId = SRoot;
		Root.Start(nullptr);
		Current = &Root;
	}

	FORCEINLINE void FStackCounterManager::RecycleCounters()
	{
		assert(Current == &Root || Current == nullptr);

		vector<FStackCounter*> counters;
		Root.GetChildCounters(counters);
		for (auto item : counters)
		{
			DeallocCounter(item);
		}
	}

	FORCEINLINE vector<array<string,2>> FStackCounterManager::GetDisplayContent() const
	{
		return Statics;
	}

	FORCEINLINE void FStackCounterManager::Start(const FStackCounterRequest& request)
	{
		auto counter = AllocCounter();
		counter->Name = request.Name;
		counter->RequestId = request.RequestId;
		counter->Start(Current);
		Current = counter;
	}

	FORCEINLINE void FStackCounterManager::Stop(const FStackCounterRequest& request)
	{
		assert(Current != nullptr && Current->RequestId == request.RequestId);
		Current = Current->Stop();
	}

	FORCEINLINE int32 FStackCounterManager::AllocRequestId()
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

	FORCEINLINE void FStackCounterManager::DeallocRequestId(int32 id)
	{
		IDPool.push_back(id);
	}

	FORCEINLINE FStackCounter * FStackCounterManager::AllocCounter()
	{
		return new FStackCounter;
	}

	FORCEINLINE void FStackCounterManager::DeallocCounter(FStackCounter * counter)
	{
		SAFE_DELETE(counter);
	}

	FORCEINLINE void FStackCounterManager::SetMaxNumLines(int32 num)
	{
		MaxNumLines = num;
	}

	FORCEINLINE void FStackCounterManager::Finish()
	{
		Root.Stop();
		Root.MergeLeaves();

		Statics.clear();
		vector<FStackCounter*> counters;
		counters.reserve(MaxNumLines);
		Root.GetVisibleChildCounters(counters);

		for (auto item : counters)
		{
			Statics.push_back(item->GetDescs(">> "));
		}

		RecycleCounters();
		Root.Start(nullptr);
	}
}