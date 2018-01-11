/*
* file Average.h
*
* author luoxw
* date 2018/01/10
*
*/

#pragma once

namespace LostCore
{
	template <typename T, int32 NumSamples>
	class TAverage
	{
	public:
		FORCEINLINE TAverage();
		FORCEINLINE void Add(T sample);
		FORCEINLINE T GetAverage() const;
		FORCEINLINE T GetLast() const;

	private:
		int32 LastIndex;
		T Samples[NumSamples];
		T Sum;
		T Average;

		static const int32 SNumSamples = NumSamples;
	};

	template <typename T, int32 NumSamples>
	TAverage<T, NumSamples>::TAverage()
		: LastIndex(-1)
		, Sum(static_cast<T>(0))
		, Average(static_cast<T>(0))
	{
		memset(Samples, 0, SNumSamples * sizeof(T));
	}

	template <typename T, int32 NumSamples>
	void TAverage<T, NumSamples>::Add(T sample)
	{
		int32 next = InCycle(LastIndex + 1, SNumSamples);
		Sum += sample - Samples[next];
		Average = Sum / SNumSamples;
		Samples[next] = sample;
		LastIndex = next;
	}

	template <typename T, int32 NumSamples>
	T TAverage<T, NumSamples>::GetAverage() const
	{
		return Average;
	}

	template <typename T, int32 NumSamples>
	T TAverage<T, NumSamples>::GetLast() const
	{
		return Samples[LastIndex];
	}

}