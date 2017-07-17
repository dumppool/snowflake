/*
* file Animation.cpp
*
* author luoxw
* date 2017/05/16
*
*
*/

#include "stdafx.h"
#include "Animation.h"

using namespace LostCore;

LostCore::FSkeletonNode::FSkeletonNode() : Name(""), Local(), World()
{
}

LostCore::FSkeletonNode::FSkeletonNode(const TTreeNode & skelRoot) : Name(""), Local(), World()
{
	LoadSkeleton(skelRoot);
}

void LostCore::FSkeletonNode::LoadSkeleton(const TTreeNode & skelRoot)
{
	Name = skelRoot.Name;

	Children.clear();
	for (auto it = skelRoot.Children.begin(); it != skelRoot.Children.end(); ++it)
	{
		Children.push_back(FSkeletonNode(*it));
	}
}

void LostCore::FSkeletonNode::LoadLocalPose(const FPose& pose)
{
	auto it = pose.find(Name);
	if (it != pose.end())
	{
		Local = it->second;
	}

	for (auto& child : Children)
	{
		child.LoadLocalPose(pose);
	}
}

void LostCore::FSkeletonNode::UpdateWorldMatrix(const FMatrix & parentWorld)
{
	World = Local * parentWorld;
	for (auto& child : Children)
	{
		child.UpdateWorldMatrix(World);
	}
}

void LostCore::FSkeletonNode::GetWorldPose(FPose& pose)
{
	pose[Name] = World;
	for (auto& child : Children)
	{
		child.GetWorldPose(pose);
	}
}

LostCore::FAnimation::FAnimation()
{
}

LostCore::FAnimation::~FAnimation()
{
}

void LostCore::FAnimation::LoadAnimation(const FJson & animJson)
{
}

void LostCore::FAnimation::Tick(float sec)
{
}

void LostCore::FAnimation::GetPose(FPose& pose)
{
}
