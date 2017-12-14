/*
* file IDAllocator.h
*
* author luoxw
* date 2017/12/06
*
*
*/

#pragma once

namespace LostCore
{
	class FIDAllocator
	{
	public:
		FIDAllocator();

		int32 Alloc();
		void Dealloc(int32 id);

		void Reset();

	private:
		int32 LastAllocatedId;
		vector<int32> Pool;

		static const int32 SMax = 1 << 30;
	};

	FORCEINLINE FIDAllocator::FIDAllocator()
		: LastAllocatedId(-1)
	{

	}

	FORCEINLINE int32 FIDAllocator::Alloc()
	{
		if (LastAllocatedId < SMax)
		{
			return ++LastAllocatedId;
		}

		assert(Pool.size() > 0);

		auto id = Pool.back();
		Pool.pop_back();
		return id;
	}

	FORCEINLINE void FIDAllocator::Dealloc(int32 id)
	{
		Pool.push_back(id);
	}

	FORCEINLINE void FIDAllocator::Reset()
	{
		LastAllocatedId = -1;
		Pool.clear();
	}

}