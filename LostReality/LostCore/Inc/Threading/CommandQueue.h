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
	template <typename CommandType>
	class FCommandQueue
	{
	public:
		explicit FCommandQueue(bool threadSafe = false);
		~FCommandQueue();

		uint32 GetQueueNums() const;
		void Swap(FCommandQueue<CommandType> & rhs);

		void Push(const CommandType& cmd);
		bool Pop(CommandType& cmd);

	private:
		bool IsThreadSafe() const;
		void Lock();
		void Unlock();

		mutex* Mutex;

		queue<CommandType> Commands;
	};

	template <typename CommandType>
	FORCEINLINE FCommandQueue<CommandType>::FCommandQueue(bool threadSafe)
		: Mutex(threadSafe ? new mutex : nullptr)
		, Commands()
	{
	}

	template <typename CommandType>
	FORCEINLINE FCommandQueue<CommandType>::~FCommandQueue()
	{
		SAFE_DELETE(Mutex);
	}

	template <typename CommandType>
	FORCEINLINE bool FCommandQueue<CommandType>::IsThreadSafe() const
	{
		return Mutex != nullptr;
	}

	template <typename CommandType>
	FORCEINLINE uint32 FCommandQueue<CommandType>::GetQueueNums() const
	{
		return Commands.size();
	}

	template<typename CommandType>
	FORCEINLINE void FCommandQueue<CommandType>::Lock()
	{
		if (Mutex != nullptr)
		{
			Mutex->lock();
		}
	}

	template<typename CommandType>
	FORCEINLINE void FCommandQueue<CommandType>::Unlock()
	{
		if (Mutex != nullptr)
		{
			Mutex->unlock();
		}
	}

	template<typename CommandType>
	FORCEINLINE void FCommandQueue<CommandType>::Swap(FCommandQueue<CommandType>& rhs)
	{
		Lock();
		rhs.Lock();

		Commands.swap(rhs.Commands);

		rhs.Unlock();
		Unlock();
	}

	template<typename CommandType>
	FORCEINLINE void FCommandQueue<CommandType>::Push(const CommandType & cmd)
	{
		Lock();
		Commands.push(cmd);
		Unlock();
	}

	template<typename CommandType>
	FORCEINLINE bool FCommandQueue<CommandType>::Pop(CommandType & cmd)
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

