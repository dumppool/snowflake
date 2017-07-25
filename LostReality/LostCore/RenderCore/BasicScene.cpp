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
	StaticMeshArray.clear();
}

FBasicScene::~FBasicScene()
{
	//assert(StaticMeshArray.size() == 0);
}

void FBasicScene::Tick(float sec)
{
	for (auto sm : StaticMeshArray)
	{
		if (sm != nullptr)
		{
			sm->Tick(sec);
		}
	}
}

void FBasicScene::Draw(IRenderContext * rc, float sec)
{
	for (auto sm : StaticMeshArray)
	{
		if (sm != nullptr)
		{
			sm->Draw(rc, sec);
		}
	}
}

bool FBasicScene::Load(IRenderContext * rc, const char* url)
{
	FJson config;
	if (!FDirectoryHelper::Get()->GetSceneJson("default_empty.json", config))
	{
		return false;
	}

	assert(config.find("nodes") != config.end() && "a scene needs [nodes] section");
	for (auto node : config["nodes"])
	{
		assert(node.find("type") != node.end() && "a node needs [type] section");
		assert(node.find("path") != node.end() && "a node needs [path] section");
		assert(node.find("transform") != node.end() && "a node needs [transform] section");
		if (node["type"] == (int)ESceneNodeType::Model)
		{
			FBasicModel* m = new FBasicModel;
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

	return true;
}

void FBasicScene::Fini()
{
	//assert(0);
}

void FBasicScene::AddModel(FBasicModel * sm)
{
	if (sm != nullptr && std::find(StaticMeshArray.begin(), StaticMeshArray.end(), sm) == StaticMeshArray.end())
	{
		StaticMeshArray.push_back(sm);
	}
}

void FBasicScene::RemoveStaticMesh(FBasicModel * sm)
{
	if (sm == nullptr)
	{
		return;
	}

	auto result = std::find(StaticMeshArray.begin(), StaticMeshArray.end(), sm);
	if (result != StaticMeshArray.end())
	{
		StaticMeshArray.erase(result);
	}
}

void FBasicScene::ClearStaticMesh(std::function<void(FBasicModel*)> func)
{
	auto clear = (func != nullptr ? func : [](FBasicModel* p) { delete p; });
	for (auto sm : StaticMeshArray)
	{
		if (sm != nullptr)
		{
			clear(sm);
		}
	}

	StaticMeshArray.clear();
}
