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
#include "PrimitiveGroupInterface.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace LostCore;

LostCore::FRect::FRect()
	: Origin(0.f, 0.f)
	, Size(0.f, 0.f)
	, Scale(1.f)
	, Depth(1.f)
	, Parent(nullptr)
	, RectPrimitive(nullptr)
	, RectPrimitiveSize(0.f, 0.f)
	, RectMaterial(nullptr)
{
}

FVec2 LostCore::FRect::GetOrigin() const
{
	if (Parent == nullptr)
	{
		return Origin;
	}
	else
	{
		Parent->GetOrigin() + Origin;
	}
}

float LostCore::FRect::GetScale() const
{
	if (Parent == nullptr)
	{
		return Scale;
	}
	else
	{
		Parent->GetScale() * Scale;
	}
}

bool LostCore::FRect::HitTest(const FVec2 & ppos, FRect** result) const
{
	FVec2 cpos;
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
		return;
	}

	Children.push_back(child);
	std::sort(Children.begin(), Children.end(), [](FRect* l, FRect* r) {return l->Depth < r->Depth; });
}

void LostCore::FRect::DelChild(FRect * child)
{
	if (child == nullptr || child->Parent != this)
	{
		return;
	}

	for (auto it = Children.begin(); it != Children.end(); ++it)
	{
		if (*it == child)
		{
			Children.erase(it);
			return;
		}
	}
}

void LostCore::FRect::Clear()
{
}

void LostCore::FRect::Draw(IRenderContext * rc, float sec)
{
	if (RectPrimitive != nullptr)
	{
		RectPrimitive->Draw(rc, sec);
	}
}

void LostCore::FRect::ReconstructPrimitive(IRenderContext * rc)
{
	if (RectPrimitive != nullptr && Size != RectPrimitiveSize)
	{
		WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(RectPrimitive));
	}

	if (RectPrimitive == nullptr && !Size.IsZero())
	{
		if (SSuccess != WrappedCreatePrimitiveGroup(&RectPrimitive))
		{
			return;
		}

		RectPrimitiveSize = Size;

		__declspec(align(16)) struct _Vertex { FVec4 RGBA; FVec2 XY; };
		float scaler = 0.5f;

		// Mesh vertices
		const _Vertex vertices[] =
		{
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(0.f, 0.f)		},		// top left
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(Size.X, 0.f)	},		// top right
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(0.f, Size.Y)	},		// bottom left
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(Size.X, Size.Y) },		// bottom right
		};

		if (!RectPrimitive->ConstructVB(rc, vertices, sizeof(vertices), sizeof(_Vertex), false))
		{
			return;
		}

		if (SSuccess != WrappedCreateMaterial(&RectMaterial) ||
			!RectMaterial->Initialize(rc, "gui.material"))
		{
			return;
		}

		RectPrimitive->SetMaterial(RectMaterial);
	}

	return;
}

bool LostCore::FRect::HitTestPrivate(const FVec2 & ppos, FRect** result) const
{
	FVec2 cpos;
	GetLocalPosition(ppos, cpos);
	return (cpos.X <= Size.X && 0.f <= cpos.X && cpos.Y <= Size.Y && 0.f <= cpos.Y);
}

void LostCore::FRect::GetLocalPosition(const FVec2 & ppos, FVec2 & cpos) const
{
	cpos = (ppos - Origin);
	cpos *= Scale;
}
