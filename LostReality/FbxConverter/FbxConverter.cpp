// FbxConverter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Importer.h"
#include "MeshLoader.h"

int main(int argc, char** argv)
{
	string src("");
	string dst("");
	for (int i = 1; i < argc; ++i)
	{
		string cmd(argv[i]);
		int equalPos = cmd.find("=");
		if (equalPos != string::npos)
		{
			string key(cmd.begin(), cmd.begin() + equalPos);
			string value(cmd.begin() + equalPos + 1, cmd.end());

			if (key.compare("src") == 0)
			{
				src = value;
			}
			else if (key.compare("dst") == 0)
			{
				dst = value;
			}
		}
	}

	if (!src.empty() && !dst.empty())
	{
		//Importer::ImportScene2(src, dst);
		Importer::ImportSceneMeshes(src, dst, true);
	}

    return 0;
}

