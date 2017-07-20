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
		FMatrix Local;
		FMatrix World;
		std::vector<FSkelPoseTree> Children;

	public:
		FSkelPoseTree();
		FSkelPoseTree(const FBone & skelRoot);

		void LoadSkeleton(const FBone& skelRoot);
		void LoadLocalPose(const FPoseMap& pose);

		void UpdateWorldMatrix(const FMatrix& parentWorld);
		void GetWorldPose(FPoseMap& pose);

		//friend FBinaryIO& operator<<(FBinaryIO& stream, const FSkelPoseData& data);
		//friend FBinaryIO& operator>>(FBinaryIO& stream, FSkelPoseData& data);
	};

	//static FBinaryIO& operator<<(FBinaryIO& stream, const FSkelPoseData& data)
	//{
	//	stream << data.Name << data.Local << data.World << data.Children.size();
	//	for (const auto& child : data.Children)
	//	{
	//		stream << child.Data;
	//	}
	//}

	//static FBinaryIO& operator>>(FBinaryIO& stream, FSkelPoseData& data)
	//{
	//	uint32 childNum;
	//	stream >> data.Name >> data.Local >> data.World >> childNum;
	//	for (uint32 i = 0; i < childNum; ++i)
	//	{
	//		data.Children.push_back(FSkelPoseData());
	//		stream >> *(data.Children.end() - 1);
	//	}
	//}

	//class FAnimation
	//{
	//public:
	//	FAnimation();
	//	~FAnimation();

	//	void LoadAnimation(const FJson& animJson);

	//	void Tick(float sec);

	//	void GetPose(FPose& pose);
	//};
}

