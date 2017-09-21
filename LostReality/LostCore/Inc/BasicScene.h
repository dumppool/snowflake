/*
* file BasicScene.h
*
* author luoxw
* date 2017/02/06
*
*
*/

#pragma once

#include "BasicInterface.h"
#include "BasicModel.h"
#include "BasicGUI.h"

namespace LostCore
{
	enum class ESceneNodeType : uint8
	{
		Undefined = 0,
		Model,
	};

	class FBasicScene
	{
	public:
		FBasicScene();
		virtual ~FBasicScene();

		virtual void Tick();
		virtual void Draw();
		virtual bool Config(const FJson& config);
		virtual bool Load(const string& url);
		virtual void Save(const string& url);

		virtual void AddModel(FBasicModel * sm);
		virtual void RemoveModel(FBasicModel * sm);
		virtual void ClearModels();

	private:
		void Fini();

		std::vector<FBasicModel*> Models;
	};
}