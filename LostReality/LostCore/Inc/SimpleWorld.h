/*
* file SimpleWorld.h
*
* author luoxw
* date 2017/02/05
*
*
*/

#pragma once
#include "BasicWorld.h"

namespace LostCore
{
	extern "C" LOSTCORE_API void CreateSimpleWorld(FBasicWorld** ppObj);
	static TExportFuncWrapper<FBasicWorld**> WrappedCreateSimpleWorld("CreateSimpleWorld", GetModule_LostCore);

	extern "C" LOSTCORE_API void DestroySimpleWorld(FBasicWorld* obj);
	static TExportFuncWrapper<FBasicWorld*> WrappedDestroySimpleWorld("DestroySimpleWorld", GetModule_LostCore);
}