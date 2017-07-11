/*
* file Importer.h
*
* author luoxw
* date 2017/03/10
*
*
*/

#pragma once

namespace Importer {

	inline float SNUM(float val)
	{
		return abs(val) < LostCore::SSmallFloat2 ? 0.0f : val;
	}

	inline double SNUM(double val)
	{
		return abs(val) < (float)LostCore::SSmallFloat2 ? 0.0 : val;
	}

	static void WriteRGB(FJson& output, const FbxColor& color)
	{
		if (color.IsValid())
		{
			vector<float> rgb;
			rgb.push_back(color.mRed);
			rgb.push_back(color.mGreen);
			rgb.push_back(color.mBlue);
			output = rgb;
		}
	}

	static void WriteRGBA(FJson& output, const FbxColor& color)
	{
		if (color.IsValid())
		{
			vector<float> rgba;
			rgba.push_back(color.mRed);
			rgba.push_back(color.mGreen);
			rgba.push_back(color.mBlue);
			rgba.push_back(color.mAlpha);
			output = rgba;
		}
	}

	static void WriteFloat2(FJson& output, const FbxDouble2& value)
	{
		vector<float> f2;
		f2.push_back(value[0]);
		f2.push_back(value[1]);
		output = f2;
	}

	static void WriteFloat3_FromUnreal(FJson& output, const FbxDouble3& value)
	{
		vector<float> f3;
		//f3.push_back(SNUM(value[1]));
		//f3.push_back(SNUM(value[2]));
		//f3.push_back(SNUM(value[0]));

		f3.push_back(SNUM(value[0]));
		f3.push_back(-SNUM(value[1]));
		f3.push_back(SNUM(value[2]));
		output = f3;
	}

	static void WriteFloat3(FJson& output, const FbxDouble3& value)
	{
		WriteFloat3_FromUnreal(output, value);
		return;

		vector<float> f3;
		f3.push_back(value[0]);
		f3.push_back(value[1]);
		f3.push_back(value[2]);
		output = f3;
	}

	static void WriteFloat3_FromUnreal(FJson& output, const FbxVector4& value, bool isCoordinate = false)
	{
		vector<float> f3;
		//f3.push_back(isCoordinate ? SNUM(value[1]) * 0.01 : SNUM(value[1]));
		//f3.push_back(isCoordinate ? SNUM(value[2]) * 0.01 : SNUM(value[2]));
		//f3.push_back(isCoordinate ? SNUM(value[0]) * 0.01 : SNUM(value[0]));
		f3.push_back(isCoordinate ? SNUM(value[0]) : SNUM(value[0]));
		f3.push_back(isCoordinate ? -SNUM(value[1]) : -SNUM(value[1]));
		f3.push_back(isCoordinate ? SNUM(value[2]) : SNUM(value[2]));
		output = f3;
	}

	static void WriteFloat3(FJson& output, const FbxVector4& value, bool isCoordinate = false)
	{
		WriteFloat3_FromUnreal(output, value, isCoordinate);
		return;

		vector<float> f3;
		f3.push_back(value[0]);
		f3.push_back(value[1]);
		f3.push_back(value[2]);
		output = f3;
	}

	static void WriteFloat4x4_FromUnreal(FJson& output, const FbxMatrix& matrix)
	{
		FbxVector4 a0 = matrix.GetRow(0);
		FbxVector4 a1 = matrix.GetRow(1);
		FbxVector4 a2 = matrix.GetRow(2);
		FbxVector4 a3 = matrix.GetRow(3);

		vector<float> f4x4;

		//f4x4.push_back(SNUM(a1[1]));
		//f4x4.push_back(SNUM(a1[2]));
		//f4x4.push_back(SNUM(a1[0]));
		//f4x4.push_back(SNUM(a1[3]));

		//f4x4.push_back(SNUM(a2[1]));
		//f4x4.push_back(SNUM(a2[2]));
		//f4x4.push_back(SNUM(a2[0]));
		//f4x4.push_back(SNUM(a2[3]));

		//f4x4.push_back(SNUM(a0[1]));
		//f4x4.push_back(SNUM(a0[2]));
		//f4x4.push_back(SNUM(a0[0]));
		//f4x4.push_back(SNUM(a0[3]));

		//f4x4.push_back(SNUM(a3[1]) * 0.01f);
		//f4x4.push_back(SNUM(a3[2]) * 0.01f);
		//f4x4.push_back(SNUM(a3[0]) * 0.01f);

		f4x4.push_back(SNUM(a1[0]));
		f4x4.push_back(-SNUM(a1[1]));
		f4x4.push_back(SNUM(a1[2]));
		f4x4.push_back(SNUM(a1[3]));

		f4x4.push_back(-SNUM(a2[0]));
		f4x4.push_back(SNUM(a2[1]));
		f4x4.push_back(-SNUM(a2[2]));
		f4x4.push_back(-SNUM(a2[3]));

		f4x4.push_back(SNUM(a0[0]));
		f4x4.push_back(-SNUM(a0[1]));
		f4x4.push_back(SNUM(a0[2]));
		f4x4.push_back(SNUM(a0[3]));

		f4x4.push_back(SNUM(a3[0]));
		f4x4.push_back(-SNUM(a3[1]));
		f4x4.push_back(SNUM(a3[2]));
		f4x4.push_back(SNUM(a3[3]));
		//f4x4.push_back(SNUM(a3[3]));

		//for (int row = 0; row < 4; ++row)
		//{
		//	FbxVector4 vec = matrix.GetRow(row);
		//	if (row < 3)
		//	{
		//		for (int col = 0; col < 4; ++col)
		//		{
		//			f4x4.push_back(abs(vec[col]) < LostCore::SSmallFloat2 ? 0.0f : vec[col]);
		//		}
		//	}
		//	else
		//	{
		//		f4x4.push_back(SNUM(vec[1]) * 0.01);
		//		f4x4.push_back(SNUM(vec[2]) * 0.01);
		//		f4x4.push_back(SNUM(vec[0]) * 0.01);
		//		f4x4.push_back(SNUM(vec[3]));
		//	}
		//}

		output = f4x4;
	}

