/*
* file GizmoLine.cpp
*
* author luoxw
* date 2017/07/25
*
*
*/

#include "stdafx.h"
#include "GizmoLine.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace std;
using namespace LostCore;

LostCore::FSegmentTool::FSegmentTool()
	: bConstructed(false), Material(nullptr), Primitive(nullptr)
{
	ResetData();
}

LostCore::FSegmentTool::~FSegmentTool()
{
	ResetData();
	DestroyPrimitive();
}

void LostCore::FSegmentTool::ResetData()
{
	Data.clear();
}

bool LostCore::FSegmentTool::ConstructPrimitive(LostCore::IRenderContext* rc, const void* buf, uint32 bytes)
{
	if (bConstructed)
	{
		return bConstructed;
	}

	assert(Material == nullptr && Primitive == nullptr);

	bConstructed = SSuccess == D3D11::WrappedCreateMaterial_SceneObject(&Material);
	bConstructed &= SSuccess == D3D11::WrappedCreatePrimitiveGroup(&Primitive);

	assert(bConstructed);
	Primitive->SetTopology(LostCore::EPrimitiveTopology::LineList);

	// TODO: ÅäÖÃ
	bConstructed &= Material->Initialize(rc, "default_xyzrgb.json");
	bConstructed &= Primitive->ConstructVB(rc, buf, bytes, sizeof(FSegmentData), true);
	
	assert(bConstructed);
	return bConstructed;
}

void LostCore::FSegmentTool::DestroyPrimitive()
{
	if (!bConstructed)
	{
		return;
	}

	assert(Material != nullptr && Primitive != nullptr);

	D3D11::WrappedDestroyMaterial_SceneObject(forward<IMaterial*>(Material));
	D3D11::WrappedDestroyPrimitiveGroup(forward<IPrimitiveGroup*>(Primitive));
	Material = nullptr;
	Primitive = nullptr;
	bConstructed = false;
}

void LostCore::FSegmentTool::Draw(LostCore::IRenderContext * rc, float sec)
{
	if (Data.size() == 0)
	{
		return;
	}

	vector<uint8> buf(Data.size() * sizeof(FSegmentData));
	memcpy(&buf[0], &Data[0], buf.size());
	Data.clear();

	if (bConstructed)
	{
		Primitive->UpdateVB(rc, &buf[0], buf.size());
	}
	else
	{
		ConstructPrimitive(rc, &buf[0], buf.size());
	}

	Material->UpdateConstantBuffer(rc, &World, sizeof(World));
	Material->Draw(rc, sec);
	Primitive->Draw(rc, sec);
}

void LostCore::FSegmentTool::AddSegment(const FSegmentData & seg)
{
	Data.push_back(seg);
}

void LostCore::FSegmentTool::SetWorldMatrix(const FFloat4x4 & mat)
{
	World = mat;
}
