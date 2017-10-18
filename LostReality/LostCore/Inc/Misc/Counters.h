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
		FORCEINLINE string ToString(const string& indent, int32 numLayers) const;
	};

	class FStackCounterManager
	{
		static const int32 SMaxID = (1 << 20);
		static const int32 SRoot = 0;
		static const int32 SOthers = 1;

		int32 LastAllocatedID;
		vector<int32> IDPool;

		// Frame data.
		FStackCounter* Current;
		FStackCounter Root;
		vector<string> Statics;

	public:
		static FStackCounterManager* Get()
		{
			static FStackCounterManager SInst;
			return &SInst;
		}

		FORCEINLINE FStackCounterManager();

		FORCEINLINE void Start(const FStackCounterRequest& request);
		FORCEINLINE void Stop(const FStackCounterRequest& request);

		FORCEINLINE int32 AllocRequestId();
		FORCEINLINE void DeallocRequestId(int32 id);

		FORCEINLINE FStackCounter* AllocCounter();
		FORCEINLINE void DeallocCounter(FStackCounter* counter);

		FORCEINLINE void Finish();
		FORCEINLINE void EndFrame();
		FORCEINLINE vector<string> GetDisplayContent() const;
	};

	FORCEINLINE FStackCounterRequest::FStackCounterRequest(const string& name)
		: RequestId(FStackCounterManager::Get()->AllocRequestId())
		, Name(name)
	{
		auto tid = this_thread::get_id();
		auto mgrAddr = FStackCounterManager::Get();
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
		ParentCounter = parentCounter;
		if (ParentCounter != nullptr)
		{
			ParentCounter->ChildCounters.insert(pair<int32, FStackCounter*>(RequestId, this));
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
		if (ChildCounters.size() == 0)
		{
			return 1;
		}

		return ChildCounters.begin()->second->GetDepth() + 1;
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
				item.second->GetVisibleChildCounters(counters);
			}
		}
	}

	FORCEINLINE string FStackCounter::ToString(const string & indent, int32 numLayers) const
	{
		const int32 rightStart = numLayers * string("....").length() + FStackCounter::SMaxNameLen;
		int32 sepLen = rightStart;
		sepLen -= indent.length();
		sepLen -= Name.length();
		sepLen = (sepLen > 4 ? sepLen : 4);
		string sep(sepLen, '.');

		string info(indent + Name + sep);
		float percentage = Past * 100 / (ParentCounter != nullptr ? ParentCounter->Past : Past);
		info.append(to_string(percentage)).append("%").append("....");
		info.append(to_string((float)Past * 1000.0f)).append("MS");
		return info;
	}

	FORCEINLINE FStackCounterManager::FStackCounterManager()
		: Current(nullptr)
		, LastAllocatedID(SOthers)
		, IDPool(0)
	{
		Root.Name = "Root";
		Root.RequestId = SRoot;
		Root.Start(nullptr);
		Current = &Root;
	}

	FORCEINLINE void FStackCounterManager::EndFrame()
	{
		assert(Current == &Root);

		vector<FStackCounter*> counters;
		Root.GetChildCounters(counters);
		for (auto item : counters)
		{
			DeallocCounter(item);
		}
	}

	FORCEINLINE vector<string> FStackCounterManager::GetDisplayContent() const
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

	FORCEINLINE void FStackCounterManager::Finish()
	{
		// TODO: 合并相同request id的叶子节点.
		Root.Stop();

		Statics.clear();
		vector<FStackCounter*> counters;
		Root.GetVisibleChildCounters(counters);

		auto depth = Root.GetDepth();
		for (auto item : counters)
		{
			Statics.push_back(item->ToString(">> ", depth));
		}

		Root.Start(nullptr);
	}
}