// FbxConverter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

using namespace Importer;

int main(int argc, char** argv)
{
	FConvertOptions options(FConvertOptions::ImportMost, FConvertOptions::NoRegenerate);
	for (int i = 1; i < argc; ++i)
	{
		string cmd(argv[i]);
		int equalPos = cmd.find("=");
		if (equalPos != string::npos)
		{
			string key(cmd.begin(), cmd.begin() + equalPos);
			string value(cmd.begin() + equalPos + 1, cmd.end());

			if (key.compare(K_INPUT_PATH) == 0)
			{
				FConvertOptions::Get()->InputPath = value;
			}
			else if (key.compare(K_OUTPUT_PATH) == 0)
			{
				FConvertOptions::Get()->OutputPath = value;
			}
		}
		else
		{
			if (cmd.compare(K_IMP_TEXCOORD) == 0)
			{
				FConvertOptions::Get()->bImportTexCoord = true;
			}
			else if (cmd.compare(K_IMP_ANIM) == 0)
			{
				FConvertOptions::Get()->bImportAnimation = true;
			}
			else if (cmd.compare(K_MERGE_NORMAL_TANGENT_ALL) == 0)
			{
				FConvertOptions::Get()->bMergeNormalTangentAll = true;
			}
			else if (cmd.compare(K_IMP_NORMAL) == 0)
			{
				FConvertOptions::Get()->bImportNormal = true;
			}
			else if (cmd.compare(K_IMP_TANGENT) == 0)
			{
				FConvertOptions::Get()->bImportTangent = true;
			}
			else if (cmd.compare(K_IMP_VERTEXCOLOR) == 0)
			{
				FConvertOptions::Get()->bImportVertexColor = true;
			}
			else if (cmd.compare(K_FORCE_GEN_NORMAL) == 0)
			{
				FConvertOptions::Get()->bForceRegenerateNormal = true;
			}
			else if (cmd.compare(K_FORCE_GEN_TANGENT) == 0)
			{
				FConvertOptions::Get()->bForceRegenerateTangent = true;
			}
			else if (cmd.compare(K_GEN_NORMAL_IF_NOT_FOUND) == 0)
			{
				FConvertOptions::Get()->bGenerateNormalIfNotFound = true;
			}
			else if (cmd.compare(K_GEN_TANGENT_IF_NOT_FOUND) == 0)
			{
				FConvertOptions::Get()->bGenerateTangentIfNotFound = true;
			}
		}
	}

	if (!FConvertOptions::Get()->InputPath.empty() && !FConvertOptions::Get()->OutputPath.empty())
	{
		Import();
	}
	else
	{
		assert(0);
	}

    return 0;
}

