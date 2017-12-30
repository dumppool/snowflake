/*
* file TextSheet.cpp
*
* author luoxw
* date 2017/12/18
*
*
*/

#include "stdafx.h"
#include "TextSheet.h"
#include "TextBox.h"
#include "ListBox.h"

using namespace LostCore;

LostCore::FTextSheet::FTextSheet()
	: MainList(nullptr)
	, CaptionBox(nullptr)
	, ColumnList(nullptr)
{

}

LostCore::FTextSheet::~FTextSheet()
{
	Destroy();
}

void LostCore::FTextSheet::Initialize()
{
	assert(MainList == nullptr && CaptionBox == nullptr && ColumnList == nullptr);

	MainList = new FListBox;
	MainList->SetAlignment(FListBox::EAlignment::Vertical);
	AddChild(MainList);

	CaptionBox = new FTextBox;
	MainList->AddChild(CaptionBox);

	ColumnList = new FListBox;
	ColumnList->SetAlignment(FListBox::EAlignment::Horizontal);
	MainList->AddChild(ColumnList);
}

void LostCore::FTextSheet::Destroy()
{
	// FRect::Destroy已经销毁了所有控件
}

void LostCore::FTextSheet::SetCaption(const string& caption)
{
	Caption = caption;
}

void LostCore::FTextSheet::SetHeader(const vector<string>& header)
{
	Header = header;
}

void LostCore::FTextSheet::AddRow(const vector<string>& row)
{
	// 不支持不同大小的数组
	if (!Header.empty())
	{
		assert(Header.size() == row.size());
	}
	else if (!Rows.empty())
	{
		assert(Rows.back().size() == row.size());
	}

	Rows.push_back(row);
}

void LostCore::FTextSheet::PrepareRows(int32 numRowsWanted)
{
	if (ColumnCtrls.empty())
	{
		return;
	}

	auto dealloc = [](FRect* child) {SAFE_DELETE(child); };
	for (auto item : ColumnCtrls)
	{
		while (numRowsWanted < item->NumChildren())
		{
			item->PopChild(dealloc);
		}

		while (numRowsWanted > item->NumChildren())
		{
			auto child = new FTextBox;
			item->AddChild(child);
		}
	}
}

void LostCore::FTextSheet::Update()
{
	if (CaptionBox != nullptr)
	{
		CaptionBox->SetText(Caption);
	}

	if (ColumnList == nullptr)
	{
		return;
	}

	if (!Rows.empty() && !Rows.back().empty())
	{
		// 准备足够数量的Columns,或回收多余的部分
		const auto numColumnsWanted = Rows.back().size();
		while (numColumnsWanted < ColumnCtrls.size())
		{
			auto ctrl = ColumnCtrls.back();
			ctrl->Detach();
			SAFE_DELETE(ctrl);
			ColumnCtrls.pop_back();
		}

		while (numColumnsWanted > ColumnCtrls.size())
		{
			auto ctrl = new FListBox;
			ctrl->SetAlignment(FListBox::EAlignment::Vertical);
			ColumnList->AddChild(ctrl);
			ColumnCtrls.push_back(ctrl);
		}

		// 为每个Column准备足够数量的控件,或回收多余的部分
		const auto numRowsWanted = Rows.size() + Header.empty() ? 0 : 1;
		PrepareRows(numRowsWanted);

		// 填充文本内容
		for (int32 row = 0; row < numRowsWanted; ++row)
		{
			for (int32 col = 0; col < numColumnsWanted; ++col)
			{
				auto ctrl = dynamic_cast<FTextBox*>(ColumnCtrls[col]->GetChild(row));
				assert(ctrl != nullptr);

				if (!Header.empty())
				{
					if (row == 0)
					{
						ctrl->SetText(Header[col]);
					}
					else
					{
						ctrl->SetText(Rows[row - 1][col]);
					}
				}
				else
				{
					ctrl->SetText(Rows[row][col]);
				}
			}
		}

		Rows.clear();
	}
}
