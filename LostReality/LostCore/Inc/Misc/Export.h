/*
* file Export.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

typedef int32 EReturnCode;
static const EReturnCode SSuccess = 0x0;
static const EReturnCode SErrorModuleNotFound = 0x0bad0001;
static const EReturnCode SErrorFunctionNotFound = 0x0bad0002;
static const EReturnCode SErrorInvalidParameters = 0x0bad0003;
static const EReturnCode SErrorOutOfMemory = 0x0bad0004;
static const EReturnCode SErrorInternalError = 0x0bad0005;

template <typename... Args>
class TExportFuncWrapper
{
	typedef EReturnCode(*Signature)(Args...);
	typedef Signature FuncType;
	FuncType FuncObject;
	char FuncName[64];

	std::function<HMODULE(void)> ModuleGetter;

public:

	TExportFuncWrapper(const char* name, std::function<HMODULE(void)> modGetter)
		: FuncObject(nullptr), ModuleGetter(modGetter)
	{
		strcpy_s(FuncName, name);
	}

	~TExportFuncWrapper()
	{
	}

	EReturnCode operator()(Args&&... args)
	{
		if (ModuleGetter == nullptr)
		{
			return SErrorModuleNotFound;
		}

		if (FuncObject == nullptr)
		{
			auto mod = ModuleGetter();
			if (mod == NULL)
			{
				return SErrorModuleNotFound;
			}

			FuncObject = (FuncType)GetProcAddress(mod, FuncName);
		}

		if (FuncObject != nullptr)
		{
			return FuncObject(args...);
		}
		else
		{
			return SErrorFunctionNotFound;
		}
	}
};

// 导出辅助宏，方便导入工程使用
#define EXPORT_WRAP_0_DCL(name)\
EXPORT_API EReturnCode name();\
static TExportFuncWrapper<> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_0_DEF(name)\
EReturnCode name()\
{\
return EXPORTER_PTR->##name##();\
}

#define EXPORT_WRAP_1_DCL(name, T0)\
EXPORT_API EReturnCode name(T0);\
static TExportFuncWrapper<T0> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_1_DEF(name, T0)\
EReturnCode name(T0 _0)\
{\
return EXPORTER_PTR->##name##(_0);\
}

#define EXPORT_WRAP_2_DCL(name, T0, T1)\
EXPORT_API EReturnCode name(T0, T1);\
static TExportFuncWrapper<T0, T1> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_2_DEF(name, T0, T1)\
EReturnCode name(T0 _0, T1 _1)\
{\
return EXPORTER_PTR->##name##(_0, _1);\
}

#define EXPORT_WRAP_3_DCL(name, T0, T1, T2)\
EXPORT_API EReturnCode name(T0, T1, T2);\
static TExportFuncWrapper<T0, T1, T2> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_3_DEF(name, T0, T1, T2)\
EReturnCode name(T0 _0, T1 _1, T2 _2)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2);\
}

#define EXPORT_WRAP_4_DCL(name, T0, T1, T2, T3)\
EXPORT_API EReturnCode name(T0, T1, T2, T3);\
static TExportFuncWrapper<T0, T1, T2, T3> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_4_DEF(name, T0, T1, T2, T3)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3);\
}

#define EXPORT_WRAP_5_DCL(name, T0, T1, T2, T3, T4)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4);\
static TExportFuncWrapper<T0, T1, T2, T3, T4> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_5_DEF(name, T0, T1, T2, T3, T4)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4);\
}

#define EXPORT_WRAP_6_DCL(name, T0, T1, T2, T3, T4, T5)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4, T5);\
static TExportFuncWrapper<T0, T1, T2, T3, T4, T5> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_6_DEF(name, T0, T1, T2, T3, T4, T5)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5, _5)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4, _5);\
}

#define EXPORT_WRAP_7_DCL(name, T0, T1, T2, T3, T4, T5, T6)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4, T5, T6);\
static TExportFuncWrapper<T0, T1, T2, T3, T4, T5, T6> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_7_DEF(name, T0, T1, T2, T3, T4, T5, T6)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5, _5, T6 _6)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4, _5, _6);\
}

#define EXPORT_WRAP_8_DCL(name, T0, T1, T2, T3, T4, T5, T6, T7)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4, T5, T6, T7);\
static TExportFuncWrapper<T0, T1, T2, T3, T4, T5, T6, T7> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_8_DEF(name, T0, T1, T2, T3, T4, T5, T6, T7)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5, _5, T6 _6, T7 _7)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4, _5, _6, _7);\
}

#define EXPORT_WRAP_9_DCL(name, T0, T1, T2, T3, T4, T5, T6, T7, T8)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4, T5, T6, T7, T8);\
static TExportFuncWrapper<T0, T1, T2, T3, T4, T5, T6, T7, T8> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_9_DEF(name, T0, T1, T2, T3, T4, T5, T6, T7, T8)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5, _5, T6 _6, T7 _7, T8 _8)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4, _5, _6, _7, _8);\
}

#define EXPORT_WRAP_10_DCL(name, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9);\
static TExportFuncWrapper<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_10_DEF(name, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5, _5, T6 _6, T7 _7, T8 _8, T9 _9)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9);\
}

// For LoadFBX...
#define EXPORT_WRAP_14_DCL(name, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)\
EXPORT_API EReturnCode name(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13);\
static TExportFuncWrapper<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13> Wrapped##name(#name, GET_MODULE);

#define EXPORT_WRAP_14_DEF(name, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13)\
EReturnCode name(T0 _0, T1 _1, T2 _2, T3 _3, T4 _4, T5 _5, T6 _6, T7 _7, T8 _8, T9 _9, T10 _10, T11 _11, T12 _12, T13 _13)\
{\
return EXPORTER_PTR->##name##(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13);\
}
//#endif //#if defined(GET_MODULE) && defined(EXPORT_API) && defined(return EXPORTER_PTR->)

template <typename T>
std::function<T> GetFunction(const char* name)
{
	FARPROC func = GetProcAddress(NULL, name);
	return std::function<T>((T*)func);
}

template <typename T, typename... Args>
typename std::result_of<std::function<T>(Args...)>::type ExeFunction(const char* name, Args&&... args)
{
	return GetFunction<T>(name)(args...);
}

FORCEINLINE void foo()
{
	int a = 111;
	int* b = &a;
	auto f = ExeFunction<int(int*)>("haha", b);
}