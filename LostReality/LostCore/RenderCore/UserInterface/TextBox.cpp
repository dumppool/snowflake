/*
* file TextBox.cpp
*
* author luoxw
* date 2017/10/15
*
*
*/

#include "stdafx.h"
#include "TextBox.h"
#include "FontProvider.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace LostCore;

LostCore::FTextBox::FTextBox()
	: Space(1)
{
}

void LostCore::FTextBox::Update()
{
	//static FStackCounterRequest SCounter("FTextBox::Commit");
	//FScopedStackCounterRequest req(SCounter);
	FRect::Update();

	float offset = 0.0f;
	for (auto item : Children)
	{
		item->SetOriginLocal(FFloat2(offset, 0.0f));
		offset += item->GetSize().X + Space;
	}

	SetSize(FFloat2(offset, GetSize().Y));
}

void LostCore::FTextBox::SetText(const wstring & text)
{
	//static FStackCounterRequest SCounter("FTextBox::SetText");
	//FScopedStackCounterRequest req(SCounter);

	content = text;
	return;

	ClearChildren(true);

	array<FRectVertex, 4> vertices;
	memcpy(vertices.data(), FRectVertex::GetDefaultVertices(FColor128(0xffffff00)), 4 * sizeof(FRectVertex));

	auto font = FFontProvider::Get()->GetGdiFont();
	assert(font != nullptr);
	float width = 0.0f;
	for (auto it = content.begin(); it != content.end(); it++)
	{
		auto desc = font->GetCharacter(*it);
		auto ptex = desc.Texture;
		if (ptex == nullptr)
		{
			continue;
		}

		const auto& charDesc = desc.Desc;
		width += charDesc.Width;
		const auto texSize = FFloat2(ptex->GetWidth(), ptex->GetHeight());
		const auto charSize = FFloat2(charDesc.Width / texSize.X, charDesc.Height / texSize.Y);
		const auto topLeft = FFloat2(charDesc.X / texSize.X, charDesc.Y / texSize.Y);
		FRect* child = nullptr;
		child = new FRect;
		child->HasGeometry(true);
		vertices[0].TexCoord = topLeft;
		vertices[1].TexCoord = topLeft + FFloat2(charSize.X, 0.0f);
		vertices[2].TexCoord = topLeft + FFloat2(0.0f, charSize.Y);
		vertices[3].TexCoord = topLeft + charSize;

		FBuf buf;
		buf.resize(4 * sizeof(FRectVertex));
		memcpy(buf.data(), vertices.data(), buf.size());
		child->ConstructPrimitive(buf, sizeof(FRectVertex));
		child->SetTexture(ptex);
		child->SetSize(FFloat2(charDesc.Width, charDesc.Height));
		AddChild(child);
	}

	SetSize(FFloat2(width, font->GetConfig().Height));
}
