// FbxConverter.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Importer.h"

int main(int argc, char** argv)
{
	string src("");
	string dst("");
	bool exportAnim = false;
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
		else
		{
			if (cmd.compare("anim") == 0)
			{
				exportAnim = true;
			}
		}
	}

	if (!src.empty() && !dst.empty())
	{
		Importer::DumpSceneMeshes(src, dst, true, exportAnim);
		Importer::ImportSceneMeshes2(src, dst, true, exportAnim);
	}

    return 0;
}

