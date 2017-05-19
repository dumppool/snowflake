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
#include "RenderContextInterface.h"
#include "FontProvider.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace LostCore;

LostCore::FRect::FRect()
	: Size(0.f, 0.f)
	, Depth(1.f)
	, Parent(nullptr)
	, RectPrimitive(nullptr)
	, RectPrimitiveSize(0.f, 0.f)
	, RectMaterial(nullptr)
{
}

void LostCore::FRect::SetOrigin(const FVec2 & origin)
{
	Param.Origin = origin;
}

FVec2 LostCore::FRect::GetOrigin() const
{
	if (Parent == nullptr)
	{
		return Param.Origin;
	}
	else
	{
		return Parent->GetOrigin() + Param.Origin;
	}
}

void LostCore::FRect::SetScale(float val)
{
	Param.Scale = val;
}

float LostCore::FRect::GetScale() const
{
	if (Parent == nullptr)
	{
		return Param.Scale;
	}
	else
	{
		return Parent->GetScale() * Param.Scale;
	}
}

void LostCore::FRect::SetSize(const FVec2 & size)
{
	Size = size;
}

FVec2 LostCore::FRect::GetSize() const
{
	return Size;
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
	DrawPrivate(rc, sec);
	for (auto it = Children.rbegin(); it != Children.rend(); ++it)
	{
		if (*it != nullptr)
		{
			(*it)->Draw(rc, sec);
		}
	}
}

void LostCore::FRect::SetColorTexture(ITexture * tex)
{
	if (RectMaterial != nullptr)
	{
		RectMaterial->UpdateTexture(nullptr, tex, 0);
	}
}

void LostCore::FRect::ReconstructPrimitive(IRenderContext * rc)
{
	bool bNeedReconstruct = false;
	if (Size != RectPrimitiveSize)
	{
		if (RectPrimitive != nullptr)
		{
			WrappedDestroyPrimitiveGroup(std::forward<IPrimitiveGroup*>(RectPrimitive));
			RectPrimitive = nullptr;
		}

		if (RectMaterial != nullptr)
		{
			WrappedDestroyMaterial_UIObject(std::forward<IMaterial*>(RectMaterial));
			RectMaterial = nullptr;
		}

		RectPrimitiveSize.X = 0.f;
		RectPrimitiveSize.Y = 0.f;

		if (!Size.IsZero())
		{
			bNeedReconstruct = true;
		}
	}

	if (bNeedReconstruct)
	{
		if (SSuccess != WrappedCreatePrimitiveGroup(&RectPrimitive))
		{
			return;
		}

		RectPrimitiveSize = Size;

		__declspec(align(16)) struct _Vertex { FVec4 RGBA; FVec2 XY; FVec2 Texcoord; };
		float scaler = 0.5f;

		// Mesh vertices
		const _Vertex vertices[] =
		{
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(0.f, 0.f),		FVec2(0.f, 0.f)	},		// top left
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(Size.X, 0.f),	FVec2(1.f, 0.f)	},		// top right
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(0.f, Size.Y),	FVec2(0.f, 1.f)	},		// bottom left
			{ FVec4(1.f, 1.f, 1.f, 0.7f), FVec2(Size.X, Size.Y),FVec2(1.f, 1.f) },		// bottom right
		};

		const int32 indices[] = {0, 1, 2, 1, 3, 2};

		if (!RectPrimitive->ConstructVB(rc, vertices, sizeof(vertices), sizeof(_Vertex), false) ||
			!RectPrimitive->ConstructIB(rc, indices, sizeof(indices), sizeof(int32), false))
		{
			return;
		}

		if (SSuccess != WrappedCreateMaterial_UIObject(&RectMaterial) ||
			!RectMaterial->Initialize(rc, "gui.material"))
		{
			return;
		}

		RectPrimitive->SetMaterial(RectMaterial);

		// 临时代码，查看字体贴图
		RectMaterial->UpdateTexture(rc, FFontProvider::Get()->GetGdiFont()->GetFontTextures()[0], 0);
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
	cpos = (ppos - Param.Origin);
	cpos *= Param.Scale;
}

void LostCore::FRect::DrawPrivate(IRenderContext * rc, float sec)
{
	ReconstructPrimitive(rc);

	if (RectPrimitive != nullptr && RectMaterial != nullptr)
	{
		RectMaterial->UpdateConstantBuffer(rc, (const void*)&Param, sizeof(Param));
		RectPrimitive->Draw(rc, sec);
	}

}

LostCore::FBasicGUI::FBasicGUI() : Font(nullptr)
{
}

LostCore::FBasicGUI::~FBasicGUI()
{
}

void LostCore::FBasicGUI::Tick(float sec)
{
}

void LostCore::FBasicGUI::Draw(IRenderContext * rc, float sec)
{
	Root.Draw(rc, sec);
}

bool LostCore::FBasicGUI::Load(IRenderContext * rc, const char* url)
{
	FFontProvider::Get()->Init(rc);

	// 临时代码
	Root.SetOrigin(FVec2(0.f, 0.f));
	Root.SetSize(FVec2(
		FFontProvider::Get()->GetGdiFont()->GetFontTextures()[0]->GetWidth(),
		FFontProvider::Get()->GetGdiFont()->GetFontTextures()[0]->GetHeight()));
	Root.SetScale(1.f);
	return true;
}

void LostCore::FBasicGUI::Fini()
{
	FFontProvider::Get()->Fini();
}
