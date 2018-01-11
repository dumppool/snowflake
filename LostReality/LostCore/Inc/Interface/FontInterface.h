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
		MEMORY_ALLOC(FFontConfig);

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

	class FCharacterDescription
	{
	public:
		WCHAR Char;
		int32 X;
		int32 Y;
		int32 Width;
		int32 Height;

		FCharacterDescription() : FCharacterDescription(0)
		{
		}

		explicit FCharacterDescription(WCHAR wc)
			: FCharacterDescription(wc, 0, 0, 0, 0)
		{
		}

		FCharacterDescription(WCHAR wc, int32 x, int32 y, int32 w, int32 h)
			: Char(wc), X(x), Y(y), Width(w), Height(h)
		{
		}

		bool IsValid() const
		{
			return (Width > 0 && Height > 0);
		}

		bool operator<(const FCharacterDescription& rhs) const
		{
			return Char < rhs.Char;
		}
	};

	class FFontTextureDescription
	{
	public:
		int32 TextureWidth;
		int32 TextureHeight;
		int32 SpaceWidth;

		FFontTextureDescription() :TextureWidth(0), TextureHeight(0), SpaceWidth(0) {}
		bool IsValid() const { return TextureWidth > 0; }
		void Swap(FFontTextureDescription& rhs)
		{
			FFontTextureDescription tmp(*this);
			memcpy(this, &rhs, sizeof(tmp));
			memcpy(&rhs, &tmp, sizeof(tmp));
		}
	};

	class IFontClient
	{
	public:
		virtual void OnFontUpdated(const FFontTextureDescription& textureDescription,
			const set<FCharacterDescription>& characterDescriptions) = 0;
	};

	class IFont
	{
	public:
		virtual ~IFont() {}
		virtual void SetConfig(const LostCore::FFontConfig& config) = 0;
		virtual void RequestCharacters(const wstring characters) = 0;
		virtual void AddClient(IFontClient* client) = 0;
		virtual void RemoveClient(IFontClient* client) = 0;
		virtual void CommitShaderResource() = 0;
	};
}