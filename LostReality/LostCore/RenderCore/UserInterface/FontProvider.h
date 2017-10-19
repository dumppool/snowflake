/*
* file FontProvider.h
*
* author luoxw
* date 2017/04/25
*
*
*/

#pragma once

#include "Interface/FontInterface.h"

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

		void Initialize();
		void Destroy();
		void EndFrame();

		IFontInterface* GetGdiFont();

	private:
		IFontInterface* GdiFont;
	};
}

