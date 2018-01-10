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

namespace LostCore
{
	enum class ESceneNodeType : uint8
	{
		Undefined = 0,
		Model,
		Camera,
	};

	class FBasicScene
	{
	public:
		FBasicScene();
		virtual ~FBasicScene();

		virtual void Tick();
		virtual bool Config(const FJson& config);
		virtual bool Load(const string& url);
		virtual FJson Save(const string& url);

		virtual void AddModel(FBasicModel * sm);
		virtual void RemoveModel(FBasicModel * sm);
		virtual void ClearModels();

		// TODO: 好像需要一个导演
		FBasicCamera* GetCamera();
		FBasicModel* RayTest(const FRay& ray, FRay::FT& dist);

	private:
		void Destroy();

		vector<FBasicModel*> Models;
		vector<FBasicCamera*> Cameras;
	};
}