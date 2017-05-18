/*
* file BasicStaticMesh.h
*
* author luoxw
* date 2017/02/10
*
*
*/

#pragma once

#include "BasicInterface.h"

namespace LostCore
{
	class IPrimitiveGroup;

	class FBasicMesh : public IBasicInterface
	{
	public:
		FBasicMesh();
		virtual ~FBasicMesh() override;

		virtual bool Init(IRenderContext * rc) override;
		virtual void Fini() override;
		virtual void Tick(float sec) override;
		virtual void Draw(IRenderContext * rc, float sec) override;

	public:
		virtual void AddPrimitiveGroup(IPrimitiveGroup* pg);
		virtual void RemovePrimitiveGroup(IPrimitiveGroup* pg);
		virtual void ClearPrimitiveGroup(std::function<void(IPrimitiveGroup*)> func);

	protected:
		std::vector<IPrimitiveGroup*> PrimitiveGroups;
	};
}