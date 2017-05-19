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
		Model = 1,
	};

	class FBasicScene : public IBasicInterface
	{
	public:
		FBasicScene();
		virtual ~FBasicScene() override;

		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;
		virtual bool Load(IRenderContext * rc, const char* url) override;
		virtual void Fini() override;

		virtual void AddModel(FBasicModel * sm);
		virtual void RemoveStaticMesh(FBasicModel * sm);
		virtual void ClearStaticMesh(std::function<void(FBasicModel*)> func);

	protected:
		std::vector<FBasicModel*> StaticMeshArray;
	};
}