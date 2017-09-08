/*
* file FontProvider.h
*
* author luoxw
* date 2017/04/25
*
*
*/

#pragma once

#include "FontInterface.h"

namespace LostCore
{
	// FBasicGUI������������ĳ�ʼ�����ͷ�.
	class FFontProvider
	{
	public:
		static FFontProvider* Get()
		{
			static FFontProvider Inst;
			return &Inst;
		}

		FFontProvider() : GdiFont(nullptr) {}
		~FFontProvider();

		void Init();
		void Fini();

		IFontInterface* GetGdiFont();

	private:
		IFontInterface* GdiFont;
	};
}

