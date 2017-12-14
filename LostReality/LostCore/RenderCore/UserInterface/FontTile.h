/*
* file FontTile.h
*
* author luoxw
* date 2017/11/14
*
*
*/

#pragma once

#include "Inc/BasicGUI.h"

namespace LostCore
{
	class FFontTile : public FRect
	{
	public:
		class IListener
		{
		public:
			virtual void OnFontTileCommitted(const FRectParameter&, const FCharacterDescription&) = 0;
		};

		FFontTile();
		virtual ~FFontTile() override;

		virtual void Update() override;
		virtual void Commit() override;

		void SetCharacter(WCHAR c);
		void SetListener(IListener* listener);

	protected:
		virtual void UpdateAndCommitRectBuffer() override;

	private:
		FCharacterDescription Character;
		IListener* Listener;
	};
}

