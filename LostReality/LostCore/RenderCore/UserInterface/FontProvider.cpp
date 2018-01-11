/*
* file FontProvider.cpp
*
* author luoxw
* date 2017/04/25
*
*
*/

#include "stdafx.h"
#include "FontProvider.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace std;
using namespace LostCore;

LostCore::FFontProvider::FFontProvider() 
	: GdiFont(nullptr)
	, Primitive(nullptr)
{

}

LostCore::FFontProvider::~FFontProvider()
{
	Destroy();
}

void LostCore::FFontProvider::Initialize()
{
	WrappedCreateGdiFont(&GdiFont);

	int32 curr = 32, last = 127;
	wstring initialString;
	initialString.resize(last - curr);
	for_each(initialString.begin(), initialString.end(), [&](WCHAR& elem)
	{
		elem = WCHAR(curr++);
	});

	Config.FontName = L"Consolas";
	Config.Height = 12;
	Config.bAntiAliased = false;
	Config.bUnderline = false;
	Config.bItalic = false;
	Config.Weight = 400;
	Config.Quality = NONANTIALIASED_QUALITY;
	Config.CharSet = DEFAULT_CHARSET;
	GdiFont->AddClient(this);
	GdiFont->SetConfig(Config);
	//GdiFont->RequestCharacters(initialString);

	ConstructPrimitive();
}

void LostCore::FFontProvider::Destroy()
{
	for (auto item : TransformData)
	{
		D3D11::WrappedDestroyInstancingData(forward<IInstancingData*>(item));
	}
	TransformData.clear();

	for (auto item : TextileData)
	{
		D3D11::WrappedDestroyInstancingData(forward<IInstancingData*>(item));
	}
	TextileData.clear();

	D3D11::WrappedDestroyPrimitiveGroup(forward<IPrimitive*>(Primitive));
	Primitive = nullptr;

	if (GdiFont != nullptr)
	{
		GdiFont->RemoveClient(this);
		D3D11::WrappedDestroyGdiFont(forward<IFont*>(GdiFont));
		GdiFont = nullptr;
	}
}

FFontConfig LostCore::FFontProvider::GetConfig() const
{
	return Config;
}

FFontTextureDescription LostCore::FFontProvider::GetTextureDescription()
{
	return TextureDescArray[0];
}

FCharacterDescription LostCore::FFontProvider::GetCharacter(WCHAR c)
{
	auto result = FCharacterDescription(c);
	auto& desc = CharacterDescArray[0];
	auto it = desc.find(result);
	if (it != desc.end())
	{
		return *it;
	}

	RequestCharacters.push_back(c);
	return result;
}

void LostCore::FFontProvider::OnFinishCommit()
{
	{
		lock_guard<mutex> lck(Mutex);
		TextureDescArray[0].Swap(TextureDescArray[1]);
		CharacterDescArray[0].swap(CharacterDescArray[1]);
	}

	if (GdiFont != nullptr && !RequestCharacters.empty())
	{
		GdiFont->RequestCharacters(RequestCharacters);
		RequestCharacters.clear();
	}

	// 准备足够的instancing buffer
	const int32 numRenderRects = RenderRects.size();
	int32 numNewBatches = (numRenderRects + SNumPerBatch - 1) / SNumPerBatch - TransformData.size();
	while ((numNewBatches--) > 0)
	{
		IInstancingData* data = nullptr;
		D3D11::WrappedCreateInstancingData(&data);
		data->SetVertexElement(INSTANCE_TRANSFORM2D);
		TransformData.push_back(data);

		D3D11::WrappedCreateInstancingData(&data);
		data->SetVertexElement(INSTANCE_TEXTILE);
		TextileData.push_back(data);
	}

	// Consume RenderRects & RenderTextiles
	int32 offset = SNumPerBatch, index = 0;
	for (; index < TransformData.size(); index++)
	{
		int32 start = index * SNumPerBatch;
		if (start >= numRenderRects)
		{
			break;
		}

		if (start + offset > numRenderRects)
		{
			offset = numRenderRects - start;
		}

		TransformData[index]->Update(RenderRects.data() + start, offset * sizeof(FRectParameter), offset);
		TextileData[index]->Update(RenderTextiles.data() + start, offset * sizeof(FTextileParameter), offset);
	}

	// 清除残留instancing数据造成的影响
	for (; index < TransformData.size(); index++)
	{
		TransformData[index]->Update(nullptr, 0, 0);
		TextileData[index]->Update(nullptr, 0, 0);
	}

	// Commit
	assert(TransformData.size() == TextileData.size());
	GdiFont->CommitShaderResource();
	for (int32 index = 0; index < TransformData.size(); index++)
	{
		TransformData[index]->Commit();
		TextileData[index]->Commit();
		Primitive->Commit();
	}

	RenderRects.clear();
	RenderTextiles.clear();
}

LostCore::FFontTile* LostCore::FFontProvider::AllocTile()
{
	auto tile = new FFontTile;
	tile->SetListener(this);
	return tile;
}

void LostCore::FFontProvider::DeallocTile(FFontTile* tile)
{
	SAFE_DELETE(tile);
}

void LostCore::FFontProvider::OnFontUpdated(const FFontTextureDescription& td, const set<FCharacterDescription>& cd)
{
	lock_guard<mutex> lck(Mutex);
	TextureDescArray[1] = td;
	CharacterDescArray[1] = cd;
}

void LostCore::FFontProvider::OnFontTileCommitted(const FRectParameter& rect, const FCharacterDescription& charDesc)
{
	RenderRects.push_back(rect);
	RenderTextiles.push_back(FTextileParameter(
		FFloat2(float(charDesc.X)/TextureDescArray[0].TextureWidth, float(charDesc.Y)/TextureDescArray[0].TextureHeight),
		FFloat2(float(charDesc.Width)/TextureDescArray[0].TextureWidth, float(charDesc.Height)/TextureDescArray[0].TextureHeight)));
}

void LostCore::FFontProvider::ConstructPrimitive()
{
	if (Primitive != nullptr)
	{
		return;
	}

	WrappedCreatePrimitiveGroup(&Primitive);
	Primitive->SetRenderOrder(ERenderOrder::UI);
	Primitive->SetVertexElement(FRectVertex::GetVertexElement());
	Primitive->SetTopology(EPrimitiveTopology::TriangleList);
	Primitive->ConstructVB(FRectVertex::GetDefaultVertices(FColor128(~0)),
		FRectVertex::GetDefaultSize(), sizeof(FRectVertex), false);
	Primitive->ConstructIB(*FRectVertex::GetDefaultIndices(), sizeof(int16), false);
}
