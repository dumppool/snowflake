/*
* file GdiFont.cpp
*
* author luoxw
* date 2017/04/21
*
*
*/
#include "stdafx.h"
#include "GdiFont.h"

using namespace LostCore;
using namespace D3D11;
using namespace Gdiplus;

#define RETURN_FALSE {\
::ReleaseDC(NULL, hFont);\
return false; }

D3D11::FGdiFont::FGdiFont()
	: PendingConfig(nullptr)
{
	assert(!FRenderContext::Get()->InRenderThread());
	FRenderContext::Get()->AddUpdateCommand(FContextCommand(this, &ExecUpdate));
}

D3D11::FGdiFont::~FGdiFont()
{
	Destroy();
}

void D3D11::FGdiFont::SetConfig(const LostCore::FFontConfig & config)
{
	assert(!FRenderContext::Get()->InRenderThread());
	lock_guard<mutex> lck(Mutex);
	SAFE_DELETE(PendingConfig);
	PendingConfig = new FFontConfig(config);
}

void D3D11::FGdiFont::RequestCharacters(const wstring characters)
{
	assert(!FRenderContext::Get()->InRenderThread());
	lock_guard<mutex> lck(Mutex);
	PendingCharacters.append(characters);
}

void D3D11::FGdiFont::AddClient(IFontClient * client)
{
	assert(!FRenderContext::Get()->InRenderThread());
	lock_guard<mutex> lck(Mutex);
	Clients.push_back(client);
}

void D3D11::FGdiFont::RemoveClient(IFontClient * client)
{
	assert(!FRenderContext::Get()->InRenderThread());
	lock_guard<mutex> lck(Mutex);
	auto it = find(Clients.begin(), Clients.end(), client);
	if (it != Clients.end())
	{
		Clients.erase(it);
	}
}

void D3D11::FGdiFont::CommitShaderResource()
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecCommitShaderResource(this);
	}
	else
	{
		FRenderContext::Get()->PushCommand(FContextCommand(this, &FGdiFont::ExecCommitShaderResource));
	}
}

void D3D11::FGdiFont::Destroy()
{
	assert(FRenderContext::Get()->InRenderThread());
	FRenderContext::Get()->RemoveUpdateCommand(FContextCommand(this, &FGdiFont::ExecUpdate));
	SAFE_DELETE(Property.FontTexture);
	SAFE_DELETE(PendingConfig);
}

void D3D11::FGdiFont::ExecUpdate(void* p)
{
	assert(FRenderContext::Get()->InRenderThread());
	assert(p != nullptr && p != (void*)0xdddddddd && p != (void*)0xcccccccc);

	auto pthis = (FGdiFont*)p;
	pthis->Reload();
	for (auto& client : pthis->Clients)
	{
		client->OnFontUpdated(pthis->Property.TextureDescription, pthis->Property.CharacterDescriptions);
	}
}

void D3D11::FGdiFont::ExecCommitShaderResource(void* p)
{
	assert(FRenderContext::Get()->InRenderThread());
	auto pthis = (FGdiFont*)p;
	auto tex = pthis->Property.FontTexture;
	if (tex != nullptr)
	{
		tex->CommitShaderResource();
	}
}

FTexture2D * D3D11::FGdiFont::GetTexture()
{
	assert(FRenderContext::Get()->InRenderThread());
	return Property.FontTexture;
}

