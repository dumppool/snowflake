/*
* file BasicScene.cpp
*
* author luoxw
* date 2017/02/06
*
*
*/

#include "stdafx.h"
#include "BasicScene.h"
#include "RenderContextInterface.h"

using namespace LostCore;

FBasicScene::FBasicScene()
{
	Models.clear();
}

FBasicScene::~FBasicScene()
{
	Fini();

	assert(Models.size() == 0);
}

void FBasicScene::Tick()
{
	for (auto sm : Models)
	{
		if (sm != nullptr)
		{
			sm->Tick();
		}
	}
}

void FBasicScene::Draw()
{
	for (auto sm : Models)
	{
		if (sm != nullptr)
		{
			sm->Draw();
		}
	}
}

bool LostCore::FBasicScene::Config(const FJson & config)
{
	if (config.find(K_NODES) != config.end())
	{
		for (auto node : config[K_NODES])
		{
			assert(node.find(K_TYPE) != node.end() && "a node needs [type] section");
			assert(node.find(K_PATH) != node.end() && "a node needs [path] section");
			assert(node.find(K_TRANSFORM) != node.end() && "a node needs [transform] section");
			auto nodeType = (ESceneNodeType)(int32)node[K_TYPE];
			if (nodeType == ESceneNodeType::Model)
			{
				FBasicModel* model = nullptr;
				if ((model = FModelFactory::NewModel(node[K_PATH])) != nullptr)
				{
					model->SetWorldMatrix(node[K_TRANSFORM]);
					if (model != nullptr)
					{
						AddModel(model);
					}
				}
			}
		}
	}

	return true;
}

bool FBasicScene::Load(const string& url)
{
	FJson config;
	if (!FDirectoryHelper::Get()->GetSceneJson(url, config))
	{
		return false;
	}
	else
	{
		return Config(config);
	}
}

void LostCore::FBasicScene::Save(const string & url)
{
	FJson config;
	for (auto model : Models)
	{
		FJson modelConfig;
		modelConfig[K_TYPE] = (int32)ESceneNodeType::Model;
		modelConfig[K_PATH] = model->GetUrl();
		modelConfig[K_TRANSFORM] = model->GetWorldMatrix();
		config[K_NODES].push_back(modelConfig);
	}

	SaveJson(config, url);
}

void FBasicScene::AddModel(FBasicModel * sm)
{
	if (sm != nullptr && std::find(Models.begin(), Models.end(), sm) == Models.end())
	{
		Models.push_back(sm);
	}
}

void FBasicScene::RemoveModel(FBasicModel * sm)
{
	if (sm == nullptr)
	{
		return;
	}

	auto result = std::find(Models.begin(), Models.end(), sm);
	if (result != Models.end())
	{
		Models.erase(result);
	}
}

void LostCore::FBasicScene::ClearModels()
{
	for (auto& model : Models)
	{
		SAFE_DELETE(model);
	}

	Models.clear();
}

void LostCore::FBasicScene::Fini()
{
	ClearModels();
}
