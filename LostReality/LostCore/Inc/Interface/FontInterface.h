/*
* file FontInterface.h
*
* author luoxw
* date 2017/04/25
*
*
*/

#pragma once

namespace LostCore
{
	class FFontConfig
	{
	public:
		std::wstring	FontName;
		float			Height;
		uint32			Weight;
		bool			bAntiAliased;
		bool			bItalic;
		bool			bUnderline;
		uint32			CharSet;
		uint32			OutPrecision;
		uint32			ClipPrecision;
		uint32			Quality;
		uint32			PitchAndFamily;

		FFontConfig()
			: FontName(L"Consolas")
			, Height(14.f)
			, Weight(FW_NORMAL)
			, bAntiAliased(true)
			, bItalic(false)
			, bUnderline(false)
			, CharSet(DEFAULT_CHARSET)
			, OutPrecision(OUT_DEFAULT_PRECIS)
			, ClipPrecision(CLIP_DEFAULT_PRECIS)
			, Quality(CLEARTYPE_QUALITY)
			, PitchAndFamily(DEFAULT_PITCH | FF_SWISS)
		{
		}

		bool operator==(const FFontConfig& rhs)
		{
			return (FontName == rhs.FontName &&
				Height == rhs.Height &&
				Weight == rhs.Weight &&
				bAntiAliased == rhs.bAntiAliased &&
				bItalic == rhs.bItalic &&
				bUnderline == rhs.bUnderline &&
				CharSet == rhs.CharSet &&
				OutPrecision == rhs.OutPrecision &&
				ClipPrecision == rhs.ClipPrecision &&
				Quality == rhs.Quality &&
				PitchAndFamily == rhs.PitchAndFamily);
		}

		bool operator!=(const FFontConfig& rhs)
		{
			return !((*this) == rhs);
		}
	};

	class FCharDesc
	{
	public:
		WCHAR Char;
		int32 X;
		int32 Y;
		int32 Width;
		int32 Height;

		FCharDesc() : FCharDesc(0)
		{
		}

		explicit FCharDesc(WCHAR wc)
			: FCharDesc(wc, 0, 0, 0, 0)
		{
		}

		FCharDesc(WCHAR wc, int32 x, int32 y, int32 w, int32 h)
			: Char(wc), X(x), Y(y), Width(w), Height(h)
		{
		}

		bool IsValid() const
		{
			return (Width > 0 && Height > 0);
		}

		bool operator==(const FCharDesc& rhs) const
		{
			return Char == rhs.Char;
		}

		bool operator!=(const FCharDesc& rhs) const
		{
			return Char != rhs.Char;
		}

		bool operator<(const FCharDesc& rhs) const
		{
			return Char < rhs.Char;
		}

		bool operator<=(const FCharDesc& rhs) const
		{
			return Char <= rhs.Char;
		}

		bool operator>(const FCharDesc& rhs) const
		{
			return Char > rhs.Char;
		}

		bool operator >=(const FCharDesc& rhs) const
		{
			return Char >= rhs.Char;
		}

		bool operator==(WCHAR c) const
		{
			return Char == c;
		}

		bool operator!=(WCHAR c) const
		{
			return Char != c;
		}
	};

	struct FCharacterTexturePair
	{
		FCharDesc Desc;
		ITexture* Texture;

		FCharacterTexturePair(const FCharDesc& desc, ITexture* tex) : Desc(desc), Texture(tex) {}
		FCharacterTexturePair():Texture(nullptr) {}
	};

	class IFont
	{
	public:
		virtual ~IFont() {}
		virtual void Initialize(const LostCore::FFontConfig& config, const wstring& chars) = 0;
		virtual FCharacterTexturePair GetCharacter(WCHAR c) = 0;
		virtual FFontConfig GetConfig() const = 0;
		virtual void UpdateRes() = 0;
	};
}