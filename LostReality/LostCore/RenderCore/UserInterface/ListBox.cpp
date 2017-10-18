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
{
}

void LostCore::FListBox::Commit()
{
	float offset = 0.0f;
	for (auto item : Children)
	{
		if (Alignment == EAlignment::Vertical)
		{
			item->SetOriginLocal(FFloat2(0.0f, offset));
			offset += item->GetSize().Y;
		}
		else
		{
			item->SetOriginLocal(FFloat2(offset, 0.0f));
			offset += item->GetSize().X;
		}
	}

	FRect::Commit();
}
