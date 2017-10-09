/*
* file DirectoryHelper.h
*
* author luoxw
* date 2017/03/01
*
*
*/

#pragma once

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

namespace LostCore
{
	static void ReplaceChar(string& target, const char* replaceFrom, const char* replaceTo)
	{
		auto pos = string::npos;
		while ((pos = target.find(replaceFrom)) != string::npos)
		{
			target.replace(pos, strlen(replaceFrom), replaceTo);
		}
	}

	/***************************************************************************
	* 所有的操作的输入路径都需要将'/'替换为'\\'
	*/

	// 假定所有文件名都包含扩展名，没有扩展名的视为文件夹名
	static bool IsDirectory(const string& url)
	{
		auto lastSlash = url.rfind('\\');
		auto lastDot = url.rfind('.');
		return (lastDot == string::npos) || (lastSlash != string::npos && lastDot < lastSlash) || (url.back() == ':');
	}

	// 输出路径以'\\'结束
	static void GetDirectory(string& outPath, const string& url)
	{
		outPath = url;
		auto lastSlash = outPath.rfind('\\');
		if (IsDirectory(outPath))
		{
			if (outPath.back() != '\\')
			{
				outPath += '\\';
			}
		}
		else if (lastSlash != string::npos)
		{
			outPath.resize(lastSlash + 1);
		}
		else
		{
			outPath = "";
		}
	}

