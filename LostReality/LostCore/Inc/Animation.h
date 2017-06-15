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
	class FSkeletonNode
	{
		string Name;
		vector<FSkeletonNode> Children;

		FMatrix Local;
		FMatrix World;

	public:

		FSkeletonNode();
		FSkeletonNode(const FTreeNode & skelRoot);

		void LoadSkeleton(const FTreeNode& skelRoot);
		void LoadLocalPose(const FPose& pose);

		void UpdateWorldMatrix(const FMatrix& parentWorld);
		void GetWorldPose(FPose& pose);
	};

	class FAnimation
	{
	public:
		FAnimation();
		~FAnimation();

		void LoadAnimation(const FJson& animJson);

		void Tick(float sec);

		void GetPose(FPose& pose);
	};
}

