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
		StaticModel,
		SkeletalModel,
	};

	class FBasicScene : public IBasicInterface
	{
	public:
		FBasicScene();
		virtual ~FBasicScene() override;

		virtual void Tick() override;
		virtual void Draw() override;
		virtual bool Config(const FJson& config) override;
		virtual bool Load(const char* url) override;

		virtual void AddModel(FBasicModel * sm);
		virtual void RemoveModel(FBasicModel * sm);
		virtual void ClearModels();

	private:
		void Fini();

		std::vector<FBasicModel*> Models;
	};
}