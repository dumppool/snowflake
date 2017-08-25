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

	class FSkeletonTree
	{
		string Name;
		FFloat4x4 Local;
		FFloat4x4 World;
		std::vector<FSkeletonTree> Children;

		string CurrAnimName;
		float CurrKeyTime;

	public:
		FSkeletonTree();
		explicit FSkeletonTree(const FBone & skelRoot);

		void LoadSkeleton(const FBone& skelRoot);
		void LoadLocalPose(const FPoseMap& pose);

		void LoadSkeletonAndBindPose(const FPoseTree& pose);

		void UpdateWorldMatrix(const FFloat4x4& parentWorld, float sec);
		void GetWorldPose(FPoseMap& pose);

		void SetAnimation(const string& animName);

		void GetSkeletonRenderData(map<string, pair<FFloat3, vector<FFloat3>>>& data);
	};

	class FAnimationLibrary
	{
		map<string, FAnimData> Data;
		set<string> LoadRecord;

	public:

		static FAnimationLibrary* Get()
		{
			static FAnimationLibrary Inst;
			return &Inst;
		}

		FAnimationLibrary();
		~FAnimationLibrary();

		bool Load(const string& path, string& animName);
		void AddAnimation(const FAnimData& anim);
		bool GetMatrix(FFloat4x4& outMatrix, float keyTime, const string& animName, const string& skeletonName) const;
	};
}

