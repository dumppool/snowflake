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
	class FBone
	{
		string Name;
		FMatrix Local;
		FMatrix World;
		vector<FBone> Children;

	public:

		FBone(const FJson& j, const FJson& defaultPose) : Local(FMatrix::Identity), World(FMatrix::Identity)
		{
			assert(j.find(K_NAME) != j.end());
			Name = j[K_NAME];

			if (defaultPose.find(Name) != defaultPose.end())
			{
				Local = defaultPose[Name];
			}

			Children.clear();
			if (j.find(K_LAYER) != j.end())
			{
				for (auto it = j[K_LAYER].begin(); it != j[K_LAYER].end(); ++it)
				{
					Children.push_back(FBone(*it, defaultPose));
				}
			}
		}

		void UpdateWorldMatrix(const FMatrix& parentWorld)
		{
			World = parentWorld * Local;
		}

		void GetPose(map<string, FMatrix>& pose)
		{

		}

	};
	class FAnimation
	{
		FBone Root;

	public:
		FAnimation();
		~FAnimation();

		void LoadDefaultPose(const FJson& j);
		void LoadAnimation(const FJson& j);

		void Tick(float sec);

		map<string, FMatrix> GetPose();
	};
}

