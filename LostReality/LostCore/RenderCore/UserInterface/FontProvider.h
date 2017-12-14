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
#include "FontTile.h"

namespace LostCore
{
	// FBasicGUI负责这个单例的初始化和释放.
	class FFontProvider : public IFontClient, public FFontTile::IListener
	{
	public:
		static FFontProvider* Get()
		{
			static FFontProvider Inst;
			return &Inst;
		}

		FFontProvider();
		~FFontProvider();

		void Initialize();
		void Destroy();
		FFontConfig GetConfig() const;
		FFontTextureDescription GetTextureDescription();
		FCharacterDescription GetCharacter(WCHAR c);
		void OnFinishCommit();

		FFontTile* AllocTile();
		void DeallocTile(FFontTile* tile);

		virtual void OnFontUpdated(const FFontTextureDescription& td, const set<FCharacterDescription>& cd) override;
		virtual void OnFontTileCommitted(const FRectParameter&, const FCharacterDescription&) override;

	private:
		void ConstructPrimitive();

	private:
		FFontConfig Config;
		IFont* GdiFont;

		// Upload props
		wstring RequestCharacters;
		vector<FRectParameter> RenderRects;
		vector<FTextileParameter> RenderTextiles;
		wstring RenderCharacters;

		// Download props & mutex
		array<FFontTextureDescription, 2> TextureDescArray;
		array<set<FCharacterDescription>, 2> CharacterDescArray;
		mutex Mutex;

		// Instancing props
		IPrimitive* Primitive;
		vector<IInstancingData*> TransformData;
		vector<IInstancingData*> TextileData;
		static const int32 SNumPerBatch = 100;
	};
}

