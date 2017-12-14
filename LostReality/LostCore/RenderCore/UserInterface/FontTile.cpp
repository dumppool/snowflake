/*
* file FontTile.cpp
*
* author luoxw
* date 2017/11/14
*
*
*/

#include "stdafx.h"
#include "FontTile.h"
#include "FontProvider.h"

using namespace LostCore;

LostCore::FFontTile::FFontTile()
	: Listener(nullptr)
{
	// Instancing渲染，没有单独的几何体.
	HasGeometry(false);
}

LostCore::FFontTile::~FFontTile()
{
	Listener = nullptr;
}

void LostCore::FFontTile::Update()
{
}

void LostCore::FFontTile::Commit()
{
	if (Listener != nullptr)
	{
		Listener->OnFontTileCommitted(FRectParameter(GetOffsetGlobal(), GetSize(), GetScaleGlobal()), Character);
	}
}

void LostCore::FFontTile::SetCharacter(WCHAR c)
{
	Character = FFontProvider::Get()->GetCharacter(c);
	SetSize(FFloat2(Character.Width, Character.Height));
}

void LostCore::FFontTile::SetListener(IListener* listener)
{
	Listener = listener;
}

void LostCore::FFontTile::UpdateAndCommitRectBuffer()
{
}
