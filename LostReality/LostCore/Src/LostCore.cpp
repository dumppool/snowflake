// LostCore.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LostCore.h"

namespace LostCore
{
	class FHuman : public ITestBasic
	{
		int HumanInt;
		float HumanFloat;

	public:

		FHuman()
		{
			const char* head = "FHuman::Contructor";
			LVMSG(head, "0x%08x", (__int3264)this);
		}

		virtual ~FHuman()
		{
			const char* head = "FHuman::Destrtuctor";
			LVMSG(head, "0x%08x", (__int3264)this);
		}

		int MemFunc1(int val) override
		{
			const char* head = "FHuman::MemFunc1";
			LVMSG(head, "%d", val);
			OutputLayout(head);
			return val;
		}

		float MemFunc2(float val) override
		{
			const char* head = "FHuman::MemFunc2";
			LVMSG(head, "%f", val);
			OutputLayout(head);
			return val;
		}

		void OutputLayout(const char* head)
		{
			LVMSG(head, "this:\t\t0x%08x", (__int3264)this);
			LVMSG(head, "HumanInt:\t0x%08x", (__int3264)&this->HumanInt);
			LVMSG(head, "HumanFloat:\t0x%08x", (__int3264)&this->HumanFloat);
			LVMSG(head, "vtable head:\t0x%08x", (__int3264)*(__int3264*)this);
			LVMSG(head, "vtable 1st:\t0x%08x", (__int3264)*((__int3264*)*(__int3264*)this + 0));
			LVMSG(head, "vtable 2nd:\t0x%08x", (__int3264)*((__int3264*)*(__int3264*)this + 1));
		}
	};

	class FMan : public FHuman
	{
		int Dummy;

	public:
		int MemFunc1(int val) override
		{
			const char* head = "FMan::MemFunc1";
			LVMSG(head, "%d", val);
			OutputLayout(head);
			return val;
		}

		float MemFunc2(float val) override
		{
			const char* head = "FMan::MemFunc2";
			LVMSG(head, "%f", val);
			OutputLayout(head);
			return val;
		}

		void OutputLayout(const char* head)
		{
			this->FHuman::OutputLayout(head);
			LVMSG(head, "this:\t\t0x%08x", (__int3264)this);
			LVMSG(head, "Dummy:\t\t0x%08x", (__int3264)&this->Dummy);
			LVMSG(head, "vtable head:\t0x%08x", (__int3264)*(__int3264*)this);
			LVMSG(head, "vtable 1st:\t0x%08x", (__int3264)*((__int3264*)*(__int3264*)this + 0));
			LVMSG(head, "vtable 2nd:\t0x%08x", (__int3264)*((__int3264*)*(__int3264*)this + 1));
		}
	};
}

LOSTCORE_API EReturnCode LostCore::CreateHuman(ITestBasic** ppObj)
{
	*ppObj = (new FHuman);
	return 0;
}

LOSTCORE_API EReturnCode LostCore::DestroyHuman(ITestBasic* pObj)
{
	delete pObj;
	return 0;
}

LOSTCORE_API EReturnCode LostCore::CreateMan(ITestBasic** ppObj)
{
	*ppObj = (new FMan);
	return 0;
}

LOSTCORE_API EReturnCode LostCore::DestroyMan(ITestBasic* pObj)
{
	delete pObj;
	return 0;
}
