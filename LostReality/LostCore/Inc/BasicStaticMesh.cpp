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

LostCore::FBasicStaticMesh::FBasicStaticMesh()
{
	PrimitiveGroups.clear();
}

LostCore::FBasicStaticMesh::~FBasicStaticMesh()
{
	assert(PrimitiveGroups.size() == 0);
}

bool LostCore::FBasicStaticMesh::Init(const char* name, IRenderContext * rc)
{
	return false;
}

void LostCore::FBasicStaticMesh::Fini()
{
	ClearPrimitiveGroup([](IPrimitiveGroup* p) { delete p; });
}

void LostCore::FBasicStaticMesh::Tick(float sec)
{
	for (auto pg : PrimitiveGroups)
	{
		if (pg != nullptr)
		{
			//pg->Tick(sec);
		}
	}
}

void LostCore::FBasicStaticMesh::Draw(float sec, IRenderContext * rc)
{
	for (auto pg : PrimitiveGroups)
	{
		if (pg != nullptr)
		{
			pg->Draw(sec, rc);
		}
	}
}

void LostCore::FBasicStaticMesh::AddPrimitiveGroup(IPrimitiveGroup* pg)
{
	if (pg != nullptr && std::find(PrimitiveGroups.begin(), PrimitiveGroups.end(), pg) == PrimitiveGroups.end())
	{
		PrimitiveGroups.push_back(pg);
	}
}

void LostCore::FBasicStaticMesh::RemovePrimitiveGroup(IPrimitiveGroup* pg)
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

void LostCore::FBasicStaticMesh::ClearPrimitiveGroup(std::function<void(IPrimitiveGroup*)> func)
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