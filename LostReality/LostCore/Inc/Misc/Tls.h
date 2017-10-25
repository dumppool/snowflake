/*
* file Tls.h
*
* author luoxw
* date 2017/10/19
*
* 1. ʵ��tls�ĵ���.
* 2. Inc�µĴ����ǿ��Ա�����ģ�������,Ϊ��ʹ�������Կ�ģ��.
*/

#pragma once

namespace LostCore
{

	// T������,Ҫ����SClassIndex�ظ�ȷʵ����ô���,���ǲ���ʹ�ñ���ʱ�����Ļ�ֻ�����.
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