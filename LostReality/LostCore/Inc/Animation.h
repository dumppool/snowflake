/*
* file Animation.h
*
* author luoxw
* date 2017/05/16
*
*
*/

#pragma once

namespace LostCore
{

	class FSkelPoseTree
	{
		string Name;
		FFloat4x4 Local;
		FFloat4x4 World;
		std::vector<FSkelPoseTree> Children;

		string CurrAnimName;
		float CurrKeyTime;

	public:
		FSkelPoseTree();
		explicit FSkelPoseTree(const FBone & skelRoot);

		void LoadSkeleton(const FBone& skelRoot);
		void LoadLocalPose(const FPoseMap& pose);

		void UpdateWorldMatrix(const FFloat4x4& parentWorld, float sec);
		void GetWorldPose(FPoseMap& pose);

		void SetAnimation(const string& animName);
	};

	class FAnimationLibrary
	{
		map<string, FAnimData> Data;

	public:

		static FAnimationLibrary* Get()
		{
			static FAnimationLibrary Inst;
			return &Inst;
		}

		FAnimationLibrary();
		~FAnimationLibrary();

		void AddAnimation(const FAnimData& anim);
		bool GetMatrix(FFloat4x4& outMatrix, float keyTime, const string& animName, const string& skeletonName) const;
	};
}

