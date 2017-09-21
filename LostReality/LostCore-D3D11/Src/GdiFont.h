/*
* file GdiFont.h
*
* author luoxw
* date 2017/04/21
*
*
*/

#pragma once

namespace D3D11
{
	class FTexture2D;
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
		FTexture2D*				FontTexture;
		std::set<WCHAR>			Chars;
		std::set<LostCore::FCharDesc> CharDesc;

		FGdiFontProperty();
	};

	class FGdiFont : public LostCore::IFontInterface
	{
	public:
		FGdiFont();
		virtual ~FGdiFont() override;

		bool Initialize(LostCore::IRenderContext* rc, const LostCore::FFontConfig& config, WCHAR* chars, int32 sz) override;
		virtual std::vector<LostCore::ITexture*> GetFontTextures() const override;
		void Destroy();

		//bool DrawString(const std::string& str, 
		FTexture2D* GetTexture()
		{
			return Property.FontTexture;
		}

	private:
		FGdiFontProperty Property;
	};
}