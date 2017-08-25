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

LostCore::FSkeletonTree::FSkeletonTree()
{
}

LostCore::FSkeletonTree::FSkeletonTree(const FBone & skelRoot)
{
	LoadSkeleton(skelRoot);
}

void LostCore::FSkeletonTree::LoadSkeleton(const FBone & skelRoot)
{
	Name = skelRoot.Data;

	CurrAnimName = "";
	CurrKeyTime = 0.0f;

	Children.clear();
	for (auto it = skelRoot.Children.begin(); it != skelRoot.Children.end(); ++it)
	{
		Children.push_back(FSkeletonTree(*it));
	}
}

void LostCore::FSkeletonTree::LoadLocalPose(const FPoseMap& pose)
{
	auto it = pose.find(Name);
	if (it != pose.end())
	{
		Local = it->second;
		//Local.Invert();
	}

	for (auto& child : Children)
	{
		child.LoadLocalPose(pose);
	}
}

void LostCore::FSkeletonTree::LoadSkeletonAndBindPose(const FPoseTree & pose)
{
	Name = pose.Data.Name;

	CurrAnimName = "";
	CurrKeyTime = 0.0f;
	Local = pose.Data.Matrix;

	Children.clear();
	for (auto it = pose.Children.begin(); it != pose.Children.end(); ++it)
	{
		Children.push_back(FSkeletonTree());
		Children.back().LoadSkeletonAndBindPose(*it);
	}
}

void LostCore::FSkeletonTree::UpdateWorldMatrix(const FFloat4x4 & parentWorld, float sec)
{
	World = Local * parentWorld;

	if (!CurrAnimName.empty())
	{
		CurrKeyTime += sec;
		FFloat4x4 local;
		if (FAnimationLibrary::Get()->GetMatrix(local, CurrKeyTime, CurrAnimName, Name))
		{
			FFloat4x4 invBP(Local);
			World = local * World;
		}
	}

	for (auto& child : Children)
	{
		child.UpdateWorldMatrix(World, sec);
	}
}

void LostCore::FSkeletonTree::GetWorldPose(FPoseMap& pose)
{
	pose[Name] = World;
	for (auto& child : Children)
	{
		child.GetWorldPose(pose);
	}
}

void LostCore::FSkeletonTree::SetAnimation(const string & animName)
{
	CurrAnimName = animName;

	for (auto& child : Children)
	{
		child.SetAnimation(animName);
	}
}

void LostCore::FSkeletonTree::GetSkeletonRenderData(map<string, pair<FFloat3, vector<FFloat3>>>& data)
{
	data[Name] = pair<FFloat3, vector<FFloat3>>();

	pair<FFloat3, vector<FFloat3>>& selfData = data[Name];
	selfData.first = World.GetOrigin();
	for (auto& child : Children)
	{
		child.GetSkeletonRenderData(data);
		selfData.second.push_back(child.World.GetOrigin());
	}
}

LostCore::FAnimationLibrary::FAnimationLibrary()
{
}

LostCore::FAnimationLibrary::~FAnimationLibrary()
{
}

bool LostCore::FAnimationLibrary::Load(const string & path, string& animName)
{
	if (LoadRecord.find(path) != LoadRecord.end())
	{
		return false;
	}

	string animPath;
	if (FDirectoryHelper::Get()->GetPrimitiveAbsolutePath(path, animPath))
	{
		FBinaryIO stream;
		stream.ReadFromFile(animPath);

		FAnimData anim;
		stream >> anim;

		LoadRecord.insert(animPath);
		AddAnimation(anim);
		animName = anim.Name;
		return true;
	}
	
	return false;
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

	keyTime *= FGlobalHandler::Get()->GetAnimateRate();

	auto& curves = (*it2).second;
	FFloat3::FT rx = 0.0;
	if (curves.find(K_ROTATE_X) != curves.end())
	{
		rx = (*curves.find(K_ROTATE_X)).second.GetValue(keyTime);
	}

	FFloat3::FT ry = 0.0;
	if (curves.find(K_ROTATE_Y) != curves.end())
	{
		ry = (*curves.find(K_ROTATE_Y)).second.GetValue(keyTime);
	}

	FFloat3::FT rz = 0.0;
	if (curves.find(K_ROTATE_Z) != curves.end())
	{
		rz = (*curves.find(K_ROTATE_Z)).second.GetValue(keyTime);
	}

	FFloat3::FT tx = 0.0;
	if (curves.find(K_TRANSLATE_X) != curves.end())
	{
		tx = (*curves.find(K_TRANSLATE_X)).second.GetValue(keyTime);
	}

	FFloat3::FT ty = 0.0;
	if (curves.find(K_TRANSLATE_Y) != curves.end())
	{
		ty = (*curves.find(K_TRANSLATE_Y)).second.GetValue(keyTime);
	}

	FFloat3::FT tz = 0.0;
	if (curves.find(K_TRANSLATE_Z) != curves.end())
	{
		tz = (*curves.find(K_TRANSLATE_Z)).second.GetValue(keyTime);
	}

	FFloat3::FT sx = 1.0;
	if (curves.find(K_SCALE_X) != curves.end())
	{
		sx = (*curves.find(K_SCALE_X)).second.GetValue(keyTime);
	}

	FFloat3::FT sy = 1.0;
	if (curves.find(K_SCALE_Y) != curves.end())
	{
		sy = (*curves.find(K_SCALE_Y)).second.GetValue(keyTime);
	}

	FFloat3::FT sz = 1.0;
	if (curves.find(K_SCALE_Z) != curves.end())
	{
		sz = (*curves.find(K_SCALE_Z)).second.GetValue(keyTime);
	}

	outMatrix.SetRotateAndOrigin(FQuat().FromEuler(FFloat3(rx, ry, rz)), FFloat3(tx, ty, tz), FFloat3(sx, sy, sz));
	return true;
}
