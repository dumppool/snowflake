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
#include "Src/ShaderManager.h"

D3D11::FForwardPipeline::FForwardPipeline()
{
}

D3D11::FForwardPipeline::~FForwardPipeline()
{
}

void D3D11::FForwardPipeline::Initialize()
{
	uint32 val = 0;
	while (val != (uint32)ERenderOrder::End)
	{
		RenderObjects[(ERenderOrder)val++] = vector<FRenderObject>();
	}
}

void D3D11::FForwardPipeline::Destroy()
{
	uint32 val = 0;
	while (val != (uint32)ERenderOrder::End)
	{
		assert(RenderObjects[(ERenderOrder)val].size() == 0);
	}
}

EPipeline D3D11::FForwardPipeline::GetEnum() const
{
	return EPipeline::Forward;
}

void D3D11::FForwardPipeline::CommitPrimitiveGroup(FPrimitiveGroup * pg)
{
	if (pg != nullptr)
	{
		Commiting.PrimitiveGroup = pg;
		auto& objs = RenderObjects.find(pg->GetRenderOrder());
		objs->second.push_back(Commiting);
		Commiting.Reset();
	}
}

void D3D11::FForwardPipeline::CommitBuffer(FConstantBuffer * buf)
{
	if (buf != nullptr)
	{
		Commiting.ConstantBuffers.push_back(buf);
	}
}

void D3D11::FForwardPipeline::BeginFrame()
{
}

void D3D11::FForwardPipeline::EndFrame()
{
}

void D3D11::FForwardPipeline::Render()
{ 
	for (auto& objs : RenderObjects)
	{
		for (auto& obj : objs.second)
		{
			FShaderKey key;
			key.LitMode = ELightingMode::Phone;
			key.VertexElement = obj.PrimitiveGroup->GetVertexElement();
			FShaderManager::Get()->GetShader(key)->Bind();

			for (auto buf : obj.ConstantBuffers)
			{
				buf->Bind();
			}

			obj.PrimitiveGroup->Draw();
		}

		objs.second.clear();
	}
}

