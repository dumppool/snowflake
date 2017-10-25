/*
* file CommandQueue.h
*
* author luoxw
* date 2017/08/08
*
*
*/

#pragma once

namespace LostCore
{
	template <typename TCmd>
	class FCommandQueue
	{
	public:
		FORCEINLINE explicit FCommandQueue(bool threadSafe = false);
		FORCEINLINE ~FCommandQueue();

		FORCEINLINE uint32 GetQueueNums() const;
		FORCEINLINE void Swap(FCommandQueue<TCmd> & rhs);

		FORCEINLINE void Push(const TCmd& cmd);
		FORCEINLINE bool Pop(TCmd& cmd);

	private:
		FORCEINLINE bool IsThreadSafe() const;
		FORCEINLINE void Lock();
		FORCEINLINE void Unlock();

		mutex* Mutex;

		queue<TCmd> Commands;
	};

	template <typename TCmd>
	FCommandQueue<TCmd>::FCommandQueue(bool threadSafe)
		: Mutex(threadSafe ? new mutex : nullptr)
		, Commands()
	{
	}

	template <typename TCmd>
	FCommandQueue<TCmd>::~FCommandQueue()
	{
		SAFE_DELETE(Mutex);
	}

	template <typename TCmd>
	bool FCommandQueue<TCmd>::IsThreadSafe() const
	{
		return Mutex != nullptr;
	}

	template <typename TCmd>
	uint32 FCommandQueue<TCmd>::GetQueueNums() const
	{
		return Commands.size();
	}

	template<typename TCmd>
	void FCommandQueue<TCmd>::Lock()
	{
		if (Mutex != nullptr)
		{
			Mutex->lock();
		}
	}

	template<typename TCmd>
	void FCommandQueue<TCmd>::Unlock()
	{
		if (Mutex != nullptr)
		{
			Mutex->unlock();
		}
	}

	template<typename TCmd>
	void FCommandQueue<TCmd>::Swap(FCommandQueue<TCmd>& rhs)
	{
		Lock();
		rhs.Lock();

		Commands.swap(rhs.Commands);

		rhs.Unlock();
		Unlock();
	}

	template<typename TCmd>
	void FCommandQueue<TCmd>::Push(const TCmd & cmd)
	{
		Lock();
		Commands.push(cmd);
		Unlock();
	}

	template<typename TCmd>
	bool FCommandQueue<TCmd>::Pop(TCmd & cmd)
	{
		Lock();

		bool success = false;
		if (Commands.size() > 0)
		{
			cmd = Commands.front();
			Commands.pop();
			success = true;
		}

		Unlock();
		return success;
	}
}

