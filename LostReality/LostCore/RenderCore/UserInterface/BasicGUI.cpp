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

void LostCore::FRect::SetOffsetLocal(const FFloat2 & origin)
{
	Param.Offset = origin;
}

FFloat2 LostCore::FRect::GetOffsetGlobal() const
{
	if (Parent == nullptr)
	{
		return Param.Offset;
	}
	else
	{
		return Parent->GetOffsetGlobal() + Param.Offset;
	}
}

void LostCore::FRect::SetScaleLocal(const FFloat2& val)
{
	Param.Scale = val;
}

LostCore::FFloat2 LostCore::FRect::GetScaleGlobal() const
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
	if (child == nullptr || child->Parent != nullptr || child == this)
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
		assert("null child or invalid parent" && 0);
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

void LostCore::FRect::PopChild(const function<void(FRect*)>& dealloc)
{
	if (Children.empty())
	{
		return;
	}

	if (dealloc != nullptr)
	{
		auto child = Children.back();
		child->Parent = nullptr;
		dealloc(child);
	}

	Children.pop_back();
}

void LostCore::FRect::Detach()
{
	if (Parent != nullptr)
	{
		Parent->DelChild(this);
	}
}

void LostCore::FRect::ClearChildren(const function<void(FRect*)>& dealloc)
{
	if (dealloc != nullptr)
	{
		for (auto item : Children)
		{
			dealloc(item);
		}
	}

	Children.clear();
}

int32 LostCore::FRect::NumChildren() const
{
	return Children.size();
}

FRect* LostCore::FRect::GetChild(int32 index)
{
	if (index < Children.size())
	{
		return Children[index];
	}
	else
	{
		return nullptr;
	}
}

void LostCore::FRect::Destroy()
{
	ClearChildren([](FRect* child) {SAFE_DELETE(child); });

	if (RectPrimitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(forward<IPrimitive*>(RectPrimitive));
		RectPrimitive = nullptr;
	}
	
	if (RectBuffer != nullptr)
	{
		D3D11::WrappedDestroyConstantBuffer(forward<IConstantBuffer*>(RectBuffer));
		RectBuffer = nullptr;
	}

	bConstructed = false;
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

//void LostCore::FRect::SetTexture(ITextureSet* tex)
//{
//	RectTexture = tex;
//}

void LostCore::FRect::ConstructPrimitive(const FBuf& buf, int32 stride)
{
	if (RectPrimitive != nullptr)
	{
		return;
	}

	WrappedCreatePrimitiveGroup(&RectPrimitive);
	RectPrimitive->SetVertexElement(FRectVertex::GetVertexElement());
	RectPrimitive->SetRenderOrder(ERenderOrder::UI);
	RectPrimitive->SetTopology(EPrimitiveTopology::TriangleList);
	if (buf.empty())
	{
		RectPrimitive->ConstructVB(FRectVertex::GetDefaultVertices(FColor128(~0)),
			FRectVertex::GetDefaultSize(), sizeof(FRectVertex), false);
	}
	else
	{
		RectPrimitive->ConstructVB(buf.data(), buf.size(), stride, false);
	}

	RectPrimitive->ConstructIB(*FRectVertex::GetDefaultIndices(), sizeof(int16), false);
}

void LostCore::FRect::HasGeometry(bool val)
{
	bHasGeometry = val;
}

void LostCore::FRect::UpdateAndCommitRectBuffer()
{
	if (!bHasGeometry)
	{
		return;
	}

	if (RectBuffer == nullptr)
	{
		WrappedCreateConstantBuffer(&RectBuffer);
		RectBuffer->SetShaderSlot(SHADER_SLOT_MATRICES);
		RectBuffer->SetShaderFlags(SHADER_FLAG_VS);
	}

	FRectParameter param(GetOffsetGlobal(), GetSize(), GetScaleGlobal());
	FBuf buf;
	param.GetBuffer(buf);
	RectBuffer->UpdateBuffer(buf);
	RectBuffer->Commit();
}

bool LostCore::FRect::HitTestPrivate(const FFloat2 & ppos, FRect** result) const
{
	FFloat2 cpos;
	GetLocalPosition(ppos, cpos);
	return (cpos.X <= Param.Size.X && 0.f <= cpos.X && cpos.Y <= Param.Size.Y && 0.f <= cpos.Y);
}

void LostCore::FRect::GetLocalPosition(const FFloat2 & ppos, FFloat2 & cpos) const
{
	cpos = (ppos - Param.Offset);
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
		ConstructPrimitive(FBuf(), 0);
		bConstructed = true;
	}

	UpdateAndCommitRectBuffer();

	if (RectPrimitive != nullptr)
	{
		RectPrimitive->Commit();
	}
}

FGUI* FGUI::SInstance = nullptr;
void LostCore::FGUI::StaticInitialize()
{
	assert(FGUI::SInstance == nullptr);
	FGUI::SInstance = new FGUI;
}

void LostCore::FGUI::StaticDestroy()
{
	assert(FGUI::SInstance != nullptr);
	SAFE_DELETE(FGUI::SInstance);
}

LostCore::FGUI* LostCore::FGUI::Get()
{
	return FGUI::SInstance;
}

LostCore::FGUI::FGUI()
{
}

LostCore::FGUI::~FGUI()
{
	Destroy();
}

void LostCore::FGUI::Tick()
{
	static FStackCounterRequest SCounter("FBasicGUI::Tick");
	FScopedStackCounterRequest scopedCounter(SCounter);

	{
		static FStackCounterRequest SUpdateCounter("Update");
		FScopedStackCounterRequest scopedUpdateCounter(SUpdateCounter);
		Root->Update();
	}

	{
		static FStackCounterRequest SCommitCounter("Commit");
		FScopedStackCounterRequest scopedCommitCounter(SCommitCounter);
		Root->Commit();
	}
}

bool LostCore::FGUI::Initialize(const FFloat2& size)
{
	Root = new FRect;
	Root->SetOffsetLocal(FFloat2(0.f, 0.f));
	Root->SetSize(size);
	Root->SetScaleLocal(FFloat2(1.f, 1.f));
	return true;
}

void LostCore::FGUI::Destroy()
{
	SAFE_DELETE(Root);
}
