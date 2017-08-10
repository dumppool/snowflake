/*
* file Importer.h
*
* author luoxw
* date 2017/03/10
*
*
*/

#pragma once

#define CONVERT_FROM_RIGHTHAND 1

namespace Importer {

	FORCEINLINE float SNUM(float val)
	{
		return abs(val) < LostCore::SSmallFloat2 ? 0.0f : val;
	}

	FORCEINLINE float SNUM(double val)
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
		
		LostCore::FFloat4x4 outMatrix;
		LostCore::FFloat3 t2(SNUM(t[0]), SNUM(-t[1]), SNUM(t[2]));
		LostCore::FFloat3 s2(SNUM(s[0]), SNUM(s[1]), SNUM(s[2]));
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

	FORCEINLINE LostCore::FFloat4 ToVector4(const FbxVector4& vec)
	{
		LostCore::FFloat4 output;
#if CONVERT_FROM_RIGHTHAND
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		output.Z = SNUM(-vec[2]);
		output.W = SNUM(vec[3]);
#else
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		output.Z = SNUM(vec[2]);
		output.W = SNUM(vec[3]);
#endif
		return output;
	}

	FORCEINLINE LostCore::FFloat3 ToVector3(const FbxVector4& vec)
	{
		LostCore::FFloat3 output;
#if CONVERT_FROM_RIGHTHAND
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		output.Z = SNUM(-vec[2]);
#else
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		output.Z = SNUM(vec[2]);
#endif
		return output;
	}

	FORCEINLINE LostCore::FFloat2 ToVector2(const FbxVector2& vec)
	{
		LostCore::FFloat2 output;
		output.X = SNUM(vec[0]);
		output.Y = SNUM(vec[1]);
		return output;
	}

	FORCEINLINE LostCore::FQuat ToQuat(const FbxQuaternion& quat)
	{
		LostCore::FQuat output;
#if CONVERT_FROM_RIGHTHAND
		output.X = SNUM(quat[0]);
		output.Y = SNUM(quat[1]);
		output.Z = SNUM(-quat[2]);
		output.W = SNUM(-quat[3]);
#else
		output.X = SNUM(quat[0]);
		output.Y = SNUM(quat[1]);
		output.Z = SNUM(quat[2]);
		output.W = SNUM(quat[3]);
#endif
		return output;
	}

