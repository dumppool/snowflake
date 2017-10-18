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
	WrappedCreateGdiFont(&GdiFont);

	// push & initialize ascii chars
	const int32 sz = 127 - '!';
	WCHAR chars[sz];
	for (int32 i = 0; i < sz; ++i)
	{
		chars[i] = '!' + i;
	}

	FFontConfig config;
	config.FontName = L"Consolas";
	config.Height = 13;
	config.bAntiAliased = false;
	config.bUnderline = false;
	config.bItalic = false;
	config.Weight = 0;
	config.Quality = NONANTIALIASED_QUALITY;
	config.CharSet = DEFAULT_CHARSET;
	GdiFont->Initialize(config, chars, sz);

	//std::wstring ws2 = L"的空空大口大口的看到v次";
	//GdiFont->Initialize(config, const_cast<wchar_t*>(ws2.c_str()), ws2.size());
}

void LostCore::FFontProvider::Destroy()
{
	WrappedDestroyGdiFont(forward<IFontInterface*>(GdiFont));
	GdiFont = nullptr;
}

void LostCore::FFontProvider::EndFrame()
{
	if (GdiFont != nullptr)
	{
		GdiFont->EndFrame();
	}
}

IFontInterface * LostCore::FFontProvider::GetGdiFont()
{
	return GdiFont;
}
