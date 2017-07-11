/*
* file DirectoryHelper.h
*
* author luoxw
* date 2017/03/01
*
*
*/

#pragma once

namespace LostCore
{
	static void ReplaceChar(std::string& target, const char* replaceFrom, const char* replaceTo)
	{
		auto pos = std::string::npos;
		while ((pos = target.find(replaceFrom)) != std::string::npos)
		{
			target.replace(pos, strlen(replaceFrom), replaceTo);
		}
	}

	/***************************************************************************
	* 所有的操作的输入路径都需要将'/'替换为'\\'
	*/

	// 假定所有文件名都包含扩展名，没有扩展名的视为文件夹名
	static bool IsDirectory(const string& path)
	{
		auto lastSlash = path.rfind('\\');
		auto lastDot = path.rfind('.');
		return (lastDot == string::npos) || (lastDot < lastSlash) || (path.back() == ':');
	}

	// 输出路径以'\\'结束
	static void GetDirectory(string& outPath, const string& path)
	{
		outPath = path;
		auto lastSlash = outPath.rfind('\\');
		if (!IsDirectory(outPath))
		{
			outPath.resize(lastSlash + 1);
		}

		if (outPath.back() != '\\')
		{
			outPath += '\\';
		}
	}

	// 获取不包含路径的文件名及扩展名
	// 如果输入文件名实际上是路径名，输出的扩展名为空
	static void GetFileName(string& outFileName, string& outExtName, const string& path)
	{
		assert(path.size() > 1);

		string tmp(path.begin(), path.end() - (path.back() == '\\' ? 1 : 0));
		if (tmp.back() == ':')
		{
			outFileName = tmp;
			outExtName = "";
			return;
		}

		auto lastSlash = tmp.rfind('\\');
		tmp.assign(tmp.begin() + lastSlash + 1, tmp.end());

		auto lastDot = tmp.rfind('.');
		if (lastDot == string::npos)
		{
			outFileName = tmp;
			outExtName = "";
		}
		else
		{
			outFileName.assign(tmp.begin(), tmp.begin() + lastDot);
			outExtName.assign(tmp.begin() + lastDot + 1, tmp.end());
		}
	}

	static void AutoTest_FilePath()
	{
		string a[] = { "D:/haha/", "D:/haha", "D:", "D:/", "D:/aa.a" };

		for_each(a, a + ARRAYSIZE(a), [](const string& p)
		{
			string fileName, extName, fullPath(p);
			ReplaceChar(fullPath, "/", "\\");
			GetFileName(fileName, extName, fullPath);

			printf("\n%s\t is %sa directory, \t(file & ext) is: [%s], [%s]",
				fullPath.c_str(),
				IsDirectory(fullPath) ? "" : "not ",
				fileName.c_str(), extName.c_str());
		});

		string d0("D:\\GitUnreal\\UnrealEngine\\Engine\\Source\\ThirdParty\\nvtesslib");
		ReplaceChar(d0, "\\", " + ");
		ReplaceChar(d0, " + ", "|");
	}

	/***************************************************************************/

	class FDirectoryHelper
	{
	public:
		static FDirectoryHelper* Get()
		{
			static FDirectoryHelper Inst;
			return &Inst;
		}

	public:
		FDirectoryHelper() 
		{
			const char* head = "FDirectoryHelper";
			char buf[MAX_PATH];
			GetModuleFileNameA(NULL, buf, MAX_PATH);
			RootDirectory = buf;
			RootDirectory.resize(RootDirectory.rfind('\\') + 1);

			// hard coded path file name
			std::string pathFilePath = RootDirectory + "paths.json";

			//RootDirectory.append("paths.json");

			std::ifstream file;
			file.open(pathFilePath.c_str());
			if (file.fail())
			{
				char errstr[128];
				strerror_s(errstr, errno);
				LVERR(head, "open path file(%s) failed: %s", pathFilePath.c_str(), errstr);
			}

			FJson j;
			file >> j;

			for (FJson::iterator it = j.begin(); it != j.end(); ++it)
			{  
				auto pkey = it.key();
				auto pval = it.value();
				if (DirectoryMap.find(it.key()) == DirectoryMap.end())
				{
					DirectoryMap[it.key()] = std::vector<std::string>();
				}

				DirectoryMap[it.key()].push_back(it.value());
				LVMSG(head, "found path, %s: %s", it.key().c_str(), DirectoryMap[it.key()].back().c_str());
			}
		}

		bool GetSpecifiedAbsolutePath(const std::string& specified, const std::string& relativePath, std::string& output)
		{
			auto& dirs = DirectoryMap[specified];
			for (auto& dir : dirs)
			{
				std::string absPath(RootDirectory + dir + relativePath);
				ReplaceChar(absPath, "/", "\\");

				std::ifstream file;
				file.open(absPath);
				if (file.fail())
				{
					continue;
				}

				output = absPath;
				file.close();
				return true;
			}

			return false;
		}

		bool GetShaderAbsolutePath(const std::string& relativePath, std::string& output)
		{
			return GetSpecifiedAbsolutePath("shader", relativePath, output);
		}

		bool GetPrimitiveAbsolutePath(const std::string& relativePath, std::string& output)
		{
			return GetSpecifiedAbsolutePath("primitive", relativePath, output);
		}

		bool GetSpecifiedJson(const std::string& specified, const std::string& relativePath, FJson& output)
		{
			for (auto& dir : DirectoryMap[specified.c_str()])
			{
				std::string absPath(RootDirectory + dir + relativePath);
				ReplaceChar(absPath, "/", "\\");

				std::ifstream file;
				file.open(absPath);
				if (file.fail())
				{
					continue;
				}

				file >> output;
				file.close();
				return true;
			}

			return false;
		}

		bool GetMaterialJson(const std::string& relativePath, FJson& output)
		{
			return GetSpecifiedJson("material", relativePath, output);
		}

		bool GetSceneJson(const std::string& relativePath, FJson& output)
		{
			return GetSpecifiedJson("scene", relativePath, output);
		}

		//bool GetPrimitiveJson(const std::string& relativePath, FJson& output)
		//{
		//	return GetSpecifiedJson("primitive", relativePath, output);
		//}

		bool GetModelJson(const std::string& relativePath, FJson& output)
		{
			return GetSpecifiedJson("model", relativePath, output);
		}

	private:
		std::string RootDirectory;
		std::map<std::string, std::vector<std::string>> DirectoryMap;
	};
}