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

	CurrAnimName = "";
	CurrKeyTime = 0.0f;

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

void LostCore::FSkelPoseTree::UpdateWorldMatrix(const FFloat4x4 & parentWorld, float sec)
{
	CurrKeyTime += sec;
	bool useDefaultPose = true;
	if (!CurrAnimName.empty())
	{
		FFloat4x4 local;
		if (FAnimationLibrary::Get()->GetMatrix(local, CurrKeyTime, CurrAnimName, Name))
		{
			useDefaultPose = false;
			World = local * parentWorld;
		}
	}

	if (useDefaultPose)
	{
		World = Local * parentWorld;
	}

	for (auto& child : Children)
	{
		child.UpdateWorldMatrix(World, sec);
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

void LostCore::FSkelPoseTree::SetAnimation(const string & animName)
{
	CurrAnimName = animName;
}

LostCore::FAnimationLibrary::FAnimationLibrary()
{
}

LostCore::FAnimationLibrary::~FAnimationLibrary()
{
}

void LostCore::FAnimationLibrary::AddAnimation(const FAnimData & anim)
{
	Data[anim.Name] = anim;
}

bool LostCore::FAnimationLibrary::GetMatrix(FFloat4x4 & outMatrix,
	float keyTime, const string & animName, const string & skeletonName) const
{
	auto it = Data.find(animName);
	if (it == Data.end())
	{
		return false;
	}

	//const auto& animData = Data[animName].CurveMap;
	auto& animData = (*it).second.CurveMap;
	auto it2 = animData.find(skeletonName);
	if (it2 == animData.end())
	{
		return false;
	}

	auto& curves = (*it2).second;
	auto rx = (*curves.find(K_ROTATE_X)).second.GetValue(keyTime);
	auto ry = (*curves.find(K_ROTATE_Y)).second.GetValue(keyTime);
	auto rz = (*curves.find(K_ROTATE_Z)).second.GetValue(keyTime);
	auto tx = (*curves.find(K_TRANSLATE_X)).second.GetValue(keyTime);
	auto ty = (*curves.find(K_TRANSLATE_Y)).second.GetValue(keyTime);
	auto tz = (*curves.find(K_TRANSLATE_Z)).second.GetValue(keyTime);
	auto sx = (*curves.find(K_SCALE_X)).second.GetValue(keyTime);
	auto sy = (*curves.find(K_SCALE_Y)).second.GetValue(keyTime);
	auto sz = (*curves.find(K_SCALE_Z)).second.GetValue(keyTime);
	outMatrix.SetRotateAndOrigin(FQuat().FromEuler(FFloat3(rx, ry, rz)), FFloat3(tx, ty, tz), FFloat3(sx, sy, sz));
}
