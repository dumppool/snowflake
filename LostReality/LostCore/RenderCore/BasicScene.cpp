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

void FBasicScene::Tick(float sec)
{
	for (auto sm : Models)
	{
		if (sm != nullptr)
		{
			sm->Tick(sec);
		}
	}
}

void FBasicScene::Draw(IRenderContext * rc, float sec)
{
	for (auto sm : Models)
	{
		if (sm != nullptr)
		{
			sm->Draw(rc, sec);
		}
	}
}

bool LostCore::FBasicScene::Config(IRenderContext * rc, const FJson & config)
{
	//assert(config.find("nodes") != config.end() && "a scene needs [nodes] section");
	if (config.find("nodes") != config.end())
	{
		for (auto node : config["nodes"])
		{
			assert(node.find("type") != node.end() && "a node needs [type] section");
			assert(node.find("path") != node.end() && "a node needs [path] section");
			assert(node.find("transform") != node.end() && "a node needs [transform] section");
			auto nodeType = (ESceneNodeType)(int32)node["type"];
			if (nodeType == ESceneNodeType::StaticModel ||
				nodeType == ESceneNodeType::SkeletalModel)
			{
				FBasicModel* m;
				if (nodeType == ESceneNodeType::StaticModel)
				{
					m = new FStaticModel;
				}
				else
				{
					m = new FSkeletalModel;
				}

				std::string p = node.find("path").value();
				m->Load(rc, p.c_str());
				AddModel(m);
				FFloat4x4 mat;
				//memcpy(&mat, &(node.find("transform").value()[0]), sizeof(mat));

				for (int y = 0; y < 4; ++y)
				{
					for (int x = 0; x < 4; ++x)
					{
						int r = (int)&(node.find("transform").value()[y * 4 + x]);
						mat.M[y][x] = node.find("transform").value()[y * 4 + x];
					}
				}

				m->SetWorldMatrix(mat);
			}
		}
	}

	return true;
}

bool FBasicScene::Load(IRenderContext * rc, const char* url)
{
	FJson config;
	if (!FDirectoryHelper::Get()->GetSceneJson("default_empty.json", config))
	{
		return false;
	}
	else
	{
		return Config(rc, config);
	}
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

void LostCore::FBasicScene::Fini()
{
	for (auto& model : Models)
	{
		SAFE_DELETE(model);
	}

	Models.clear();
}
