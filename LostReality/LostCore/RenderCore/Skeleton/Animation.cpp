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

LostCore::FSkeletonTree::FSkeletonTree(const FPoseTree & skelRoot, const FFloat4x4& parentInvBindPose)
{
	LoadSkeleton(skelRoot, parentInvBindPose);
}

void LostCore::FSkeletonTree::LoadSkeleton(const FPoseTree & skelRoot, const FFloat4x4& parentInvBindPose)
{
	Name = skelRoot.Data.Name;
	Local = skelRoot.Data.Matrix;

	auto invLocal = Local;
	InvBindPose = parentInvBindPose * invLocal.Invert();

	CurrAnimName = "";
	CurrKeyTime = 0.0f;

	Children.clear();
	for (auto it = skelRoot.Children.begin(); it != skelRoot.Children.end(); ++it)
	{
		Children.push_back(FSkeletonTree(*it, InvBindPose));
	}
}

void LostCore::FSkeletonTree::UpdateWorldMatrix(const FFloat4x4 & parentWorld, float sec)
{
	if (!CurrAnimName.empty())
	{
		CurrKeyTime += sec * FGlobalHandler::Get()->GetAnimateRate();
		FFloat4x4 offset;
		if (FAnimationLibrary::Get()->GetMatrix(offset, CurrKeyTime, CurrAnimName, Name))
		{
			FFloat4x4 invBP(Local);
			BoneWorld = offset * parentWorld;
		}
	}
	else
	{
		BoneWorld = Local * parentWorld;
	}

	World = InvBindPose * BoneWorld;
	for (auto& child : Children)
	{
		child.UpdateWorldMatrix(BoneWorld, sec);
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
	selfData.first = BoneWorld.GetOrigin();
	for (auto& child : Children)
	{
		child.GetSkeletonRenderData(data);
		selfData.second.push_back(child.BoneWorld.GetOrigin());
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
		if (!stream.ReadFromFile(animPath))
		{
			return false;
		}

		string name, ext;
		GetFileName(name, ext, animPath);
		if (ext.compare(K_ANIM_EXT_CURVE) == 0)
		{
			FAnimCurveData anim;
			stream >> anim;
			LoadRecord.insert(animPath);
			AddAnimationCurve(anim);
			animName = anim.Name;
			return true;
		}
		else if (ext.compare(K_ANIM_EXT_KEYFRAME) == 0)
		{
			FAnimKeyFrameData anim;
			stream >> anim;
			LoadRecord.insert(animPath);
			AddAnimationKeyFrame(anim);
			animName = anim.Name;
			return true;
		}
	}
	
	return false;
}

void LostCore::FAnimationLibrary::AddAnimationCurve(const FAnimCurveData & anim)
{
	Curves[anim.Name] = anim;
}

void LostCore::FAnimationLibrary::AddAnimationKeyFrame(const FAnimKeyFrameData & anim)
{
	KeyFrames[anim.Name] = anim;
}

bool LostCore::FAnimationLibrary::GetMatrix(FFloat4x4 & outMatrix,
	float keyTime, const string & animName, const string & skeletonName) const
{
	if (GetMatrixKeyFrame(outMatrix, keyTime, animName, skeletonName))
	{
		return true;
	}
	else if (GetMatrixCurve(outMatrix, keyTime, animName, skeletonName))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool LostCore::FAnimationLibrary::GetMatrixCurve(FFloat4x4 & outMatrix, float keyTime, const string & animName, const string & skeletonName) const
{
	auto it = Curves.find(animName);
	if (it == Curves.end())
	{
		return false;
	}

	auto& animData = (*it).second.CurveMap;
	auto it2 = animData.find(skeletonName);
	if (it2 == animData.end())
	{
		return false;
	}

	auto& curves = (*it2).second;

	FFloat3::FT rx = 0.0;
	if (curves.find(K_ROTATE_X) != curves.end())
	{
		rx = (*curves.find(K_ROTATE_X)).second.Eval(keyTime);
	}

	FFloat3::FT ry = 0.0;
	if (curves.find(K_ROTATE_Y) != curves.end())
	{
		ry = (*curves.find(K_ROTATE_Y)).second.Eval(keyTime);
	}

	FFloat3::FT rz = 0.0;
	if (curves.find(K_ROTATE_Z) != curves.end())
	{
		rz = (*curves.find(K_ROTATE_Z)).second.Eval(keyTime);
	}

	FFloat3::FT tx = 0.0;
	if (curves.find(K_TRANSLATE_X) != curves.end())
	{
		tx = (*curves.find(K_TRANSLATE_X)).second.Eval(keyTime);
	}

	FFloat3::FT ty = 0.0;
	if (curves.find(K_TRANSLATE_Y) != curves.end())
	{
		ty = (*curves.find(K_TRANSLATE_Y)).second.Eval(keyTime);
	}

	FFloat3::FT tz = 0.0;
	if (curves.find(K_TRANSLATE_Z) != curves.end())
	{
		tz = (*curves.find(K_TRANSLATE_Z)).second.Eval(keyTime);
	}

	FFloat3::FT sx = 1.0;
	if (curves.find(K_SCALE_X) != curves.end())
	{
		sx = (*curves.find(K_SCALE_X)).second.Eval(keyTime);
	}

	FFloat3::FT sy = 1.0;
	if (curves.find(K_SCALE_Y) != curves.end())
	{
		sy = (*curves.find(K_SCALE_Y)).second.Eval(keyTime);
	}

	FFloat3::FT sz = 1.0;
	if (curves.find(K_SCALE_Z) != curves.end())
	{
		sz = (*curves.find(K_SCALE_Z)).second.Eval(keyTime);
	}

	outMatrix.SetRotateAndOrigin(FQuat().FromEuler(FFloat3(rx, ry, rz)), FFloat3(tx, ty, tz), FFloat3(sx, sy, sz));
	return true;
}

bool LostCore::FAnimationLibrary::GetMatrixKeyFrame(FFloat4x4 & outMatrix, float keyTime, const string & animName, const string & skeletonName) const
{
	auto it = KeyFrames.find(animName);
	if (it == KeyFrames.end())
	{
		return false;
	}

	auto& animData = (*it).second.KeyFrameMap;
	auto it2 = animData.find(skeletonName);
	if (it2 == animData.end())
	{
		return false;
	}

	auto& curve = (*it2).second;
	outMatrix.SetIdentity();
	outMatrix = curve.Eval(keyTime);
	return true;
}
