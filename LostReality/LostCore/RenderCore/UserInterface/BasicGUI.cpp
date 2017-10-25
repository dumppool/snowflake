/*
* file BasicGUI.cpp
*
* author luoxw
* date 2017/04/18
*
*
*/

#include "stdafx.h"
#include "BasicGUI.h"
#include "Interface/PrimitiveGroupInterface.h"
#include "Interface/RenderContextInterface.h"
#include "FontProvider.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace LostCore;

LostCore::FRect::FRect()
	: Depth(1.f)
	, Parent(nullptr)
	, RectPrimitive(nullptr)
	, bConstructed(false)
	, RectTexture(nullptr)
	, RectBuffer(nullptr)
	, bHasGeometry(false)
	, bAutoUpdateWidth(true)
	, bAutoUpdateHeight(true)
{
}

LostCore::FRect::~FRect()
{
	Destroy();
}

FRect * LostCore::FRect::GetRoot()
{
	FRect* root = this;
	while (root->Parent != nullptr)
	{
		root = root->Parent;
	}

	return root;
}

void LostCore::FRect::SetOriginLocal(const FFloat2 & origin)
{
	Param.Origin = origin;
}

FFloat2 LostCore::FRect::GetOriginGlobal() const
{
	if (Parent == nullptr)
	{
		return Param.Origin;
	}
	else
	{
		return Parent->GetOriginGlobal() + Param.Origin;
	}
}

void LostCore::FRect::SetScaleLocal(float val)
{
	Param.Scale = val;
}

float LostCore::FRect::GetScaleGlobal() const
{
	if (Parent == nullptr)
	{
		return Param.Scale;
	}
	else
	{
		return Parent->GetScaleGlobal() * Param.Scale;
	}
}

void LostCore::FRect::SetSize(const FFloat2 & size)
{
	Param.Size = size;
}

FFloat2 LostCore::FRect::GetSize() const
{
	return Param.Size;
}

void LostCore::FRect::SetAutoUpdateWidth(bool val)
{
	bAutoUpdateWidth = val;
}

bool LostCore::FRect::GetAutoUpdateWidth() const
{
	return bAutoUpdateWidth;
}

void LostCore::FRect::SetAutoUpdateHeight(bool val)
{
	bAutoUpdateHeight = val;
}

bool LostCore::FRect::GetAutoUpdateHeight() const
{
	return bAutoUpdateHeight;
}

void LostCore::FRect::SetTexCoords(const array<FFloat2, 4>& texCoords)
{
}

bool LostCore::FRect::HitTest(const FFloat2 & ppos, FRect** result) const
{
	FFloat2 cpos;
	GetLocalPosition(ppos, cpos);
	if (HitTestPrivate(ppos, result))
	{
		for (auto child : Children)
		{
			if (child != nullptr && child->HitTest(cpos, result))
			{
				if (result != nullptr)
				{
					*result = child;
				}

				return true;
			}
		}

		if (result != nullptr)
		{
			*result = const_cast<FRect*>(this);
			return true;
		}
	}

	return false;
}

void LostCore::FRect::AddChild(FRect * child)
{
	if (child == nullptr || child->Parent != nullptr)
	{
		assert(0);
		return;
	}

	child->Parent = this;
	Children.push_back(child);
	std::sort(Children.begin(), Children.end(), [](FRect* l, FRect* r) {return l->Depth < r->Depth; });
}

void LostCore::FRect::DelChild(FRect * child)
{
	if (child == nullptr || child->Parent != this)
	{
		assert(0);
		return;
	}

	child->Parent = nullptr;
	for (auto it = Children.begin(); it != Children.end(); ++it)
	{
		if (*it == child)
		{
			Children.erase(it);
			return;
		}
	}
}

void LostCore::FRect::Detach()
{
	if (Parent != nullptr)
	{
		Parent->DelChild(this);
	}
}

void LostCore::FRect::ClearChildren(bool dealloc)
{
	if (dealloc)
	{
		for (auto item : Children)
		{
			SAFE_DELETE(item);
		}
	}

	Children.clear();
}

void LostCore::FRect::Destroy()
{
	if (RectPrimitive != nullptr)
	{
		WrappedDestroyPrimitiveGroup(forward<IPrimitiveGroup*>(RectPrimitive));
		RectPrimitive = nullptr;
		bConstructed = false;
	}

	if (RectBuffer != nullptr)
	{
		WrappedDestroyConstantBuffer(forward<IConstantBuffer*>(RectBuffer));
		RectBuffer = nullptr;
	}

	ClearChildren(true);
}