	FORCEINLINE LostCore::FFloat4x4 ToMatrix(const FbxAMatrix& mat)
	{
		LostCore::FFloat4x4 output;
		for (int row = 0; row < 4; ++row)
		{
			auto vec = mat.GetRow(row);
#if CONVERT_FROM_RIGHTHAND
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
#else
			output.M[row][0] = SNUM(vec[0]);
			output.M[row][1] = SNUM(vec[1]);
			output.M[row][2] = SNUM(vec[2]);
			output.M[row][3] = SNUM(vec[3]);
#endif

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

	static FbxAMatrix GetGlobalMatrix(FbxNode* link, const FbxTime& time, FbxPose* pose, FbxAMatrix* parentGlobalMatrix = nullptr)
	{
		FbxAMatrix globalMatrix;
		bool found = false;
		if (pose != nullptr && pose->Find(link) > -1)
		{
			auto linkIndex = pose->Find(link);
			if (pose->IsBindPose() || !pose->IsLocalMatrix(linkIndex))
			{
				globalMatrix = *(FbxAMatrix*)&(pose->GetMatrix(linkIndex));
			}
			else
			{
				FbxAMatrix parentMatrix;
				parentMatrix.SetIdentity();
				if (parentGlobalMatrix != nullptr)
				{
					parentMatrix = *parentGlobalMatrix;
				}
				else
				{
					if (link->GetParent() != nullptr)
					{
						parentMatrix = GetGlobalMatrix(link->GetParent(), time, pose);
					}
				}

				FbxAMatrix localMatrix = *(FbxAMatrix*)&(pose->GetMatrix(linkIndex));
				globalMatrix = parentMatrix * localMatrix;
			}

			found = true;
		}

		if (!found)
		{
			globalMatrix = link->EvaluateGlobalTransform(time);
		}

		return globalMatrix;
	}

	// no validation at all
	static FbxAMatrix GetLinkMatrixFromPose(FbxPose* pose, FbxNode* link)
	{
		//if (pose != nullptr && link != nullptr && pose->Find(link) >= 0)
		//return *((FbxAMatrix*)&pose->GetMatrix(pose->Find(link)));

		return GetGlobalMatrix(link, FbxTime(), pose);

		//FbxAMatrix mat;
		//mat.SetIdentity();
		//return mat;
	}

	static void BuildSkeletonTree(FbxNode* link, FSkeletonTreeAlias& output)
	{
		if (IsBone(link))
		{
			for (int i = 0; i < link->GetChildCount(); ++i)
			{
				auto childData = FSkeletonTreeAlias();
				BuildSkeletonTree(link->GetChild(i), childData);
				output.AddChild(childData);
			}

			output.Data = link->GetName();
		}
	}

	static void AMatrixScale(FbxAMatrix& mat, float val)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				mat[i][j] *= val;
			}
		}
	}

	static void AMatrixAddToDiagonal(FbxAMatrix& mat, float val)
	{
		for (int i = 0; i < 4; ++i)
		{
			mat[i][i] += val;
		}
	}

	static void AMatrixAdd(FbxAMatrix& mat, const FbxAMatrix& matR)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				mat[i][j] += matR[i][j];
			}
		}
	}

	static FbxAMatrix GetGeometry(FbxNode* node)
	{
		auto t = node->GetGeometricTranslation(FbxNode::eSourcePivot);
		auto r = node->GetGeometricRotation(FbxNode::eSourcePivot);
		auto s = node->GetGeometricScaling(FbxNode::eSourcePivot);
		return FbxAMatrix(t, r, s);
	}

	static FbxAMatrix GetInitialClusterMatrix(FbxMesh* mesh, FbxCluster* cluster, FPoseMapAlias& pm)
	{
		if (mesh == nullptr || cluster == nullptr)
		{
			return FbxAMatrix();
		}

		auto pn = cluster->GetLink()->GetName();

		FbxAMatrix meshGlobal, meshGeometry, clusterLocal, clusterGlobal;
		cluster->GetTransformMatrix(meshGlobal);
		cluster->GetTransformLinkMatrix(clusterGlobal);
		meshGeometry = GetGeometry(mesh->GetNode());
		clusterLocal = meshGlobal * meshGeometry;
		clusterLocal = clusterGlobal.Inverse() * clusterLocal;

		pm[cluster->GetLink()->GetName()] = ToMatrix(clusterLocal).Normalize3x3();

		//auto scene = mesh->GetScene();
		//FbxAnimStack* currAnimStack = scene->GetSrcObject<FbxAnimStack>(0);
		//FbxTakeInfo* takeInfo = scene->GetTakeInfo(currAnimStack->GetName());
		//auto time = takeInfo->mLocalTimeSpan.GetStart();
		//clusterLocal = GetGlobalMatrix(cluster->GetLink(), time, 
		//	scene->GetPoseCount()>0?scene->GetPose(0):nullptr) * clusterLocal;

		return clusterLocal;
	}

	static void GetGlobalPoseMap(const LostCore::FFloat4x4& parentMat, const FSkeletonTreeAlias& skelNode, FPoseMapAlias& localPose, FPoseMapAlias& globalPose)
	{
		globalPose[skelNode.Data] = localPose[skelNode.Data] * parentMat;
		for (auto& child : skelNode.Children)
		{
			GetGlobalPoseMap(globalPose[skelNode.Data], child, localPose, globalPose);
		}
	}

	static FbxAMatrix GetNodeLocalMatrix(FbxNode* node)
	{
		auto t = node->LclTranslation.Get();
		auto s = node->LclScaling.Get();
		auto r = node->LclRotation.Get();
		return FbxAMatrix(t, r, s);
	}

	static void BuildPoseTree(FbxPose* pose, FbxNode* link, FPoseTreeAlias& output, FPoseTreeAlias& output2)
	{
		if (pose != nullptr && IsBone(link) && pose->Find(link)>=0)
		{
			for (int i = 0; i < link->GetChildCount(); ++i)
			{
				auto childData = (FPoseTreeAlias());
				auto childData2 = (FPoseTreeAlias());
				BuildPoseTree(pose, link->GetChild(i), childData, childData2);
				output.AddChild(childData);
				output2.AddChild(childData2);
			}

			auto mat = GetLinkMatrixFromPose(pose, link);
			output2.Data.Matrix = ToMatrix(mat);
			output2.Data.Name = link->GetName();

			auto parentLink = link->GetParent();
			if (parentLink != nullptr && IsBone(parentLink))
			{
				mat = GetLinkMatrixFromPose(pose, parentLink).Inverse() * mat;
			}

			output.Data.Matrix = ToMatrix(mat);
			output.Data.Name = link->GetName();
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

		bool bImportTexCoord;
		bool bImportAnimation;
		bool bImportVertexColor;

		bool bImportNormal;
		bool bForceRegenerateNormal;
		bool bGenerateNormalIfNotFound;

		bool bImportTangent;
		bool bForceRegenerateTangent;
		bool bGenerateTangentIfNotFound;

		string InputPath;
		string OutputPath;

		explicit FConvertOptions(EImportType import, ERegenerateType regenerate)
		{
			bImportTexCoord = false;
			bImportAnimation = false;
			bImportVertexColor = false;

			bImportNormal = false;
			bForceRegenerateNormal = false;
			bGenerateNormalIfNotFound = false;

			bImportTangent = false;
			bForceRegenerateTangent = false;
			bGenerateTangentIfNotFound = false;
		}

		static FConvertOptions* Get()
		{
			static FConvertOptions Inst(FConvertOptions::ImportMost, FConvertOptions::NoRegenerate);
			return &Inst;
		}
	};

	extern bool DumpSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
	extern bool ImportSceneMeshes(const string& importSrc, const string& convertDst, bool outputBinary, bool exportAnimation);
	extern bool ImportSceneMeshes2();
}