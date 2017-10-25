/*
* file Tls.h
*
* author luoxw
* date 2017/10/19
*
* 1. 实现tls的单例.
* 2. Inc下的代码是可以被其他模块包含的,为了使单例可以跨模块.
*/

#pragma once

namespace LostCore
{

	// T的例子,要避免SClassIndex重复确实不怎么舒服,但是不能使用编译时计数的话只能如此.
	//class T : public FTlsSingletonTemplate<T>
	//{
	//public:
	//	static const int32 SClassIndex = x;
	//}
	template <typename T>
	class FTlsSingletonTemplate
	{
		static T* SInstance;

	public:
		static FORCEINLINE T* Get()
		{
			if (SInstance == nullptr)
			{
				auto t = FProcessUnique::Get()->GetCurrentThread();
				auto p = t->GetSingleton(T::SClassIndex);
				if (p == nullptr)
				{
					p = new T;
					t->AddSingleton(T::SClassIndex, p);
				}

				SInstance = (T*)p;
			}

			assert(SInstance != nullptr);
			return SInstance;
		}
	};
}