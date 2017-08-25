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

	static string GetCurrentWorkingPath()
	{
		string result;
		result.resize(MAX_PATH);
		GetModuleFileNameA(NULL, &result[0], MAX_PATH);
		GetDirectory(result, result);
		return result;
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

		// 如果输入相对路径的文件夹名，输出一个绝对路径，并总是返回true。
		// 如果输入绝对路径的文件夹名，不做任何处理，返回true。
		// 如果输入相对路径的文件名，找到这个文件输出文件绝对路径名并返回true，没找到返回false。
		// 如果输入绝对路径的文件名，找到这个文件返回true，否则false。
		bool GetSpecifiedAbsolutePath(const std::string& specified, const std::string& path, std::string& output)
		{
			bool isAbsUrl = path.length() > 1 && path[1] == ':';
			if (!isAbsUrl)
			{
				auto& dirs = DirectoryMap[specified];
				for (auto& dir : dirs)
				{
					output = (RootDirectory + dir + path);
					ReplaceChar(output, "/", "\\");

					if (IsDirectory(output))
					{
						return true;
					}
					else
					{
						std::ifstream file;
						file.open(output);
						if (file.fail())
						{
							continue;
						}

						file.close();
						return true;
					}
				}
			}
			else
			{
				if (IsDirectory(path))
				{
					return true;
				}
				else
				{
					std::ifstream file;
					file.open(path);
					if (!file.fail())
					{
						output = path;
						file.close();
						return true;
					}
				}
			}

			return false;
		}

		bool GetShaderAbsolutePath(const std::string& path, std::string& output)
		{
			return GetSpecifiedAbsolutePath("shader", path, output);
		}

		bool GetPrimitiveAbsolutePath(const std::string& path, std::string& output)
		{
			return GetSpecifiedAbsolutePath("primitive", path, output);
		}

		bool GetAnimationAbsolutePath(const string& path, string& output)
		{
			return GetSpecifiedAbsolutePath("animation", path, output);
		}

		bool GetSpecifiedJson(const std::string& specified, const std::string& path, FJson& output)
		{
			bool isAbsUrl = path.length() > 1 && path[1] == ':';
			if (!isAbsUrl)
			{
				for (auto& dir : DirectoryMap[specified.c_str()])
				{
					std::string absPath(RootDirectory + dir + path);
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
			}
			else
			{
				std::ifstream file;
				file.open(path);
				if (!file.fail())
				{
					file >> output;
					file.close();
					return true;
				}
			}

			return false;
		}

		bool GetMaterialJson(const std::string& path, FJson& output)
		{
			return GetSpecifiedJson("material", path, output);
		}

		bool GetSceneJson(const std::string& path, FJson& output)
		{
			return GetSpecifiedJson("scene", path, output);
		}

		//bool GetPrimitiveJson(const std::string& path, FJson& output)
		//{
		//	return GetSpecifiedJson("primitive", path, output);
		//}

		bool GetModelJson(const std::string& path, FJson& output)
		{
			return GetSpecifiedJson("model", path, output);
		}

	private:
		std::string RootDirectory;
		std::map<std::string, std::vector<std::string>> DirectoryMap;
	};
}