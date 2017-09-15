/*
* file ModelFactory.cpp
*
* author luoxw
* date 2017/09/05
*
*
*/

#include "stdafx.h"
#include "ModelFactory.h"

using namespace LostCore;

FBasicModel * LostCore::FModelFactory::NewModel(const FJson & config)
{
	auto rc = FGlobalHandler::Get()->GetRenderContext();
	if (rc == nullptr)
	{
		return nullptr;
	}

	assert(config.find(K_VERTEX_ELEMENT) != config.end());
	uint32 flag = config.value(K_VERTEX_ELEMENT, 0);
	FBasicModel* model = nullptr;
	if ((flag & EVertexElement::Skin) == EVertexElement::Skin)
	{
		model = new FSkeletalModel;
	}
	else
	{
		model = new FStaticModel;
	}

	if (model != nullptr)
	{
		model->Config(config);
	}

	return model;
}
