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
	: Depth(1.f)
	, Parent(nullptr)
	, RectPrimitive(nullptr)
	, RectPrimitiveSize(0.f, 0.f)
	, RectMaterial(nullptr)
	, RectBuffer(nullptr)
{
}

LostCore::FRect::~FRect()
{
	Clear();
}

void LostCore::FRect::SetOrigin(const FFloat2 & origin)
{
	Param.Origin = origin;
}

FFloat2 LostCore::FRect::GetOrigin() const
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

void LostCore::FRect::SetSize(const FFloat2 & size)
{
	Param.Size = size;
}

FFloat2 LostCore::FRect::GetSize() const
{
	return Param.Size;
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
	if (RectPrimitive != nullptr)
	{
		WrappedDestroyPrimitiveGroup(forward<IPrimitiveGroup*>(RectPrimitive));
		RectPrimitive = nullptr;
	}

	if (RectMaterial != nullptr)
	{
		WrappedDestroyMaterial(forward<IMaterial*>(RectMaterial));
		RectMaterial = nullptr;
	}

	if (RectBuffer != nullptr)
	{
		WrappedDestroyConstantBuffer(forward<IConstantBuffer*>(RectBuffer));
		RectBuffer = nullptr;
	}

	for (auto& c : Children)
	{
		SAFE_DELETE(c);
	}

	Children.clear();
}

void LostCore::FRect::Draw()
{
	DrawPrivate();
	for (auto it = Children.rbegin(); it != Children.rend(); ++it)
	{
		if (*it != nullptr)
		{
			(*it)->Draw();
		}
	}
}

void LostCore::FRect::SetColorTexture(ITexture * tex)
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	if (RectMaterial != nullptr)
	{
		RectMaterial->UpdateTexture(nullptr, tex, 0);
	}
}

void LostCore::FRect::ReconstructPrimitive()
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	if (rc == nullptr)
	{
		return;
	}

	if (RectPrimitiveSize.IsZero())
	{
		RectPrimitiveSize.X = 1.f;
		RectPrimitiveSize.Y = 1.f;
		if (SSuccess != WrappedCreatePrimitiveGroup(&RectPrimitive))
		{
			return;
		}

		__declspec(align(16)) struct _Vertex { FFloat2 XY; FFloat2 Texcoord; FFloat4 RGBA; };

		// Mesh vertices
		const _Vertex vertices[] =
		{
			//{ FFloat2(0.f, 0.f), FFloat2(0.f, 0.f),	FFloat4(1.f, 1.f, 1.f, 0.7f) },		// top left
			//{ FFloat2(Size.X, 0.f), FFloat2(1.f, 0.f),	FFloat4(1.f, 1.f, 1.f, 0.7f) },		// top right
			//{ FFloat2(0.f, Size.Y), FFloat2(0.f, 1.f),	FFloat4(1.f, 1.f, 1.f, 0.7f) },		// bottom left
			//{ FFloat2(Size.X, Size.Y), FFloat2(1.f, 1.f),	FFloat4(1.f, 1.f, 1.f, 0.7f) },		// bottom right
			{ FFloat2(0.f, 0.f),	FFloat2(0.f, 0.f), FFloat4(1.f, 1.f, 1.f, 0.7f) },		// top left
			{ FFloat2(1.f, 0.f),	FFloat2(1.f, 0.f), FFloat4(1.f, 1.f, 1.f, 0.7f) },		// top right
			{ FFloat2(0.f, 1.f),	FFloat2(0.f, 1.f), FFloat4(1.f, 1.f, 1.f, 0.7f) },		// bottom left
			{ FFloat2(1.f, 1.f),	FFloat2(1.f, 1.f), FFloat4(1.f, 1.f, 1.f, 0.7f) },		// bottom right
		};

		const int32 indices[] = {0, 1, 2, 1, 3, 2};

		if (!RectPrimitive->ConstructVB(rc, vertices, sizeof(vertices), sizeof(_Vertex), false) ||
			!RectPrimitive->ConstructIB(rc, indices, sizeof(indices), sizeof(int32), false))
		{
			return;
		}

		if (SSuccess != WrappedCreateMaterial(&RectMaterial) ||
			!RectMaterial->Initialize(rc, "gui.json"))
		{
			return;
		}

		if (SSuccess != WrappedCreateConstantBuffer(&RectBuffer) ||
			!RectBuffer->Initialize(rc, sizeof(Param), false))
		{
			return;
		}

		// 临时代码，查看字体贴图
		RectMaterial->UpdateTexture(rc, FFontProvider::Get()->GetGdiFont()->GetFontTextures()[0], 0);
	}

	return;
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

void LostCore::FRect::DrawPrivate()
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();
	if (rc == nullptr)
	{
		return;
	}

	ReconstructPrimitive();

	if (RectBuffer != nullptr)
	{
		RectBuffer->UpdateBuffer(rc, (const void*)&Param.GetBuffer(), sizeof(Param));
		RectBuffer->Bind(rc, 1 | SHADER_SLOT_VS);
	}

	if (RectMaterial != nullptr)
	{
		RectMaterial->Bind(rc);
	}

	if (RectPrimitive != nullptr && RectMaterial != nullptr)
	{
		RectPrimitive->Draw(rc, sec);
	}

}

LostCore::FBasicGUI::FBasicGUI() : Font(nullptr)
{
}

LostCore::FBasicGUI::~FBasicGUI()
{
	Fini();
}

void LostCore::FBasicGUI::Tick()
{
}

void LostCore::FBasicGUI::Draw()
{
	//Root.Draw(sec);
}

bool LostCore::FBasicGUI::Config(const FJson & config)
{
	return false;
}

bool LostCore::FBasicGUI::Load(const char* url)
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();

	FFontProvider::Get()->Init();

	// 临时代码
	Root.SetOrigin(FFloat2(0.f, 0.f));
	Root.SetSize(FFloat2(
		(float)FFontProvider::Get()->GetGdiFont()->GetFontTextures()[0]->GetWidth(),
		(float)FFontProvider::Get()->GetGdiFont()->GetFontTextures()[0]->GetHeight()));
	Root.SetScale(1.f);
	return true;
}

void LostCore::FBasicGUI::Fini()
{
	FFontProvider::Get()->Fini();
	Root.Clear();
}
