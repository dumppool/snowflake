/*
* file Counters.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once


class FHighFrequencyCounter
{
	LARGE_INTEGER TicksPerSec;
	LARGE_INTEGER Timestamp;

public:
	FORCEINLINE FHighFrequencyCounter();

	FORCEINLINE void Start();

	//************************************
	// Method:    Stop
	// FullName:  FHighFrequencyCounter::Stop
	// Access:    public 
	// Returns:   double in ms
	// Qualifier:
	//************************************
	FORCEINLINE double Stop();

	FORCEINLINE double Count();

	FORCEINLINE bool Started() const;
};

class FScopedHighFrequencyCounter
{
	const CHAR* MsgHead;
	FHighFrequencyCounter Counter;

public:
	FORCEINLINE FScopedHighFrequencyCounter(const CHAR* head);
	FORCEINLINE ~FScopedHighFrequencyCounter();
};

FORCEINLINE FScopedHighFrequencyCounter::FScopedHighFrequencyCounter(const CHAR * head) : MsgHead(head), Counter()
{
	Counter.Start();
}

FORCEINLINE FScopedHighFrequencyCounter::~FScopedHighFrequencyCounter()
{
	double duration = Counter.Stop();
	{
#if 0
		LVMSG("FScopedHighFrequencyCounter", "%s\t\t%fms", MsgHead ? MsgHead : "<null>", duration);
#endif
	}
}

FORCEINLINE FHighFrequencyCounter::FHighFrequencyCounter()
{
	Timestamp.QuadPart = 0;
	QueryPerformanceFrequency(&TicksPerSec);
}

FORCEINLINE void FHighFrequencyCounter::Start()
{
	QueryPerformanceCounter(&Timestamp);
}

FORCEINLINE double FHighFrequencyCounter::Stop()
{
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	return (double)((end.QuadPart - Timestamp.QuadPart) * 1000.0 / TicksPerSec.QuadPart);
}

FORCEINLINE double FHighFrequencyCounter::Count()
{
	LARGE_INTEGER begin, end;
	begin = Timestamp;
	QueryPerformanceCounter(&Timestamp);
	QueryPerformanceCounter(&end);
	return (double)((end.QuadPart - begin.QuadPart) * 1000.0 / TicksPerSec.QuadPart);
}

FORCEINLINE bool FHighFrequencyCounter::Started() const
{
	return Timestamp.QuadPart != 0;
}
