/*
* file UtilitiesHeader.h
*
* author luoxw
* date 2017/01/18
*
*
*/

#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

#include <assert.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <time.h>
#include <fstream>
#include <memory>
#include <functional>

using namespace std;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

#include <DirectXMath.h>
#include <DirectXColors.h>

typedef DirectX::XMFLOAT2 LVVec2;
typedef DirectX::XMFLOAT3 LVVec3;
typedef DirectX::XMFLOAT4 LVVec;
typedef DirectX::XMVECTOR LVQuat;
typedef DirectX::XMMATRIX LVMatrix;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
  do                    \
  {                     \
    if(p)               \
    {                   \
      (p)->Release();   \
      (p) = NULL;       \
    }                   \
  } while((void)0, 0)
#define SAFE_DELETE(p)  \
  do                    \
  {                     \
    if(p)               \
    {                   \
      delete (p);		\
      (p) = nullptr;    \
    }                   \
  } while((void)0, 0)
#define SAFE_DELETE_ARRAY(p)  \
  do                          \
  {                           \
    if(p)                     \
    {                         \
      delete[] (p);		      \
      (p) = nullptr;          \
    }                         \
  } while((void)0, 0)
#endif //SAFE_RELEASE

#define INLINE __forceinline

inline static void log_cap_cnt(const CHAR* Prefix, const CHAR* cap, const CHAR* fmt, ...)
{
	char msg[1024];

	va_list args;
	va_start(args, fmt);
	vsnprintf(msg, 1023, fmt, args);
	//msg[sz] = '\0';
	va_end(args);

	time_t t = ::time(0);
	tm curr;
	::localtime_s(&curr, &t);
	char cdate[1024];
	snprintf(cdate, 1023, "%d/%d/%d %d:%d:%d", 1900 + curr.tm_year, curr.tm_mon + 1, curr.tm_mday, curr.tm_hour, curr.tm_min, curr.tm_sec);

	char* envdir = nullptr;
	size_t sz = 0;
	if (!_dupenv_s(&envdir, &sz, "APPDATA") == 0 || envdir == nullptr)
	{
		::MessageBoxA(0, "", "failed to get environment path", 0);
		return;
	}

	char logpath[1024];
	snprintf(logpath, 1023, "%s\\_LostVR", envdir);
	free(envdir);
	envdir = nullptr;

	DWORD pid = ::GetCurrentProcessId();
	char filepath[1024];
	snprintf(filepath, 1023, "%s\\%s-%d.log", logpath, Prefix, pid);

	ofstream logfile(filepath, ios::app | ios::out);
	if (!logfile)
	{
		SECURITY_ATTRIBUTES secu;
		secu.bInheritHandle = TRUE;
		secu.lpSecurityDescriptor = nullptr;
		secu.nLength = sizeof(SECURITY_ATTRIBUTES);
		if (::CreateDirectoryA(logpath, &secu) == FALSE)
		{
			::MessageBoxA(0, logpath, "failed to create directory", 0);
			return;
		}
	}

	logfile << cdate << "\t";
	logfile << "PID: " << pid << "\t";
	logfile << cap << ": " << string(msg).c_str() << endl;
	logfile.flush();
}


#define LVMSG_PREFIX(Prefix, Cap, ...) {\
log_cap_cnt(Prefix, Cap, __VA_ARGS__);}

//static const CHAR* SLogPrefixDefault = "Default";
//static const CHAR* SLogPrefixError = "ErrorReport";
//#define LVMSG(Head, ...)	LVMSG_PREFIX(SLogPrefixDefault, Head, __VA_ARGS__)
//#define LVERROR(Head, ...)	LVMSG_PREFIX(SLogPrefixError, Head, __VA_ARGS__)

#ifdef MODULE_MSG_PREFIX
#define LVMSG(head, ...) LVMSG_PREFIX(MODULE_MSG_PREFIX, head, __VA_ARGS__)
#else
#error "MODULE_MSG_PREFIX not defined"
#endif

#ifdef MODULE_ERR_PREFIX
#define LVERR(head, ...) LVMSG_PREFIX(MODULE_ERR_PREFIX, head, __VA_ARGS__)
#else
#error "MODULE_ERR_PREFIX not defined"
#endif

#ifdef MODULE_WARN_PREFIX
#define LVWARN(head, ...) LVMSG_PREFIX(MODULE_WARN_PREFIX, head, __VA_ARGS__)
#else
#error "MODULE_WARN_PREFIX not defined"
#endif

