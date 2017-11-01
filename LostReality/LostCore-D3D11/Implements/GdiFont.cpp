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
{
}

D3D11::FGdiFont::~FGdiFont()
{
	Destroy();
}

void D3D11::FGdiFont::Initialize(const LostCore::FFontConfig & config, const wstring& chars)
{
	if (FRenderContext::Get()->InRenderThread())
	{
		auto ret = ExecInitialize(config, chars);
		assert(ret);
	}
	else
	{
		auto p = shared_from_this();
		FRenderContext::Get()->PushCommand([=]()
		{
			auto ret = p->ExecInitialize(config, chars);
			assert(ret);
		});
	}
}

LostCore::FCharacterTexturePair D3D11::FGdiFont::GetCharacter(WCHAR c)
{
	auto it = Property.CharDesc.find(FCharDesc(c));
	if (it == Property.CharDesc.end())
	{
		PendingCharacters.append(&c);
		return FCharacterTexturePair();
	}

	return FCharacterTexturePair(FCharDesc(it->Char, it->X, it->Y, it->Width, it->Height), Property.FontTexture);
}

LostCore::FFontConfig D3D11::FGdiFont::GetConfig() const
{
	return Property.Config;
}

void D3D11::FGdiFont::UpdateRes()
{
	if (FRenderContext::Get()->InRenderThread())
	{
		ExecUpdateRes();
	}
	else
	{
		auto p = shared_from_this();
		FRenderContext::Get()->PushCommand([=]()
		{
			p->ExecUpdateRes();
		});
	}
}

void D3D11::FGdiFont::Destroy()
{
	Property.FontTexture = nullptr;
}

bool D3D11::FGdiFont::ExecInitialize(const LostCore::FFontConfig & config, const wstring& chars)
{
	const char* head = "FGdiFont::ExecInitialize";

	std::set<WCHAR> inputChars;
	inputChars.insert(chars.begin(), chars.end());
	inputChars.insert(Property.Chars.begin(), Property.Chars.end());
	bool bDirt = Property.Chars != inputChars;

	if (Property.Config == config && !bDirt)
	{
		return true;
	}

	Property.Config = config;
	Property.CharDesc.clear();
	Property.Chars.clear();

	std::vector<WCHAR> vecChars(inputChars.begin(), inputChars.end());

	Property.FontTexture = FTexture2DPtr(new FTexture2D);

	auto hint = Property.Config.bAntiAliased ? TextRenderingHintAntiAliasGridFit : TextRenderingHintSingleBitPerPixelGridFit;
	hint = TextRenderingHintClearTypeGridFit;

	LOGFONTW lf;
	wcscpy_s(lf.lfFaceName, Property.Config.FontName.c_str());
	lf.lfHeight = (int32)-Property.Config.Height;
	lf.lfWidth = 0;
	lf.lfWeight = Property.Config.Weight;
	lf.lfItalic = Property.Config.bItalic;
	lf.lfUnderline = Property.Config.bUnderline;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = Property.Config.CharSet;
	lf.lfOutPrecision = Property.Config.OutPrecision;
	lf.lfClipPrecision = Property.Config.ClipPrecision;
	lf.lfQuality = Property.Config.Quality;
	lf.lfPitchAndFamily = Property.Config.PitchAndFamily;

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

	int32 size = (int32)(Property.Config.Height) * vecChars.size();
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

	int32 numRows = (int32)(rect.Width / STexWidth) + 1;
	Property.TexHeight = (int32)(numRows * Property.CharHeight) + 1;

	int32 tsz = (int32)Property.Config.Height * 2;
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

	Bitmap bmp3(STexWidth, Property.TexHeight, PixelFormat32bppARGB);
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
		if (currX + charWidth >= STexWidth)
		{
			currX = 0;
			currY += (int32)(Property.CharHeight) + 1;
		}

		Property.Chars.insert(wc);
		Property.CharDesc.insert(LostCore::FCharDesc(wc, currX, currY, charWidth, (int32)Property.CharHeight));

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

	Property.SpaceWidth = rect.Width;

	BitmapData bmpData;
	if (Gdiplus::Ok != (ret = bmp3.LockBits(&Rect(0, 0, STexWidth, Property.TexHeight), ImageLockModeRead, PixelFormat32bppARGB, &bmpData)))
	{
		LVERR(head, "Bitmap::LockBits failed: %d", ret);
		RETURN_FALSE;
	}

	Property.FontTexture->Construct(STexWidth, Property.TexHeight, DXGI_FORMAT_B8G8R8A8_UNORM, false, false, true, false, bmpData.Scan0, STexWidth * 4);

	bmp3.UnlockBits(&bmpData);

	::ReleaseDC(NULL, hFont);
	return true;
}

void D3D11::FGdiFont::ExecUpdateRes()
{
	assert(FRenderContext::Get()->InRenderThread());
	if (!PendingCharacters.empty())
	{
		Initialize(Property.Config, PendingCharacters);
		PendingCharacters.clear();
	}
}

D3D11::FGdiFontProperty::FGdiFontProperty()
{
}
