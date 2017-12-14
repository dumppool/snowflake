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
		item->SetOffsetLocal(FFloat2(offset, 0.0f));
		offset += item->GetSize().X + Space;
	}

	SetSize(FFloat2(offset, GetSize().Y));
}

void LostCore::FTextBox::SetText(const wstring & text)
{
	//static FStackCounterRequest SCounter("FTextBox::SetText");
	//FScopedStackCounterRequest req(SCounter);

	content = text;

	ClearChildren([](FRect* child) {FFontProvider::Get()->DeallocTile((FFontTile*)child); });

	float width = 0.0f;
	auto config = FFontProvider::Get()->GetConfig();
	auto texDesc = FFontProvider::Get()->GetTextureDescription();
	if (!texDesc.IsValid())
	{
		FFontProvider::Get()->GetCharacter('1');
		return;
	}

	for (auto it = content.begin(); it != content.end(); it++)
	{
		auto charDesc = FFontProvider::Get()->GetCharacter(*it);
		if (!charDesc.IsValid())
		{
			continue;
		}

		width += charDesc.Width;
		auto child = FFontProvider::Get()->AllocTile();
		child->SetCharacter(*it);
		AddChild(child);
	}

	SetSize(FFloat2(width, config.Height));
}
