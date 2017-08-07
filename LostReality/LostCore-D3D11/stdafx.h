// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

// utility includes
#define MODULE_MSG_PREFIX "lostcore-d3d11-log"
#define MODULE_WARN_PREFIX "lostcore-d3d11-warn"
#define MODULE_ERR_PREFIX "lostcore-d3d11-error"
#include "UtilitiesHeader.h"

template<>
class TRefCountPtr<ID3D11PixelShader>
{
	typedef ID3D11PixelShader ReferencedType;
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

	//uint32 GetRefCount()
	//{
	//	uint32 Result = 0;
	//	if (Reference)
	//	{
	//		Result = Reference->GetRefCount();
	//		check(Result > 0); // you should never have a zero ref count if there is a live ref counted pointer (*this is live)
	//	}
	//	return Result;
	//}

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

#include "Keywords.h"

// file system includes
#include "File/json.hpp"
using FJson = nlohmann::json;

// math includes
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Color.h"

#include "File/DirectoryHelper.h"

// vertex detail includes
#include "VertexTypes.h"

// interface includes
#include "RenderContextInterface.h"
#include "TextureInterface.h"
#include "MaterialInterface.h"
#include "PrimitiveGroupInterface.h"
#include "FontInterface.h"

// local includes
#include "Src/RenderContextBase.h"
#include "Src/RenderContext.h"
