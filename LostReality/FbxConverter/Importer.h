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

	INLINE float SNUM(float val)
	{
		return abs(val) < LostCore::SSmallFloat2 ? 0.0f : val;
	}

	INLINE float SNUM(double val)
	{
		return (float)abs(val) < LostCore::SSmallFloat2 ? 0.f : (float)val;
	}

	static void WriteRGB(FJson& output, const FbxColor& color)
	{
		if (color.IsValid())
		{
			vector<float> rgb;
			rgb.push_back(SNUM(color.mRed));
			rgb.push_back(SNUM(color.mGreen));
			rgb.push_back(SNUM(color.mBlue));
			output = rgb;
		}
	}

	static void WriteRGBA(FJson& output, const FbxColor& color)
	{
		if (color.IsValid())
		{
			vector<float> rgba;
			rgba.push_back(SNUM(color.mRed));
			rgba.push_back(SNUM(color.mGreen));
			rgba.push_back(SNUM(color.mBlue));
			rgba.push_back(SNUM(color.mAlpha));
			output = rgba;
		}
	}

	static void WriteFloat2(FJson& output, const FbxDouble2& value)
	{
		vector<float> f2;
		f2.push_back(SNUM(value[0]));
		f2.push_back(SNUM(value[1]));
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
		f3.push_back(SNUM(value[0]));
		f3.push_back(SNUM(value[1]));
		f3.push_back(SNUM(value[2]));
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
		f3.push_back(SNUM(value[0]));
		f3.push_back(SNUM(value[1]));
		f3.push_back(SNUM(value[2]));
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
		LostCore::FVec3 t2(SNUM(t[0]), SNUM(-t[1]), SNUM(t[2]));
		LostCore::FVec3 s2(SNUM(s[0]), SNUM(s[1]), SNUM(s[2]));
		LostCore::FQuat q2(SNUM(q[0]), SNUM(-q[1]), SNUM(q[2]), SNUM(-q[3]));
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

	INLINE LostCore::FVec4 ToVector4(const FbxVector4& vec)
	{
		LostCore::FVec4 output;
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		output.Z = SNUM(-vec[2]);
		output.W = SNUM(vec[3]);
		return output;
	}

	INLINE LostCore::FVec3 ToVector3(const FbxVector4& vec)
	{
		LostCore::FVec3 output;
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		output.Z = SNUM(-vec[2]);
		return output;
	}

	INLINE LostCore::FVec2 ToVector2(const FbxVector2& vec)
	{
		LostCore::FVec2 output;
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		return output;
	}

	INLINE LostCore::FQuat ToQuat(const FbxQuaternion& quat)
	{
		LostCore::FQuat output;
		output.X = SNUM(quat[0]);
		output.Y = SNUM(quat[1]);
		output.Z = SNUM(-quat[2]);
		output.W = SNUM(-quat[3]);
		return output;
	}

	INLINE LostCore::FMatrix ToMatrix(const FbxAMatrix& mat)
	{
		LostCore::FMatrix output;
		for (int row = 0; row < 4; ++row)
		{
			auto vec = mat.GetRow(row);
			if (row == 2)
			{
				output.M[row][0] = SNUM(-vec[0]);
				output.M[row][1] = SNUM(-vec[1]);
				output.M[row][2] = SNUM(vec[2]);
				output.M[row][3] = SNUM(-vec[3]);
			}
			else
			{
				output.M[row][0] = SNUM(vec[0]);
				output.M[row][1] = SNUM(vec[1]);
				output.M[row][2] = SNUM(-vec[2]);
				output.M[row][3] = SNUM(vec[3]);
			}
		}

		return output;
	}

	static bool IsOddNegativeScale(const FbxAMatrix& mat)
	{
		auto scale = mat.GetS();
		int count = (scale[0] < 0 ? 1 : 0) + (scale[1] < 0 ? 1 : 0) + (scale[2] < 0 ? 1 : 0);

		return count == 1 || count == 3;
	}

	static FbxNode* GetRootLink(FbxNode* link)
	{
		FbxScene* scene = link != nullptr ? link->GetScene() : nullptr;
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
		if (link == nullptr)
		{
			return false;
		}

		auto attr = link->GetNodeAttribute();
		if (attr == nullptr)
		{
			return false;
		}

		auto attrType = attr->GetAttributeType();
		return attrType == FbxNodeAttribute::eSkeleton || attrType == FbxNodeAttribute::eMesh || attrType == FbxNodeAttribute::eNull;
	}

	// no validation at all
	static FbxAMatrix GetLinkMatrixFromPose(FbxPose* pose, FbxNode* link)
	{
		//if (pose != nullptr && link != nullptr && pose->Find(link) >= 0)
		{
			return *((FbxAMatrix*)&pose->GetMatrix(pose->Find(link)));
		}

		//FbxAMatrix mat;
		//mat.SetIdentity();
		//return mat;
	}

	static void BuildSkeletonTree(FbxNode* link, FSkeletonTreeAlias& data)
	{
		if (IsBone(link))
		{
			for (int i = 0; i < link->GetChildCount(); ++i)
			{
				auto childData = FSkeletonTreeAlias();
				BuildSkeletonTree(link->GetChild(i), childData);
				data.AddChild(childData);
			}

			data.Data = link->GetName();
		}
	}

	static void BuildPoseTree(FbxPose* pose, FbxNode* link, FPoseTreeAlias& data)
	{
		if (pose != nullptr && IsBone(link) && pose->Find(link)>=0)
		{
			for (int i = 0; i < link->GetChildCount(); ++i)
			{
				auto childData = FPoseTreeAlias();
				BuildPoseTree(pose, link->GetChild(i), childData);
				data.AddChild(childData);
			}

			auto mat = GetLinkMatrixFromPose(pose, link);
			auto parentLink = link->GetParent();
			if (parentLink != nullptr && IsBone(parentLink))
			{
				mat = GetLinkMatrixFromPose(pose, parentLink).Inverse() * mat;
			}

			data.Data.Matrix = ToMatrix(mat);
			data.Data.Name = link->GetName();
		}
	}

	static void GetPoseMapFromTree(const FPoseTreeAlias& poseTree, FPoseMapAlias& poseMap)
	{
		poseMap[poseTree.Data.Name] = poseTree.Data.Matrix;
		for (const auto& c : poseTree.Children)
		{
			GetPoseMapFromTree(c, poseMap);
		}
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
					link = GetRootLink(link);
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
		if (node != nullptr)
		{
			auto scene = node->GetScene();
			if (scene != nullptr)
			{
				auto selfWorld = scene->GetAnimationEvaluator()->GetNodeGlobalTransform(node);
				if (parentNode != nullptr)
				{
					auto parentWorld = scene->GetAnimationEvaluator()->GetNodeGlobalTransform(parentNode);
					auto parentLocal = scene->GetAnimationEvaluator()->GetNodeLocalTransform(parentNode);
					return parentWorld.Inverse() * selfWorld;
				}
				else
				{
					return selfWorld;
				}
			}
		}

		FbxAMatrix selfWorld;
		selfWorld.SetIdentity();
		return selfWorld;
	}

	struct FConvertOptions
	{
		enum EImportType
		{
			ImportLeast,
			ImportNormal,
			ImportTangent,
			ImportMost,
		};

		enum ERegenerateType
		{
			NoRegenerate,
			ForceRegenerate,
			RegenerateIfNotFound,
		};

		bool bImportNormal;
		bool bImportTangent;
		bool bImportVertexColor;

		bool bForceRegenerateNormal;
		bool bRegenerateNormalIfNotFound;

		bool bForceRegenerateTangent;
		bool bRegenerateTangentIfNotFound;

		explicit FConvertOptions(EImportType import, ERegenerateType regenerate)
		{
			switch (import)
			{
			case ImportLeast:
				bImportNormal = false;
				bImportTangent = false;
				bImportVertexColor = false;
				break;
			case ImportNormal:
				bImportNormal = true;
				bImportTangent = false;
				bImportVertexColor = false;
				break;
			case ImportTangent:
				bImportNormal = true;
				bImportTangent = true;
				bImportVertexColor = false;
				break;
			case ImportMost:
				bImportNormal = true;
				bImportTangent = true;
				bImportVertexColor = false;
				break;
			default:
				break;
			}

			switch (regenerate)
			{
			case NoRegenerate:
				bForceRegenerateNormal = false;
				bRegenerateNormalIfNotFound = false;
				bForceRegenerateTangent = false;
				bRegenerateTangentIfNotFound = false;
				break;
			case ForceRegenerate:
				bImportNormal = false;
				bForceRegenerateNormal = true;
				bRegenerateNormalIfNotFound = false;
				bImportTangent = false;
				bForceRegenerateTangent = true;
				bRegenerateTangentIfNotFound = false;
				break;
			case RegenerateIfNotFound:
				bImportNormal = true;
				bForceRegenerateNormal = false;
				bRegenerateNormalIfNotFound = true;
				bImportTangent = true;
				bForceRegenerateTangent = false;
				bRegenerateTangentIfNotFound = true;
				break;
			default:
				break;
			}
		}
	};

	static FConvertOptions SOptions(FConvertOptions::ImportMost, FConvertOptions::NoRegenerate);

	extern bool DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
	extern bool ImportSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
	extern bool ImportSceneMeshes2(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
}