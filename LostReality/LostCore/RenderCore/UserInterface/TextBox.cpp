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

void LostCore::FTextBox::Commit()
{
	static FStackCounterRequest SCounter("FTextBox::Commit");
	FScopedStackCounterRequest req(SCounter);

	float offset = 0.0f;
	for (auto item : Children)
	{
		item->SetOriginLocal(FFloat2(offset, 0.0f));
		offset += item->GetSize().X + Space;
	}

	FRect::Commit();
}

void LostCore::FTextBox::SetText(const wstring & text)
{
	//static FStackCounterRequest SCounter("FTextBox::SetText");
	//FScopedStackCounterRequest req(SCounter);

	content = text;
	ClearChildren(true);

	array<FRectVertex, 4> vertices;
	memcpy(vertices.data(), FRectVertex::GetDefaultVertices(), 4 * sizeof(FRectVertex));

	auto font = FFontProvider::Get()->GetGdiFont();
	assert(font != nullptr);
	SetSize(FFloat2(0.0f, font->GetConfig().Height));
	for (auto it = content.begin(); it != content.end(); it++)
	{
		auto desc = font->GetCharacter(*it);
		if (desc.Texture == nullptr)
		{
			continue;
		}

		const auto& charDesc = desc.Desc;
		const auto texSize = FFloat2(desc.Texture->GetWidth(), desc.Texture->GetHeight());
		const auto charSize = FFloat2(charDesc.Width / texSize.X, charDesc.Height / texSize.Y);
		const auto topLeft = FFloat2(charDesc.X / texSize.X, charDesc.Y / texSize.Y);
		auto child = new FRect;
		child->HasGeometry(true);
		child->SetTexture(desc.Texture);
		child->SetSize(FFloat2(charDesc.Width, charDesc.Height));
		vertices[0].TexCoord = topLeft;
		vertices[1].TexCoord = topLeft + FFloat2(charSize.X, 0.0f);
		vertices[2].TexCoord = topLeft + FFloat2(0.0f, charSize.Y);
		vertices[3].TexCoord = topLeft + charSize;
		child->ConstructPrimitive(vertices.data(), 4 * sizeof(FRectVertex), sizeof(FRectVertex));
		AddChild(child);
	}
}