#define LVASSERT_PREFIX(Prefix, Condition, Cap, ...) {\
	if (!(Condition))\
	{\
		LVMSG_PREFIX(Prefix, Cap, __VA_ARGS__);\
		LVMSG_PREFIX(Prefix, "assert failed", "file: %s, line: %d.", __FILE__, __LINE__);\
		assert(0);\
	}\
}

#define LVASSERT(Condition, Cap, ...) LVASSERT_PREFIX(SLogPrefixError, Condition, Cap, __VA_ARGS__)

//inline bool GetWndFromProcessID(DWORD pid, HWND hwnd[], UINT& nwnd)
//{
//	HWND temp = NULL;
//	nwnd = 0;
//	while ((temp = FindWindowEx(NULL, temp, NULL, NULL)) && sizeof(hwnd) > nwnd)
//	{
//		DWORD p;
//		GetWindowThreadProcessId(temp, &p);
//		if (pid == p)
//		{
//			hwnd[nwnd++] = temp;
//			//break;
//		}
//	}
//
//	HWND root = hwnd[nwnd - 1];
//	temp = root;
//	//nwnd = 1;
//
//	while ((temp = GetParent(temp)) != NULL)
//	{
//		DWORD p;
//		GetWindowThreadProcessId(temp, &p);
//		if (pid == p)
//		{
//			root = temp;
//		}
//		else
//		{
//			break;
//		}
//	}
//
//	LVMSG("GetWndFromProcessID", "last wnd: %x, root: %x", hwnd[nwnd - 1], root);
//	hwnd[0] = root;
//	nwnd = 1;
//	return true;
//}

inline wstring UTF8ToWide(const string &utf8)
{
	if (utf8.length() == 0) {
		return wstring();
	}

	// compute the length of the buffer we'll need
	int charcount = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);

	if (charcount == 0) {
		return wstring();
	}

	// convert
	wchar_t* buf = new wchar_t[charcount];
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, buf, charcount);
	wstring result(buf);
	delete[] buf;
	return result;
}

// static
inline string WideToUTF8(const wstring &wide) {
	if (wide.length() == 0) {
		return string();
	}

	// compute the length of the buffer we'll need
	int charcount = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1,
		NULL, 0, NULL, NULL);
	if (charcount == 0) {
		return string();
	}

	// convert
	char *buf = new char[charcount];
	WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, buf, charcount,
		NULL, NULL);

	string result(buf);
	delete[] buf;
	return result;
}

/**
* A smart pointer to an object which implements AddRef/Release.
* from UnrealEngine
*/
template<typename ReferencedType>
class TRefCountPtr
{
	typedef ReferencedType* ReferenceType;

public:

	FORCEINLINE TRefCountPtr() :
		Reference(nullptr)
	{ }

	TRefCountPtr(ReferencedType* InReference, bool bAddRef = true)
	{
		Reference = InReference;
		if (Reference && bAddRef)
		{
			Reference->AddRef();
		}
	}

	TRefCountPtr(const TRefCountPtr& Copy)
	{
		Reference = Copy.Reference;
		if (Reference)
		{
			Reference->AddRef();
		}
	}

	FORCEINLINE TRefCountPtr(TRefCountPtr&& Copy)
	{
		Reference = Copy.Reference;
		Copy.Reference = nullptr;
	}

	~TRefCountPtr()
	{
		if (Reference)
		{
			Reference->Release();
		}
	}

	TRefCountPtr& operator=(ReferencedType* InReference)
	{
		// Call AddRef before Release, in case the new reference is the same as the old reference.
		ReferencedType* OldReference = Reference;
		Reference = InReference;
		if (Reference)
		{
			Reference->AddRef();
		}
		if (OldReference)
		{
			OldReference->Release();
		}
		return *this;
	}

	FORCEINLINE TRefCountPtr& operator=(const TRefCountPtr& InPtr)
	{
		return *this = InPtr.Reference;
	}

	TRefCountPtr& operator=(TRefCountPtr&& InPtr)
	{
		if (this != &InPtr)
		{
			ReferencedType* OldReference = Reference;
			Reference = InPtr.Reference;
			InPtr.Reference = nullptr;
			if (OldReference)
			{
				OldReference->Release();
			}
		}
		return *this;
	}

	FORCEINLINE ReferencedType* operator->() const
	{
		return Reference;
	}

	FORCEINLINE operator ReferenceType() const
	{
		return Reference;
	}

	FORCEINLINE ReferencedType** GetInitReference()
	{
		*this = nullptr;
		return &Reference;
	}

	FORCEINLINE ReferencedType* GetReference() const
	{
		return Reference;
	}

	FORCEINLINE friend bool IsValidRef(const TRefCountPtr& InReference)
	{
		return InReference.Reference != nullptr;
	}

