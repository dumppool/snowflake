/*
* file PerformanceCounters.h
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

		static FORCEINLINE LARGE_INTEGER GetTimeStamp()
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
			return FPerformanceCounter::GetSeconds(FPerformanceCounter::GetTimeStamp().QuadPart - start.QuadPart);
		}
	};
}