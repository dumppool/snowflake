/*
* file GizmoLine.cpp
*
* author luoxw
* date 2017/07/25
*
*
*/

#include "stdafx.h"
#include "Gizmo/GizmoLine.h"

#include "LostCore-D3D11.h"
using namespace D3D11;

using namespace std;
using namespace LostCore;

LostCore::FSegmentTool::FSegmentTool()
	: bConstructed(false), Material(nullptr), Primitive(nullptr), bDepthTest(true)
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

bool LostCore::FSegmentTool::ConstructPrimitive(const void* buf, uint32 bytes)
{
	if (bConstructed)
	{
		return bConstructed;
	}

	auto rc = FGlobalHandler::Get()->GetRenderContext();
	if (rc == nullptr)
	{
		return false;
	}

	assert(Material == nullptr && Primitive == nullptr);

	bConstructed = SSuccess == D3D11::WrappedCreateMaterial(&Material);
	bConstructed &= SSuccess == D3D11::WrappedCreatePrimitiveGroup(&Primitive);

	assert(bConstructed);

	// TODO: ÅäÖÃ
	Material->SetDepthStencilState(bDepthTest ? K_DEPTH_STENCIL_Z_WRITE : K_DEPTH_STENCIL_ALWAYS);
	Primitive->SetTopology(LostCore::EPrimitiveTopology::LineList);

	bConstructed &= Material->Initialize(rc, "default_xyzrgb.json");
	bConstructed &= Primitive->ConstructVB(rc, buf, bytes, GetAlignedSize(sizeof(FSegmentVertex), 16), false);


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

void LostCore::FSegmentTool::Draw()
{
	if (Data.size() == 0)
	{
		return;
	}

	auto rc = FGlobalHandler::Get()->GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	vector<uint8> buf(Data.size() * sizeof(FSegmentVertex));
	memcpy(&buf[0], &Data[0], buf.size());

	if (bConstructed)
	{
		Primitive->UpdateVB(rc, &buf[0], buf.size());
	}
	else
	{
		ConstructPrimitive(&buf[0], buf.size());
	}

	Material->UpdateConstantBuffer(rc, &World, sizeof(World));
	Material->Draw(rc, sec);
	Primitive->Draw(rc, sec);
}

void LostCore::FSegmentTool::AddSegment(const FSegmentData & seg)
{
	Data.push_back(FSegmentVertex(seg.StartPt, seg.StartPtColor));
	Data.push_back(FSegmentVertex(seg.StopPt, seg.StopPtColor));
}

void LostCore::FSegmentTool::SetWorldMatrix(const FFloat4x4 & mat)
{
	World = mat;
}

void LostCore::FSegmentTool::EnableDepthTest(bool enable)
{
	bDepthTest = enable;
	if (Material != nullptr)
	{
		Material->SetDepthStencilState(bDepthTest ? K_DEPTH_STENCIL_Z_WRITE : K_DEPTH_STENCIL_ALWAYS);
	}
}
