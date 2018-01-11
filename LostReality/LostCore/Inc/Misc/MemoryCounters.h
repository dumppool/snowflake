/*
* file MemoryCounters.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#pragma once

namespace LostCore
{
	typedef tuple<int32, int32> FUsage;
	typedef map<string, tuple<int32, int32>> FNameUsageMap;
	class FMemoryCounterManager : public TProcessUniqueSingleton<FMemoryCounterManager, 1>
	{
	public:
		FORCEINLINE virtual void Tick() override;
		FORCEINLINE void Update(const string& name, int32 delta);
		FORCEINLINE FNameUsageMap GetMemoryUsage() const;

		static FORCEINLINE vector<string> GetInfoHeader();

	private:
		FNameUsageMap MemoryUsage;
		mutex Mutex;
	};

	void FMemoryCounterManager::Tick()
	{

	}

	void FMemoryCounterManager::Update(const string& name, int32 delta)
	{
		lock_guard<mutex> lck(Mutex);
		auto it = MemoryUsage.find(name);
		if (it == MemoryUsage.end())
		{
			assert(delta > 0);
			it = MemoryUsage.insert(FNameUsageMap::value_type(name, FUsage(1, delta))).first;
		}
		else
		{
			std::get<0>(it->second) += delta > 0 ? 1 : -1;
			std::get<1>(it->second) += delta;
		}
	}

	FNameUsageMap FMemoryCounterManager::GetMemoryUsage() const
	{
		return MemoryUsage;
	}

	vector<std::string> FMemoryCounterManager::GetInfoHeader()
	{
		static vector<string> header;
		if (header.empty())
		{
			header.push_back("Name");
			header.push_back("Count");
			header.push_back("Usage");
		}
		
		return header;
	}

#if ENABLE_MEMORY_COUNTER
#define MEMORY_ALLOC(cls)\
void* operator new(uint32 sz)\
{\
LostCore::FMemoryCounterManager::Get()->Update(#cls, sz);\
return malloc(sz);\
}\
void operator delete(void* p)\
{\
LostCore::FMemoryCounterManager::Get()->Update(#cls, -static_cast<int32>(sizeof(cls))); \
free(p);\
}
#else
#define MEMORY_ALLOC(name)
#endif
}