/*
* file TickGroup.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#include "stdafx.h"
#include "TickGroup.h"

using namespace LostCore;

LostCore::FTickBase::FTickBase(float cycle)
	: Cycle(cycle)
	, Elapsed(0.0f)
{
	FTickGroup::Get()->AddTickObject(this);
}

LostCore::FTickBase::~FTickBase()
{
	FTickGroup::Get()->DelTickObject(this);
}

void LostCore::FTickBase::TryTick(float seconds)
{
	assert(!IsZero(Cycle));
	bool canTick = (Elapsed + seconds) > Cycle;
	Elapsed = InCycle(Elapsed + seconds, Cycle);
	if (canTick)
	{
		Tick();
	}
}

LostCore::FTickGroup::FTickGroup()
	: TimeStamp({ 0 })
{

}

void LostCore::FTickGroup::Tick()
{
	if (TimeStamp.QuadPart == 0)
	{
		TimeStamp = FPerformanceCounter::GetTimeStamp();
		return;
	}

	auto stamp = FPerformanceCounter::GetTimeStamp();
	auto elapsed = FPerformanceCounter::GetSeconds(stamp.QuadPart - TimeStamp.QuadPart);
	TimeStamp = stamp;
	for (auto item : TickObjects)
	{
		item->TryTick(elapsed);
	}
}

void LostCore::FTickGroup::AddTickObject(FTickBase* obj)
{
	TickObjects.push_back(obj);
}

void LostCore::FTickGroup::DelTickObject(FTickBase* obj)
{
	auto it = find(TickObjects.begin(), TickObjects.end(), obj);
	if (it != TickObjects.end())
	{
		TickObjects.erase(it);
	}
}