void LostCore::FRect::Update()
{
	static FStackCounterRequest SCounter("FRect::Update");
	//FScopedStackCounterRequest scopedCounter(SCounter);

	auto maxSize = GetSize();
	for (auto it = Children.rbegin(); it != Children.rend(); ++it)
	{
		(*it)->Update();
		auto size = (*it)->GetSize();
		if (bAutoUpdateWidth)
		{
			maxSize.X = Max(size.X, maxSize.X);
		}

		if (bAutoUpdateHeight)
		{
			maxSize.Y = Max(size.Y, maxSize.Y);
		}
	}

	if (bAutoUpdateWidth || bAutoUpdateHeight)
	{
		SetSize(maxSize);
	}
}

void LostCore::FRect::Commit()
{
	static FStackCounterRequest SCounter("FRect::Commit");
	//FScopedStackCounterRequest scopedCounter(SCounter);

	CommitPrivate();
	for (auto it = Children.rbegin(); it != Children.rend(); ++it)
	{
		if (*it != nullptr)
		{
			(*it)->Commit();
		}
	}
}

void LostCore::FRect::SetTexture(ITexture * tex)
{
	RectTexture = tex;
}

void LostCore::FRect::ConstructPrimitive(const void* buf, int32 sz, int32 stride)
{
	if (!bHasGeometry)
	{
		return;
	}

	bConstructed = true;
	if (SSuccess != WrappedCreatePrimitiveGroup(&RectPrimitive))
	{
		return;
	}

	RectPrimitive->SetVertexElement(FRectVertex::GetVertexElement());
	RectPrimitive->SetRenderOrder(ERenderOrder::UI);
	RectPrimitive->SetTopology(EPrimitiveTopology::TriangleList);
	if (sz == 0)
	{
		RectPrimitive->ConstructVB(FRectVertex::GetDefaultVertices(FColor128(~0)), 4 * sizeof(FRectVertex), sizeof(FRectVertex), false);
	}
	else
	{
		RectPrimitive->ConstructVB(buf, sz, stride, false);
	}

	RectPrimitive->ConstructIB(FRectVertex::GetDefaultIndices(), 6 * sizeof(int16), sizeof(int16), false);

	if (SSuccess != WrappedCreateConstantBuffer(&RectBuffer) ||
		!RectBuffer->Initialize(sizeof(Param), false))
	{
		return;
	}

	RectBuffer->SetShaderSlot(SHADER_SLOT_MATRICES);
	RectBuffer->SetShaderFlags(SHADER_FLAG_VS);
}

void LostCore::FRect::HasGeometry(bool val)
{
	bHasGeometry = val;
}

bool LostCore::FRect::HitTestPrivate(const FFloat2 & ppos, FRect** result) const
{
	FFloat2 cpos;
	GetLocalPosition(ppos, cpos);
	return (cpos.X <= Param.Size.X && 0.f <= cpos.X && cpos.Y <= Param.Size.Y && 0.f <= cpos.Y);
}

void LostCore::FRect::GetLocalPosition(const FFloat2 & ppos, FFloat2 & cpos) const
{
	cpos = (ppos - Param.Origin);
	cpos *= Param.Scale;
}

void LostCore::FRect::CommitPrivate()
{
	if (!bHasGeometry)
	{
		return;
	}

	if (!bConstructed)
	{
		ConstructPrimitive(nullptr, 0, 0);
	}

	if (RectBuffer != nullptr)
	{
		FRectParameter param(GetSize(), GetOriginGlobal(), GetScaleGlobal());
		FBufFast buf;
		param.GetBuffer(buf);
		RectBuffer->UpdateBuffer(buf);
		RectBuffer->Commit();
	}

	if (RectTexture != nullptr)
	{
		RectTexture->CommitShaderResource();
	}

	if (RectPrimitive != nullptr)
	{
		RectPrimitive->Commit();
	}
}

LostCore::FBasicGUI::FBasicGUI()
{
}

LostCore::FBasicGUI::~FBasicGUI()
{
	Destroy();
}

void LostCore::FBasicGUI::Tick()
{
	static FStackCounterRequest SCounter("FBasicGUI::Tick");
	FScopedStackCounterRequest scopedCounter(SCounter);

	Root->Update();
	Root->Commit();
}

bool LostCore::FBasicGUI::Initialize(const FFloat2& size)
{
	Root = new FRect;
	Root->SetOriginLocal(FFloat2(0.f, 0.f));
	Root->SetSize(size);
	Root->SetScaleLocal(1.f);
	return true;
}

void LostCore::FBasicGUI::Destroy()
{
	SAFE_DELETE(Root);
}
