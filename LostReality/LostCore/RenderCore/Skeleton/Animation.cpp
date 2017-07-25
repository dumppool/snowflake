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

LostCore::FSkelPoseTree::FSkelPoseTree()
{
}

LostCore::FSkelPoseTree::FSkelPoseTree(const FBone & skelRoot)
{
	LoadSkeleton(skelRoot);
}

void LostCore::FSkelPoseTree::LoadSkeleton(const FBone & skelRoot)
{
	Name = skelRoot.Data;

	Children.clear();
	for (auto it = skelRoot.Children.begin(); it != skelRoot.Children.end(); ++it)
	{
		Children.push_back(FSkelPoseTree(*it));
	}
}

void LostCore::FSkelPoseTree::LoadLocalPose(const FPoseMap& pose)
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

void LostCore::FSkelPoseTree::UpdateWorldMatrix(const FFloat4x4 & parentWorld)
{
	World = Local * parentWorld;
	for (auto& child : Children)
	{
		child.UpdateWorldMatrix(World);
	}
}

void LostCore::FSkelPoseTree::GetWorldPose(FPoseMap& pose)
{
	pose[Name] = World;
	for (auto& child : Children)
	{
		child.GetWorldPose(pose);
	}
}

//LostCore::FAnimation::FAnimation()
//{
//}
//
//LostCore::FAnimation::~FAnimation()
//{
//}
//
//void LostCore::FAnimation::LoadAnimation(const FJson & animJson)
//{
//}
//
//void LostCore::FAnimation::Tick(float sec)
//{
//}
//
//void LostCore::FAnimation::GetPose(FPose& pose)
//{
//}
