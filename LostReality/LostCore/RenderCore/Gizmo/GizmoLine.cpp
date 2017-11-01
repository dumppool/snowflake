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
	: bConstructed(false), Primitive(nullptr), bDepthTest(true)
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

bool LostCore::FSegmentTool::ConstructPrimitive(const FBuf& buf)
{
	if (bConstructed)
	{
		return bConstructed;
	}

	bConstructed = true;
	assert(Primitive == nullptr);

	D3D11::WrappedCreateConstantBuffer(ConstantBuffer);
	D3D11::WrappedCreatePrimitiveGroup(Primitive);

	// TODO: ÅäÖÃ
	//Material->SetDepthStencilState(bDepthTest ? K_DEPTH_STENCIL_Z_WRITE : K_DEPTH_STENCIL_ALWAYS);
	Primitive->SetTopology(LostCore::EPrimitiveTopology::LineList);

	//bConstructed &= ConstantBuffer->Initialize(sizeof(World), false);
	ConstantBuffer->SetShaderSlot(SHADER_SLOT_MATRICES);
	ConstantBuffer->SetShaderFlags(SHADER_FLAG_VS);

	Primitive->SetVertexElement(VERTEX_COLOR);
	Primitive->ConstructVB(buf, GetAlignedSize(sizeof(FSegmentVertex), 16), false);

	return true;
}

void LostCore::FSegmentTool::DestroyPrimitive()
{
	ConstantBuffer = nullptr;
	Primitive = nullptr;
	bConstructed = false;
}

void LostCore::FSegmentTool::Commit()
{
	if (Data.size() == 0)
	{
		return;
	}

	auto rc = FGlobalHandler::Get()->GetRenderContext();
	auto sec = FGlobalHandler::Get()->GetFrameTime();

	FBuf buf(Data.size() * sizeof(FSegmentVertex));
	memcpy(buf.data(), Data.data(), buf.size());

	if (bConstructed)
	{
		Primitive->UpdateVB(buf);
	}
	else
	{
		ConstructPrimitive(buf);
	}

	if (ConstantBuffer != nullptr)
	{
		FBuf buf;
		World.GetBuffer(buf);
		ConstantBuffer->UpdateBuffer(buf);
		ConstantBuffer->Commit();
	}

	if (Primitive != nullptr)
	{
		Primitive->Commit();
	}
}

void LostCore::FSegmentTool::AddSegment(const FSegmentData & seg)
{
	Data.push_back(FSegmentVertex(seg.StartPt, seg.StartPtColor));
	Data.push_back(FSegmentVertex(seg.StopPt, seg.StopPtColor));
}

void LostCore::FSegmentTool::SetWorldMatrix(const FFloat4x4 & mat)
{
	World.Matrix = mat;
}

void LostCore::FSegmentTool::EnableDepthTest(bool enable)
{
	bDepthTest = enable;
}
