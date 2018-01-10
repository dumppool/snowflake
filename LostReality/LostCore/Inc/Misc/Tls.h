/*
* file Tls.h
*
* author luoxw
* date 2017/10/19
*
* 1. �̱߳��ص���ģ��.
* 2. Inc�µĴ����ǿ��Ա�����ģ�������,Ϊ��ʹ�������Կ�ģ��.
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