	// 获取不包含路径的文件名及扩展名
	// 如果输入文件名实际上是路径名，输出的扩展名为空
	static void GetFileName(string& outFileName, string& outExtName, const string& url)
	{
		assert(url.size() > 1);

		string tmp(url.begin(), url.end() - (url.back() == '\\' ? 1 : 0));
		if (tmp.back() == ':')
		{
			outFileName = tmp;
			outExtName = "";
			return;
		}

		auto lastSlash = tmp.rfind('\\');
		if (lastSlash != string::npos)
		{
			tmp.assign(tmp.begin() + lastSlash + 1, tmp.end());
		}

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

	static string CopyFileTo(const string& dstDirAbs, const string& srcFileAbs, bool overwrite = true)
	{
		string fileName, fileExt;
		GetFileName(fileName, fileExt, srcFileAbs);

		string dstFileAbs(dstDirAbs);
		dstFileAbs = dstFileAbs.append(fileName).append(".").append(fileExt);
		CopyFileA(srcFileAbs.c_str(), dstFileAbs.c_str(), overwrite ? FALSE : TRUE);

		return dstFileAbs;
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

	static void SaveJson(const FJson& json, const string& url)
	{
		if (url.empty())
		{
			return;
		}

		ofstream file;
		file.open(url);
		if (file.fail())
		{
			char errstr[128];
			strerror_s(errstr, errno);
			LVERR("FBinaryIO::WriteToFile", "failed to write[%s]: %s", url.c_str(), errstr);
			return;
		}

		file << json;
		file.close();
	}

	static FJson LoadJson(const string& url)
	{
		if (url.empty())
		{
			return FJson();
		}

		FJson output;
		std::ifstream file;
		file.open(url);
		if (file.fail())
		{
		}
		else
		{
			file >> output;
			file.close();
		}
		
		return output;
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

			// hard coded url file name
			string pathFilePath = RootDirectory + "paths.json";

			//RootDirectory.append("paths.json");

			std::ifstream file;
			file.open(pathFilePath.c_str());
			if (file.fail())
			{
				char errstr[128];
				strerror_s(errstr, errno);
				LVERR(head, "open url file(%s) failed: %s", pathFilePath.c_str(), errstr);
			}

			FJson j;
			file >> j;

			for (FJson::iterator it = j.begin(); it != j.end(); ++it)
			{  
				auto pkey = it.key();
				auto pval = it.value();
				if (DirectoryMap.find(it.key()) == DirectoryMap.end())
				{
					DirectoryMap[it.key()] = std::vector<string>();
				}

				DirectoryMap[it.key()].push_back(it.value());
				LVMSG(head, "found url, %s: %s", it.key().c_str(), DirectoryMap[it.key()].back().c_str());
			}
		}

		// 如果输入相对路径的文件夹名，输出一个绝对路径，并总是返回true。
		// 如果输入绝对路径的文件夹名，返回true。
		// 如果输入相对路径的文件名，找到这个文件输出文件绝对路径名并返回true，没找到返回false。
		// 如果输入绝对路径的文件名，找到这个文件返回true，否则false。
		bool GetSpecifiedAbsolutePath(const string& category, const string& url, string& output)
		{
			bool isAbsUrl = url.length() > 1 && url[1] == ':';
			if (!isAbsUrl)
			{
				auto& dirs = DirectoryMap[category];
				for (auto& dir : dirs)
				{
					output = (RootDirectory + dir + url);

					ReplaceChar(output, "/", "\\");

					if (IsDirectory(output))
					{
						return true;
					}
					else
					{
						if (PathFileExistsA(output.c_str()))
						{
							return true;
						}
					}
				}
			}
			else
			{
				output = url;
				ReplaceChar(output, "/", "\\");
				if (IsDirectory(output))
				{
					return true;
				}
				else if (PathFileExistsA(output.c_str()))
				{
					return true;
				}
			}

			return false;
		}

		bool GetSpecifiedRelativePath(const string& category, const string& url, string& output)
		{
			bool isAbsUrl = url.length() > 1 && url[1] == ':';
			if (!isAbsUrl)
			{
				output = url;
				ReplaceChar(output, "/", "\\");
				if (IsDirectory(output))
				{
					return true;
				}
				else
				{
					auto& dirs = DirectoryMap[category];
					for (auto& dir : dirs)
					{
						string pathAbs = (RootDirectory + dir + output);
						ReplaceChar(pathAbs, "/", "\\");
						if (TRUE == PathFileExistsA(pathAbs.c_str()))
						{
							return true;
						}
					}
				}
			}
			else
			{
				bool isDir = IsDirectory(url);
				if (!isDir && FALSE == PathFileExistsA(url.c_str()))
				{
					return false;
				}

				auto& dirs = DirectoryMap[category];
				for (auto& dir : dirs)
				{
					string specifiedRoot = (RootDirectory + dir);
					ReplaceChar(specifiedRoot, "/", "\\");
					if (url.find(specifiedRoot) == 0)
					{
						output = string(url.begin() + specifiedRoot.size(), url.end());
						return true;
					}
				}
			}

			return false;
		}

		bool GetShaderCodeAbsolutePath(const string& url, string& output)
		{
			return GetSpecifiedAbsolutePath(K_SHADER_CODE, url, output);
		}

		bool GetShaderBlobAbsolutePath(const string& url, string& output)
		{
			return GetSpecifiedAbsolutePath(K_SHADER_BLOB, url, output);
		}

		bool GetPrimitiveAbsolutePath(const string& url, string& output)
		{
			return GetSpecifiedAbsolutePath(K_PRIMITIVE, url, output);
		}

		bool GetPrimitiveRelativePath(const string& url, string& output)
		{
			return GetSpecifiedRelativePath(K_PRIMITIVE, url, output);
		}

		bool GetAnimationAbsolutePath(const string& url, string& output)
		{
			return GetSpecifiedAbsolutePath(K_ANIMATION, url, output);
		}

		bool GetAnimationRelativePath(const string& url, string& output)
		{
			return GetSpecifiedRelativePath(K_ANIMATION, url, output);
		}

		bool GetModelAbsolutePath(const string& url, string& output)
		{
			return GetSpecifiedAbsolutePath(K_MODEL, url, output);
		}

		bool GetModelRelativePath(const string& url, string& output)
		{
			return GetSpecifiedRelativePath(K_MODEL, url, output);
		}

		bool GetSpecifiedJson(const string& category, const string& url, FJson& output)
		{
			string urlAbs;
			if (GetSpecifiedAbsolutePath(category, url, urlAbs))
			{
				output = LoadJson(urlAbs);
				return true;
			}
			else
			{
				return false;
			}
		}

		bool GetMaterialJson(const string& url, FJson& output)
		{
			return GetSpecifiedJson(K_MATERIAL, url, output);
		}

		bool GetSceneJson(const string& url, FJson& output)
		{
			return GetSpecifiedJson(K_SCENE, url, output);
		}

		//bool GetPrimitiveJson(const string& url, FJson& output)
		//{
		//	return GetSpecifiedJson("primitive", url, output);
		//}

		bool GetGizmoJson(const string& url, FJson& output)
		{
			return GetSpecifiedJson(K_GIZMO, url, output);
		}

		bool GetModelJson(const string& url, FJson& output)
		{
			return GetSpecifiedJson(K_MODEL, url, output);
		}
		
	private:
		string RootDirectory;
		std::map<string, std::vector<string>> DirectoryMap;
	};
}