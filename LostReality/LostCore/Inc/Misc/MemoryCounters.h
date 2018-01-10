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
	class FMemoryCounterManager : public TProcessUniqueSingleton<FMemoryCounterManager, 1>
	{
	public:
		FORCEINLINE virtual void Tick() override;
		FORCEINLINE void Update(const string& name, int32 delta);
		FORCEINLINE map<string, int32> GetMemoryUsage() const;

	private:
		map<string, int32> MemoryUsage;
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
			it = MemoryUsage.insert(pair<string, int32>(name, delta)).first;
		}
		else
		{
			it->second += delta;
		}
	}

	map<string, int32> FMemoryCounterManager::GetMemoryUsage() const
	{
		return MemoryUsage;
	}

#if ENABLE_MEMORY_COUNTER
#define MEMORY_ALLOC(cls)\
void* operator new(uint32 sz)\
{\
FMemoryCounterManager::Get()->Update(#cls, sz);\
return malloc(sz);\
}\
void operator delete(void* p)\
{\
FMemoryCounterManager::Get()->Update(#cls, -static_cast<int32>(sizeof(cls))); \
free(p);\
}
#else
#define MEMORY_ALLOC(name)
#endif
}