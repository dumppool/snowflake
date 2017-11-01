/*
* file GdiFont.h
*
* author luoxw
* date 2017/04/21
*
*
*/

#pragma once

#include "Implements/Texture.h"

namespace D3D11
{
	static const int32 STexWidth = 1024;
	//static const WCHAR SStartChar = '!';
	//static const WCHAR SEndChar = 127;
	//static const int32 SNumChars = int32(SEndChar - SStartChar);

	class FGdiFontProperty
	{
	public:
		LostCore::FFontConfig	Config;

		float					CharHeight;
		int32					TexHeight;
		float					SpaceWidth;
		FTexture2DPtr			FontTexture;
		std::set<WCHAR>			Chars;
		std::set<LostCore::FCharDesc> CharDesc;

		FGdiFontProperty();
	};

	class FGdiFont : public LostCore::IFont, public enable_shared_from_this<FGdiFont>
	{
	public:
		FGdiFont();
		virtual ~FGdiFont() override;

		virtual void Initialize(const LostCore::FFontConfig& config, const wstring& chars) override;
		virtual LostCore::FCharacterTexturePair GetCharacter(WCHAR c) override;
		virtual LostCore::FFontConfig GetConfig() const override;
		virtual void UpdateRes() override;

		void Destroy();

		FTexture2DPtr GetTexture()
		{
			return Property.FontTexture;
		}

	private:
		bool ExecInitialize(const LostCore::FFontConfig& config, const wstring& chars);
		void ExecUpdateRes();

	private:
		FGdiFontProperty Property;
		wstring PendingCharacters;
	};
}