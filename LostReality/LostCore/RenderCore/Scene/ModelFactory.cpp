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
#include "BasicModel.h"

using namespace LostCore;

FBasicModel * LostCore::FModelFactory::NewModel(const string & url)
{
	FJson config;
	if (!FDirectoryHelper::Get()->GetModelJson(url, config))
	{
		return nullptr;
	}

	FBasicModel* model = nullptr;
	if (HAS_FLAGS(VERTEX_SKIN, config[K_VERTEX_ELEMENT]))
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
