/*
* file Tls.h
*
* author luoxw
* date 2017/10/19
*
* 1. 线程本地单例模板.
* 2. Inc下的代码是可以被其他模块包含的,为了使单例可以跨模块.
*/

#pragma once

namespace LostCore
{

	template <typename T, int32 ClassIndex>
	class TTlsSingleton : public ITickable
	{
	public:
		static const int32 SClassIndex = ClassIndex;

		static T* Get()
		{
			auto t = FProcessUnique::Get()->GetCurrentThread();
			auto p = t->GetSingleton(T::SClassIndex);
			if (p == nullptr)
			{
				p = new T;
				t->AddSingleton(T::SClassIndex, p);
			}

			return dynamic_cast<T*>(p);
		}
	};
}