	FORCEINLINE bool IsValid() const
	{
		return Reference != nullptr;
	}

	FORCEINLINE void SafeRelease()
	{
		*this = nullptr;
	}

	uint32 GetRefCount()
	{
		uint32 Result = 0;
		if (Reference)
		{
			Result = Reference->GetRefCount();
			check(Result > 0); // you should never have a zero ref count if there is a live ref counted pointer (*this is live)
		}
		return Result;
	}

	FORCEINLINE void Swap(TRefCountPtr& InPtr) // this does not change the reference count, and so is faster
	{
		ReferencedType* OldReference = Reference;
		Reference = InPtr.Reference;
		InPtr.Reference = OldReference;
	}

	//friend FArchive& operator<<(FArchive& Ar, TRefCountPtr& Ptr)
	//{
	//	ReferenceType PtrReference = Ptr.Reference;
	//	Ar << PtrReference;
	//	if (Ar.IsLoading())
	//	{
	//		Ptr = PtrReference;
	//	}
	//	return Ar;
	//}

private:

	ReferencedType* Reference;
};

template<typename ReferencedType>
FORCEINLINE bool operator==(const TRefCountPtr<ReferencedType>& A, const TRefCountPtr<ReferencedType>& B)
{
	return A.GetReference() == B.GetReference();
}

template<typename ReferencedType>
FORCEINLINE bool operator==(const TRefCountPtr<ReferencedType>& A, ReferencedType* B)
{
	return A.GetReference() == B;
}

template<typename ReferencedType>
FORCEINLINE bool operator==(ReferencedType* A, const TRefCountPtr<ReferencedType>& B)
{
	return A == B.GetReference();
}

typedef int32 EReturnCode;
static const EReturnCode SSuccess = 0x0;
static const EReturnCode SErrorModuleNotFound		= 0x0bad0001;
static const EReturnCode SErrorFunctionNotFound		= 0x0bad0002;
static const EReturnCode SErrorInvalidParameters	= 0x0bad0003;
static const EReturnCode SErrorOutOfMemory			= 0x0bad0004;
static const EReturnCode SErrorInternalError		= 0x0bad0005;

template <typename... Args>
class TExportFuncWrapper
{
	typedef EReturnCode (*Signature)(Args...);
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

inline void foo()
{
	int a = 111;
	int* b = &a;
	auto f = ExeFunction<int(int*)>("haha", b);
}

class FHighFrequencyCounter
{
	LARGE_INTEGER TicksPerSec;
	LARGE_INTEGER Timestamp;

public:
	INLINE FHighFrequencyCounter();

	INLINE void Start();

	//************************************
	// Method:    Stop
	// FullName:  FHighFrequencyCounter::Stop
	// Access:    public 
	// Returns:   double in ms
	// Qualifier:
	//************************************
	INLINE double Stop();

	INLINE double Count();

	INLINE bool Started() const;
};

class FScopedHighFrequencyCounter
{
	const CHAR* MsgHead;
	FHighFrequencyCounter Counter;

public:
	INLINE FScopedHighFrequencyCounter(const CHAR* head);
	INLINE ~FScopedHighFrequencyCounter();
};

INLINE FScopedHighFrequencyCounter::FScopedHighFrequencyCounter(const CHAR * head) : MsgHead(head), Counter()
{
	Counter.Start();
}

INLINE FScopedHighFrequencyCounter::~FScopedHighFrequencyCounter()
{
	double duration = Counter.Stop();
	{
#if 0
		LVMSG("FScopedHighFrequencyCounter", "%s\t\t%fms", MsgHead ? MsgHead : "<null>", duration);
#endif
	}
}

INLINE FHighFrequencyCounter::FHighFrequencyCounter()
{
	Timestamp.QuadPart = 0;
	QueryPerformanceFrequency(&TicksPerSec);
}

INLINE void FHighFrequencyCounter::Start()
{
	QueryPerformanceCounter(&Timestamp);
}

INLINE double FHighFrequencyCounter::Stop()
{
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	return (double)((end.QuadPart - Timestamp.QuadPart) * 1000.0 / TicksPerSec.QuadPart);
}

INLINE double FHighFrequencyCounter::Count()
{
	LARGE_INTEGER begin, end;
	begin = Timestamp;
	QueryPerformanceCounter(&Timestamp);
	QueryPerformanceCounter(&end);
	return (double)((end.QuadPart - begin.QuadPart) * 1000.0 / TicksPerSec.QuadPart);
}

INLINE bool FHighFrequencyCounter::Started() const
{
	return Timestamp.QuadPart != 0;
}