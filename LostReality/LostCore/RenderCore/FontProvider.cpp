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

void LostCore::FFontProvider::Init(IRenderContext * rc)
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
	config.FontName = L"����";
	config.Height = 36;
	config.bAntiAliased = false;
	config.bUnderline = false;
	config.bItalic = false;
	config.Weight = 0;
	config.Quality = NONANTIALIASED_QUALITY;
	config.CharSet = DEFAULT_CHARSET;
	GdiFont->Initialize(rc, config, chars, sz);

	//std::wstring ws2 = L"�Ŀտմ�ڴ�ڵĿ���v��";
	//GdiFont->Initialize(rc, config, const_cast<wchar_t*>(ws2.c_str()), ws2.size());
}

void LostCore::FFontProvider::Fini()
{
	WrappedDestroyGdiFont(forward<IFontInterface*>(GdiFont));
}

IFontInterface * LostCore::FFontProvider::GetGdiFont()
{
	return GdiFont;
}