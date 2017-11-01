/*
* file FontProvider.cpp
*
* author luoxw
* date 2017/04/25
*
*
*/

#include "stdafx.h"
#include "FontProvider.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace std;
using namespace LostCore;

LostCore::FFontProvider::~FFontProvider()
{
	Destroy();

	assert(GdiFont == nullptr);
}

void LostCore::FFontProvider::Initialize()
{
	WrappedCreateGdiFont(GdiFont);

	int32 curr = 32, last = 127;
	wstring initialString;
	initialString.resize(last - curr);
	for_each(initialString.begin(), initialString.end(), [&](WCHAR& elem)
	{
		elem = WCHAR(curr++);
	});

	FFontConfig config;
	config.FontName = L"Consolas";
	config.Height = 13;
	config.bAntiAliased = false;
	config.bUnderline = false;
	config.bItalic = false;
	config.Weight = 0;
	config.Quality = NONANTIALIASED_QUALITY;
	config.CharSet = DEFAULT_CHARSET;
	GdiFont->Initialize(config, initialString);
}

void LostCore::FFontProvider::Destroy()
{
	GdiFont = nullptr;
}

void LostCore::FFontProvider::UpdateRes()
{
	if (GdiFont != nullptr)
	{
		GdiFont->UpdateRes();
	}
}

IFontPtr LostCore::FFontProvider::GetGdiFont()
{
	return GdiFont;
}
