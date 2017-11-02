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
	// FBasicGUI负责这个单例的初始化和释放.
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
		void UpdateRes();

		IFont* GetGdiFont();

	private:
		IFont* GdiFont;
	};
}

