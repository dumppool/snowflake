/*
* file ForwardPipeline.cpp
*
* author luoxw
* date 2017/09/21
*
*
*/

#include "stdafx.h"
#include "ForwardPipeline.h"

#include "Implements/ConstantBuffer.h"
#include "Implements/PrimitiveGroup.h"
#include "Implements/Texture.h"
#include "States/DepthStencilStateDef.h"
#include "States/BlendStateDef.h"
#include "States/RasterizerStateDef.h"
#include "Src/ShaderManager.h"

using namespace LostCore;

D3D11::FForwardPipeline::FForwardPipeline()
	: RenderObjects()
{
}

D3D11::FForwardPipeline::~FForwardPipeline()
{
}

void D3D11::FForwardPipeline::Initialize()
{
	//uint32 val = 0;
	//while (val != (uint32)ERenderOrder::End)
	//{
	//	RenderObjects.Ref()[(ERenderOrder)val++] = vector<FRenderObject>();
	//}
}

void D3D11::FForwardPipeline::Destroy()
{
	//uint32 val = 0;
	//while (val != (uint32)ERenderOrder::End)
	//{
	//	assert(RenderObjects.Ref()[(ERenderOrder)val].size() == 0);
	//}
}

EPipeline D3D11::FForwardPipeline::GetEnum() const
{
	return EPipeline::Forward;
}

void D3D11::FForwardPipeline::CommitPrimitiveGroup(FPrimitiveGroup* pg)
{
	static FStackCounterRequest SCounter("FForwardPipeline::CommitPrimitiveGroup");
	FScopedStackCounterRequest scopedCounter(SCounter);

	if (RenderObjects.empty())
	{
		uint32 val = 0;
		while (val != (uint32)ERenderOrder::End)
		{
			RenderObjects[(ERenderOrder)val++] = vector<FRenderObject>();
		}
	}

	assert(pg != nullptr);
	Committing.PrimitiveGroup = pg;
	auto& objs = RenderObjects.find(pg->GetRenderOrder());
	objs->second.push_back(Committing);
	Committing.Reset();
}

void D3D11::FForwardPipeline::CommitBuffer(FConstantBuffer* buf)
{
	assert(buf != nullptr);
	Committing.ConstantBuffers.push_back(buf);
}

void D3D11::FForwardPipeline::CommitShaderResource(FTexture2D* tex)
{
	assert(tex != nullptr);
	Committing.ShaderResources.push_back(tex);
}

void D3D11::FForwardPipeline::BeginFrame()
{
	Committing.Reset();
}

void D3D11::FForwardPipeline::EndFrame()
{
}

void D3D11::FForwardPipeline::RenderFrame()
{
	static FStackCounterRequest SCounter("FForwardPipeline::Render");
	FScopedStackCounterRequest scopedCounter(SCounter);

	auto cxt = FRenderContext::GetDeviceContext("FForwardPipeline::Render");

	for (auto& objs : RenderObjects)
	{
		if (objs.first == ERenderOrder::Opacity)
		{
			auto ds = FDepthStencilStateMap::Get()->GetState(DS_DEPTH_READ | DS_DEPTH_WRITE);
			cxt->OMSetDepthStencilState(ds.GetReference(), 0);

			auto blendMode = EBlendMode::None;
			auto blendWrite = EBlendWrite::RGB;
			uint32 blendFlags = (uint8(blendMode) << BLEND_MODE_OFFSET) | (uint8(blendWrite) << BLEND_WRITE_OFFSET);
			auto blend = FBlendStateMap::Get()->GetState(blendFlags);
			cxt->OMSetBlendState(blend.GetReference(), nullptr, ~0);

			auto rs = FRasterizerStateMap::Get()->GetState(RAS_CULL_BACK);
			cxt->RSSetState(rs.GetReference());
		}
		else if (objs.first == ERenderOrder::Translucent)
		{
			auto ds = FDepthStencilStateMap::Get()->GetState(DS_DEPTH_READ);
			cxt->OMSetDepthStencilState(ds.GetReference(), 0);

			auto blendMode = EBlendMode::AlphaBlend;
			auto blendWrite = EBlendWrite::RGBA;
			uint32 blendFlags = (uint8(blendMode) << BLEND_MODE_OFFSET) | (uint8(blendWrite) << BLEND_WRITE_OFFSET);
			auto blend = FBlendStateMap::Get()->GetState(blendFlags);
			cxt->OMSetBlendState(blend.GetReference(), nullptr, ~0);

			auto rs = FRasterizerStateMap::Get()->GetState(RAS_CULL_BACK);
			cxt->RSSetState(rs.GetReference());
		}
		else if (objs.first == ERenderOrder::UI)
		{
			auto ds = FDepthStencilStateMap::Get()->GetState(0);
			cxt->OMSetDepthStencilState(ds.GetReference(), 0);

			auto blendMode = EBlendMode::AlphaBlend;
			auto blendWrite = EBlendWrite::RGBA;
			uint32 blendFlags = (uint8(blendMode) << BLEND_MODE_OFFSET) | (uint8(blendWrite) << BLEND_WRITE_OFFSET);
			auto blend = FBlendStateMap::Get()->GetState(blendFlags);
			cxt->OMSetBlendState(blend.GetReference(), nullptr, ~0);

			auto rs = FRasterizerStateMap::Get()->GetState(RAS_CULL_BACK);
			cxt->RSSetState(rs.GetReference());
		}

		for (auto& obj : objs.second)
		{
			auto pg = obj.PrimitiveGroup;
			if (pg == nullptr)
			{
				continue;
			}

			FShaderKey key;
			key.LitMode = ELightingMode::Phone;
			key.VertexElement = pg->GetVertexElement();
			FShaderManager::Get()->GetShader(key)->Bind();

			for (auto buf : obj.ConstantBuffers)
			{
				buf->Bind();
			}

			for (auto tex : obj.ShaderResources)
			{
				tex->BindShaderResource(cxt);
			}

			pg->Draw();
		}

		objs.second.clear();
	}
}

