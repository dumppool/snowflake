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
			target.replace(pos, 1, replaceTo);
		}
	}

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