	static void WriteFloat4x4_FromUnreal2(FJson& output, const FbxMatrix& matrix)
	{
		FbxAMatrix amatrix = *(FbxAMatrix*)&matrix;
		auto t = amatrix.GetT();
		auto s = amatrix.GetS();
		auto q = amatrix.GetQ();
		
		LostCore::FMatrix outMatrix;
		LostCore::FVec3 t2(t[0], -t[1], t[2]);
		LostCore::FVec3 s2(s[0], s[1], s[2]);
		LostCore::FQuat q2(q[0], -q[1], q[2], -q[3]);
		outMatrix.SetRotateAndOrigin(q2, t2, s2);

		output = outMatrix;
	}

	static void WriteFloat4x4(FJson& output, const FbxMatrix& matrix)
	{
		WriteFloat4x4_FromUnreal2(output, matrix);
		return;

		vector<float> f4x4;
		for (int row = 0; row < 4; ++row)
		{
			FbxVector4 vec = matrix.GetRow(row);
			for (int col = 0; col < 4; ++col)
			{
				f4x4.push_back(SNUM(vec[col]));
			}
		}

		output = f4x4;
	}

	static FbxNode* GetRootLink(FbxScene* scene, FbxNode* link)
	{
		FbxNode* root = link;
		auto rootParent = root != nullptr ? root->GetParent() : nullptr;
		while (root != nullptr && rootParent != nullptr && rootParent != scene->GetRootNode())
		{
			auto attr = rootParent->GetNodeAttribute();
			if (attr != nullptr)
			{
				auto attrType = attr->GetAttributeType();
				if (attrType == FbxNodeAttribute::eMesh ||
					attrType == FbxNodeAttribute::eNull ||
					attrType == FbxNodeAttribute::eSkeleton)
				{
					if (attrType == FbxNodeAttribute::eMesh && ((FbxMesh*)attr)->GetDeformerCount(FbxDeformer::eSkin) > 0)
					{
						break;
					}

					root = rootParent;
					rootParent = root->GetParent();
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}

		return root;
	}

	static bool IsBone(FbxNode* link)
	{
		auto attr = link->GetNodeAttribute();
		if (attr == nullptr)
		{
			return false;
		}

		auto attrType = attr->GetAttributeType();
		return attrType == FbxNodeAttribute::eSkeleton || attrType == FbxNodeAttribute::eMesh || attrType == FbxNodeAttribute::eNull;
	}

	static void SortLinkRecursively(FbxNode* link, vector<FbxNode*>& links)
	{
		if (IsBone(link))
		{
			links.push_back(link);
			for (int index = 0; index < link->GetChildCount(); ++index)
			{
				SortLinkRecursively(link->GetChild(index), links);
			}
		}
	}

	static void SortSkeletalLink(FbxScene* scene, FbxMesh* mesh, vector<FbxNode*>& links)
	{
		if (mesh == nullptr)
		{
			return;
		}

		auto deformerCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
		if (deformerCount > 1)
		{
			LVMSG("SortSkeletalLink", "mesh[%s] has %d deformers.", mesh->GetName(), deformerCount);
		}

		vector<FbxNode*> rootLinks;
		for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
		{
			FbxSkin* skin = (FbxSkin*)mesh->GetDeformer(deformerIndex, FbxDeformer::eSkin);
			for (int clusterIndex = 0; skin != nullptr && clusterIndex < skin->GetClusterCount(); ++clusterIndex)
			{
				FbxNode* link = (FbxNode*)skin->GetCluster(clusterIndex)->GetLink();
				if (link != nullptr)
				{
					auto linkName = link->GetName();
					link = GetRootLink(scene, link);
					bool found = false;
					for (auto plink : rootLinks)
					{
						if (plink == link)
						{
							found = true;
							break;
						}
					}

					if (!found)
					{
						rootLinks.push_back(link);
					}
				}
			}
		}

		for (auto link : rootLinks)
		{
			SortLinkRecursively(link, links);
		}
	}

	static FbxAMatrix ComputeMatrixLocalToParent(FbxNode* node, FbxNode* parentNode)
	{
		auto scene = node->GetScene();
		if (scene != nullptr && parentNode != nullptr)
		{
			auto parentWorld = scene->GetAnimationEvaluator()->GetNodeGlobalTransform(parentNode);
			auto parentLocal = scene->GetAnimationEvaluator()->GetNodeLocalTransform(parentNode);
			auto selfWorld = scene->GetAnimationEvaluator()->GetNodeGlobalTransform(node);
			return parentLocal * selfWorld;
		}

		FbxAMatrix selfWorld;
		selfWorld.SetIdentity();
		return selfWorld;
	}

	extern bool DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
	extern bool ImportSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
	extern bool ImportSceneMeshes2(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
}