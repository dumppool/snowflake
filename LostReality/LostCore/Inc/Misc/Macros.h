/*
* file Macros.h
*
* author luoxw
* date 2017/09/15
*
*
*/

#pragma once

#ifdef LOSTCORE_EXPORTS
#define LOSTCORE_API extern "C" __declspec(dllexport)
#else
#define LOSTCORE_API extern "C" __declspec(dllimport)
#endif

#define ALIGNED_LR(x) __declspec(align(x))

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

#define HAS_FLAGS(flags, srcFlags) ((flags&srcFlags)==flags)

#define CH(s) u8##s

#define ENABLE_MEMORY_COUNTER 1
