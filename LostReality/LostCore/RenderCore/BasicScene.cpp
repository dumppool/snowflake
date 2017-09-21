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
		for (auto& node : config[K_NODES])
		{
			auto nodeType = (ESceneNodeType)(int32)node[K_TYPE];
			if (nodeType == ESceneNodeType::Model)
			{
				FBasicModel* model = FModelFactory::NewModel(node[K_PATH]);
				if (model != nullptr)
				{
					model->SetWorldMatrix(node[K_TRANSFORM]);
					if (model != nullptr)
					{
						AddModel(model);
					}
				}
			}
			else if (nodeType == ESceneNodeType::Camera)
			{
				FBasicCamera* camera = FCameraFactory::NewCamera(node);
				if (camera != nullptr)
				{
					Cameras.push_back(camera);
				}
			}
		}

		if (Cameras.empty())
		{
			Cameras.push_back(new FBasicCamera);
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

FJson LostCore::FBasicScene::Save(const string & url)
{
	FJson config;
	for (auto model : Models)
	{
		config[K_NODES].push_back(model->Save());
	}

	for (auto cam : Cameras)
	{
		config[K_NODES].push_back(cam->Save());
	}

	SaveJson(config, url);
	return config;
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

FBasicCamera* LostCore::FBasicScene::GetCamera()
{
	if (Cameras.size() > 0)
	{
		return Cameras[0];
	}

	return nullptr;
}

FBasicModel* LostCore::FBasicScene::RayTest(const FRay & ray, FRay::FT & dist)
{
	FBasicModel* nearest = nullptr;
	FRay localRay(ray);

	// 逐个遍历测试场景对象.
	// TODO: Should be Actor with or without model.
	for (auto model : Models)
	{
		if (model != nullptr && model->RayTest(localRay, dist))
		{
			nearest = model;
			localRay.Distance = dist;
		}
	}

	return nearest;
}

void LostCore::FBasicScene::Fini()
{
	ClearModels();
}
