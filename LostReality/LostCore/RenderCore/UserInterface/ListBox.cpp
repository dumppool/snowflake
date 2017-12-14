/*
* file ListBox.cpp
*
* author luoxw
* date 2017/10/15
*
*
*/

#include "stdafx.h"
#include "ListBox.h"

using namespace LostCore;

LostCore::FListBox::FListBox()
	: FRect()
	, Alignment(EAlignment::Vertical)
	, Space(0)
{
}

void LostCore::FListBox::Update()
{
	FRect::Update();
	float offset = 0.0f;
	for (auto item : Children)
	{
		if (Alignment == EAlignment::Vertical)
		{
			item->SetOffsetLocal(FFloat2(0.0f, offset));
			offset += item->GetSize().Y + Space;
		}
		else
		{
			item->SetOffsetLocal(FFloat2(offset, 0.0f));
			offset += item->GetSize().X + Space;
		}
	}
}

void LostCore::FListBox::SetAlignment(EAlignment alignment)
{
	Alignment = alignment;
}

void LostCore::FListBox::SetSpace(int32 val)
{
	Space = val;
}
