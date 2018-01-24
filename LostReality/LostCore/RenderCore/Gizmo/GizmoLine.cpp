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

bool LostCore::FSegmentTool::ConstructPrimitive(const void* buf, uint32 sz)
{
	if (bConstructed)
	{
		return bConstructed;
	}

	assert(Primitive == nullptr);

	D3D11::WrappedCreateConstantBuffer(&ConstantBuffer);
	D3D11::WrappedCreatePrimitiveGroup(&Primitive);

	// TODO: ÅäÖÃ
	//Material->SetDepthStencilState(bDepthTest ? K_DEPTH_STENCIL_Z_WRITE : K_DEPTH_STENCIL_ALWAYS);
	Primitive->SetTopology(LostCore::EPrimitiveTopology::LineList);

	//bConstructed &= ConstantBuffer->Initialize(sizeof(World), false);
	ConstantBuffer->SetShaderSlot(SHADER_SLOT_MATRICES);
	ConstantBuffer->SetShaderFlags(SHADER_FLAG_VS);

	Primitive->SetVertexElement(VERTEX_COORDINATE3D|VERTEX_COLOR);
	Primitive->ConstructVB(buf, sz, GetAlignedSize(sizeof(FSegmentVertex), 16), false);

	bConstructed = true;
	return true;
}

void LostCore::FSegmentTool::DestroyPrimitive()
{
	if (ConstantBuffer != nullptr)
	{
		D3D11::WrappedDestroyConstantBuffer(forward<IConstantBuffer*>(ConstantBuffer));
		ConstantBuffer = nullptr;
	}

	if (Primitive != nullptr)
	{
		D3D11::WrappedDestroyPrimitiveGroup(forward<IPrimitive*>(Primitive));
		Primitive = nullptr;
	}

	bConstructed = false;
}

void LostCore::FSegmentTool::Commit()
{
	if (Data.size() == 0)
	{
		return;
	}

	uint32 sz = Data.size() * sizeof(FSegmentVertex);
	if (bConstructed)
	{
		Primitive->UpdateVB(Data.data(), sz, sizeof(FSegmentVertex));
	}
	else
	{
		ConstructPrimitive(Data.data(), sz);
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
