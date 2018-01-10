/*
* file TickGroup.h
*
* author luoxw
* date 2018/01/01
*
*
*/

#pragma once

namespace LostCore
{
	class FTickBase
	{
	public:
		explicit FTickBase(float cycle);
		~FTickBase();

		virtual void TryTick(float seconds);

	protected:
		virtual void Tick() = 0;

	private:
		float Cycle;
		float Elapsed;
	};

	class FTickGroup : public TTlsSingleton<FTickGroup, 2>
	{
	public:
		FTickGroup();

		virtual void Tick() override;

		void AddTickObject(FTickBase* obj);
		void DelTickObject(FTickBase* obj);

	private:
		vector<FTickBase*> TickObjects;
		LARGE_INTEGER TimeStamp;
	};
}

