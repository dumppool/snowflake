/*
* file ExportTest.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once

namespace LostCore
{
	class ITestBasic
	{
	public:
		virtual ~ITestBasic() {}
		virtual int MemFunc1(int val) = 0;
		virtual float MemFunc2(float val) = 0;
	};

	extern "C" LOSTCORE_API EReturnCode CreateHuman(ITestBasic** ppObj);
	static TExportFuncWrapper<ITestBasic**> WrappedCreateHuman("CreateHuman", &GetModule_LostCore);

	extern "C" LOSTCORE_API EReturnCode DestroyHuman(ITestBasic* pObj);
	static TExportFuncWrapper<ITestBasic*> WrappedDestroyHuman("DestroyHuman", &GetModule_LostCore);

	extern "C" LOSTCORE_API EReturnCode CreateMan(ITestBasic** ppObj);
	static TExportFuncWrapper<ITestBasic**> WrappedCreateMan("CreateMan", &GetModule_LostCore);

	extern "C" LOSTCORE_API EReturnCode DestroyMan(ITestBasic* pObj);
	static TExportFuncWrapper<ITestBasic*> WrappedDestroyMan("DestroyMan", &GetModule_LostCore);
}