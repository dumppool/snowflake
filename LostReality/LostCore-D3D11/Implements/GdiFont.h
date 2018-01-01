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

	class FGdiFontProperty
	{
	public:
		LostCore::FFontConfig	Config;

		float					CharHeight;
		FTexture2D*				FontTexture;
		set<WCHAR>			Characters;
		set<LostCore::FCharacterDescription>	CharacterDescriptions;
		LostCore::FFontTextureDescription			TextureDescription;

		FGdiFontProperty();
	};

	class FGdiFont : public LostCore::IFont
	{
	public:
		FGdiFont();
		virtual ~FGdiFont() override;

		virtual void SetConfig(const LostCore::FFontConfig& config) override;
		virtual void RequestCharacters(const wstring characters) override;
		virtual void AddClient(LostCore::IFontClient* client) override;
		virtual void RemoveClient(LostCore::IFontClient* client) override;
		virtual void CommitShaderResource() override;

		FTexture2D* GetTexture();

	private:
		bool Reload();
		void Destroy();

		static void ExecUpdate(void* p);
		static void ExecCommitShaderResource(void* p);

	private:
		FGdiFontProperty Property;

		LostCore::FFontConfig* PendingConfig;
		wstring PendingCharacters;
		mutex Mutex;

		vector<LostCore::IFontClient*> Clients;
	};
}