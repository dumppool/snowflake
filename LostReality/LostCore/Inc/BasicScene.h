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
#include "BasicStaticMesh.h"
#include "BasicGUI.h"

namespace LostCore
{
	class FBasicScene : public IBasicInterface
	{
	public:
		FBasicScene();
		virtual ~FBasicScene() override;

		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;
		virtual bool Init(IRenderContext * rc) override;
		virtual void Fini() override;

		virtual void AddStaticMesh(FBasicStaticMesh * sm);
		virtual void RemoveStaticMesh(FBasicStaticMesh * sm);
		virtual void ClearStaticMesh(std::function<void(FBasicStaticMesh*)> func);

	protected:
		std::vector<FBasicStaticMesh*> StaticMeshArray;
		FRect* GUIRoot;
	};
}