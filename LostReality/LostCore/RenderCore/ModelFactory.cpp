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

FBasicModel * LostCore::FModelFactory::NewModel(const string & url)
{
	FJson config;
	if (!FDirectoryHelper::Get()->GetModelJson(url, config))
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

	model->SetUrl(url);
	model->Config(config);
	return model;
}
