/*
* file BasicStaticMesh.cpp
*
* author luoxw
* date 2017/02/10
*
*
*/

#include "stdafx.h"
#include "BasicStaticMesh.h"
#include "PrimitiveGroupInterface.h"

using namespace LostCore;

LostCore::FBasicMesh::FBasicMesh()
{
	PrimitiveGroups.clear();
}

LostCore::FBasicMesh::~FBasicMesh()
{
	assert(PrimitiveGroups.size() == 0);
}

bool LostCore::FBasicMesh::Init(IRenderContext * rc)
{
	return false;
}

void LostCore::FBasicMesh::Fini()
{
	ClearPrimitiveGroup([](IPrimitiveGroup* p) { delete p; });
}

void LostCore::FBasicMesh::Tick(float sec)
{
	for (auto pg : PrimitiveGroups)
	{
		if (pg != nullptr)
		{
			//pg->Tick(sec);
		}
	}
}

void LostCore::FBasicMesh::Draw(IRenderContext * rc, float sec)
{
	for (auto pg : PrimitiveGroups)
	{
		if (pg != nullptr)
		{
			pg->Draw(rc, sec);
		}
	}
}

void LostCore::FBasicMesh::AddPrimitiveGroup(IPrimitiveGroup* pg)
{
	if (pg != nullptr && std::find(PrimitiveGroups.begin(), PrimitiveGroups.end(), pg) == PrimitiveGroups.end())
	{
		PrimitiveGroups.push_back(pg);
	}
}

void LostCore::FBasicMesh::RemovePrimitiveGroup(IPrimitiveGroup* pg)
{
	if (pg == nullptr)
	{
		return;
	}

	auto it = std::find(PrimitiveGroups.begin(), PrimitiveGroups.end(), pg);
	if (it != PrimitiveGroups.end())
	{
		PrimitiveGroups.erase(it);
	}
}

void LostCore::FBasicMesh::ClearPrimitiveGroup(std::function<void(IPrimitiveGroup*)> func)
{
	if (func != nullptr)
	{
		for (auto pg : PrimitiveGroups)
		{
			if (pg != nullptr)
			{
				func(pg);
			}
		}
	}

	PrimitiveGroups.clear();
}