bool D3D11::FGdiFont::Reload()
{
	assert(FRenderContext::Get()->InRenderThread());

	const char* head = "FGdiFont::Reload";
	bool dirtChars = false;
	bool dirtConfig = false;
	std::set<WCHAR> inputChars;
	{
		lock_guard<mutex> lck(Mutex);
		inputChars.insert(PendingCharacters.begin(), PendingCharacters.end());

		if (PendingConfig != nullptr)
		{
			Property.Config = *PendingConfig;
			SAFE_DELETE(PendingConfig);
			dirtConfig = true;
		}
	}

	inputChars.insert(Property.Characters.begin(), Property.Characters.end());
	dirtChars = Property.Characters != inputChars;
	if (!dirtConfig && !dirtChars)
	{
		return true;
	}

	auto& config = Property.Config;

	auto& td = Property.TextureDescription;
	td.TextureWidth = STexWidth;

	Property.CharacterDescriptions.clear();
	Property.Characters.clear();

	std::vector<WCHAR> vecChars(inputChars.begin(), inputChars.end());

	Property.FontTexture = (new FTexture2D);

	auto hint = config.bAntiAliased ? TextRenderingHintAntiAliasGridFit : TextRenderingHintSingleBitPerPixelGridFit;
	hint = TextRenderingHintClearTypeGridFit;

	LOGFONTW lf;
	wcscpy_s(lf.lfFaceName, config.FontName.c_str());
	lf.lfHeight = (int32)-config.Height;
	lf.lfWidth = 0;
	lf.lfWeight = config.Weight;
	lf.lfItalic = config.bItalic;
	lf.lfUnderline = config.bUnderline;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = config.CharSet;
	lf.lfOutPrecision = config.OutPrecision;
	lf.lfClipPrecision = config.ClipPrecision;
	lf.lfQuality = config.Quality;
	lf.lfPitchAndFamily = config.PitchAndFamily;

	ULONG_PTR token = NULL;
	GdiplusStartupInput startupInput(NULL, TRUE, TRUE);
	GdiplusStartupOutput startupOutput;
	Gdiplus::Status ret;
	if (Gdiplus::Ok != (ret = GdiplusStartup(&token, &startupInput, &startupOutput)))
	{
		LVERR(head, "GdiplusStartup failed: %d", ret);
		return false;
	}

	HDC hFont = ::GetDC(NULL);
	Gdiplus::Font font(hFont, &lf);
	if (Gdiplus::Ok != (ret = font.GetLastStatus()))
	{
		LVERR(head, "Gdiplus::Font::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	int32 size = (int32)(config.Height) * vecChars.size();
	Bitmap bmp(size, size, PixelFormat32bppARGB);
	if (Gdiplus::Ok != (ret = bmp.GetLastStatus()))
	{
		LVERR(head, "Bitmap::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	Graphics graphics(&bmp);
	if (Gdiplus::Ok != (ret = graphics.GetLastStatus()))
	{
		LVERR(head, "Graphics::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	if (Gdiplus::Ok != (ret = graphics.SetTextRenderingHint(hint)))
	{
		LVERR(head, "Graphics::SetTextRenderingHint failed: %d", ret);
		RETURN_FALSE;
	}

	Property.CharHeight = font.GetHeight(&graphics);

	RectF rect;
	if (Gdiplus::Ok != (ret = graphics.MeasureString(vecChars.data(), vecChars.size(), &font, PointF(0, 0), &rect)))
	{
		LVERR(head, "Graphics::MeasureString failed: %d", ret);
		RETURN_FALSE;
	}

	int32 numRows = (int32)(rect.Width / td.TextureWidth) + 1;
	td.TextureHeight = (int32)(numRows * Property.CharHeight) + 1;

	int32 tsz = (int32)config.Height * 2;
	Bitmap bmp2(tsz, tsz, PixelFormat32bppARGB);
	if (Gdiplus::Ok != (ret = bmp2.GetLastStatus()))
	{
		LVERR(head, "Bitmap::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	Graphics graphics2(&bmp2);
	if (Gdiplus::Ok != (ret = graphics2.GetLastStatus()))
	{
		LVERR(head, "Graphics::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	if (Gdiplus::Ok != (ret = graphics2.SetTextRenderingHint(hint)))
	{
		LVERR(head, "Graphics::SetTextRenderingHint failed: %d", ret);
		RETURN_FALSE;
	}

	Bitmap bmp3(td.TextureWidth, td.TextureHeight, PixelFormat32bppARGB);
	if (Gdiplus::Ok != (ret = bmp3.GetLastStatus()))
	{
		LVERR(head, "Bitmap::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	Graphics graphics3(&bmp3);
	if (Gdiplus::Ok != (ret = graphics3.GetLastStatus()))
	{
		LVERR(head, "Graphics::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	if (Gdiplus::Ok != (ret = graphics3.Clear(Color(0, 255, 255, 255))))
	{
		LVERR(head, "Graphics::Clear failed: %d", ret);
		RETURN_FALSE;
	}

	if (Gdiplus::Ok != (ret = graphics3.SetCompositingMode(CompositingModeSourceCopy)))
	{
		LVERR(head, "Graphics::SetCompositingMode failed: %d", ret);
		RETURN_FALSE;
	}

	SolidBrush brush(Color(255, 255, 255, 255));
	if (Gdiplus::Ok != (ret = brush.GetLastStatus()))
	{
		LVERR(head, "SolidBrush::GetLastStatus failed: %d", ret);
		RETURN_FALSE;
	}

	int32 currX = 0;
	int32 currY = 0;
	for (auto it = inputChars.begin(); it != inputChars.end(); ++it)
	{
		WCHAR wc = *it;

		// 跳过不绘制的字符
		if (wc == ' ')
		{
			Property.Characters.insert(wc);
			Property.CharacterDescriptions.insert(FCharacterDescription(wc));
			continue;
		}

		if (Gdiplus::Ok != (ret = graphics2.Clear(Color(0, 255, 255, 255))))
		{
			LVERR(head, "graphics2::Clear failed: %d", ret);
			RETURN_FALSE;
		}

		if (Gdiplus::Ok != (ret = graphics2.DrawString(&wc, 1, &font, PointF(0, 0), &brush)))
		{
			LVERR(head, "graphics2::DrawString failed: %d", ret);
			RETURN_FALSE;
		}

		int32 minX = 0;
		for (int32 x = 0; x < tsz; ++x)
		{
			for (int32 y = 0; y < tsz; ++y)
			{
				Color col;
				if (Gdiplus::Ok != (ret = bmp2.GetPixel(x, y, &col)))
				{
					LVERR(head, "Bitmap::GetPixel failed: %d", ret);
					RETURN_FALSE;
				}

				if (col.GetAlpha() > 0)
				{
					minX = x;
					x = tsz;
					break;
				}
			}
		}

		int32 maxX = tsz - 1;
		for (int32 x = tsz - 1; x >= 0; --x)
		{
			for (int32 y = 0; y < tsz; ++y)
			{
				Color col;
				if (Gdiplus::Ok != (ret = bmp2.GetPixel(x, y, &col)))
				{
					LVERR(head, "Bitmap::GetPixel failed: %d", ret);
					RETURN_FALSE;
				}

				if (col.GetAlpha() > 0)
				{
					maxX = x;
					x = -1;
					break;
				}
			}
		}

		int32 charWidth = maxX - minX + 1;
		if (currX + charWidth >= td.TextureWidth)
		{
			currX = 0;
			currY += (int32)(Property.CharHeight) + 1;
		}

		Property.Characters.insert(wc);
		Property.CharacterDescriptions.insert(FCharacterDescription(wc, currX, currY, charWidth, (int32)Property.CharHeight));

		int32 height = (int32)(Property.CharHeight) + 1;
		if (Gdiplus::Ok != (ret = graphics3.DrawImage(&bmp2, currX, currY, minX, 0, charWidth, height, UnitPixel)))
		{
			LVERR(head, "Graphics::DrawImage failed: %d", ret);
			RETURN_FALSE;
		}

		currX += charWidth + 1;
	}

	const WCHAR space = ' ';
	if (Gdiplus::Ok != (ret = graphics2.MeasureString(&space, 1, &font, PointF(0, 0), &rect)))
	{
		LVERR(head, "Graphics::MeasureString failed: %d", ret);
		RETURN_FALSE;
	}

	// 如果字符串里有空格
	td.SpaceWidth = rect.Width;
	Property.CharacterDescriptions.insert(FCharacterDescription(space, 0, 0, rect.Width, (int32)Property.CharHeight));

	BitmapData bmpData;
	if (Gdiplus::Ok != (ret = bmp3.LockBits(&Rect(0, 0, td.TextureWidth, 
		td.TextureHeight), ImageLockModeRead, PixelFormat32bppARGB, &bmpData)))
	{
		LVERR(head, "Bitmap::LockBits failed: %d", ret);
		RETURN_FALSE;
	}

	Property.FontTexture->Construct(td.TextureWidth, td.TextureHeight,
		DXGI_FORMAT_B8G8R8A8_UNORM, false, false, true, false, bmpData.Scan0, td.TextureWidth * 4);

	bmp3.UnlockBits(&bmpData);

	::ReleaseDC(NULL, hFont);
	return true;
}

D3D11::FGdiFontProperty::FGdiFontProperty()
{
}
