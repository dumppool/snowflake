/*
* file TextSheet.h
*
* author luoxw
* date 2017/12/18
*
*
*/

#pragma once

#include "BasicGUI.h"

namespace LostCore
{
	class FTextBox;
	class FListBox;

	class FTextSheet : public FRect
	{
	public:
		FTextSheet();
		virtual ~FTextSheet() override;

		virtual void Update() override;

		void Initialize();
		void Destroy();

		void SetCaption(const string& caption);
		void SetHeader(const vector<string>& header);
		void AddRow(const vector<string>& row);
		void AddRows(const vector<vector<string>>& rows);

	private:
		void PrepareRows(int32 numRowsWanted);

		string Caption;
		vector<string> Header;
		vector<vector<string>> Rows;

		FListBox* MainList;
		FTextBox* CaptionBox;
		FListBox* ColumnList;
		vector<FListBox*> ColumnCtrls;
	};
}

