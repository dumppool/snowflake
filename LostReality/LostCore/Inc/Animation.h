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
		FFloat4x4 InvBindPose;
		FFloat4x4 World;
		FFloat4x4 BoneWorld;
		std::vector<FSkeletonTree> Children;

		string CurrAnimName;
		float CurrKeyTime;

	public:
		FSkeletonTree();
		FSkeletonTree(const FPoseTree & skelRoot, const FFloat4x4& parentInvBindPose);

		void LoadSkeleton(const FPoseTree& skelRoot, const FFloat4x4& parentInvBindPose);

		void UpdateWorldMatrix(const FFloat4x4& parentWorld, float sec);
		void GetWorldPose(FPoseMap& pose);

		void SetAnimation(const string& animName);

		void GetSkeletonRenderData(map<string, pair<FFloat3, vector<FFloat3>>>& data);
	};

	class FAnimationLibrary
	{
		map<string, FAnimCurveData> Curves;
		map<string, FAnimKeyFrameData> KeyFrames;
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
		void AddAnimationCurve(const FAnimCurveData& anim);
		void AddAnimationKeyFrame(const FAnimKeyFrameData& anim);
		bool GetMatrix(FFloat4x4& outMatrix, float keyTime, const string& animName, const string& skeletonName) const;
		bool GetMatrixCurve(FFloat4x4& outMatrix, float keyTime, const string& animName, const string& skeletonName) const;
		bool GetMatrixKeyFrame(FFloat4x4& outMatrix, float keyTime, const string& animName, const string& skeletonName) const;
	};
}

