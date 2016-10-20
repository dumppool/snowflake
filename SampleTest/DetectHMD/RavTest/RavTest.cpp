// RavTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <assert.h>

using namespace std;

const WCHAR* SConfigFileName	= L"offset.ini";
const CHAR* SModuleVarName		= "dll";
const CHAR* SFunctionVarName	= "func";

vector<string> SFunctionNameArray;
string SModuleName;

bool OpenConfigFile(ofstream& file)
{
	WCHAR buf[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, &buf[0], MAX_PATH-1);

	int idx = MAX_PATH - 1;
	while (--idx >= 0)
	{
		if (buf[idx] == '\\')
		{
			buf[idx+1] = '\0';
			break;
		}
	}

	if (0 != wcscat_s(&buf[0], MAX_PATH-1, SConfigFileName))
	{
		return false;
	}

	ofstream Config("", ios::app);
	file.open(buf, ios::beg|ios::out|ios::in);

	//cout << "opening config: " << &buf[0] << endl;
	printf("opening config: %ls\n", buf);
	return true;
}

void UpdateConfigFile()
{
	ofstream file;
	if (OpenConfigFile(file))
	{
		HMODULE hDll = LoadLibraryA(SModuleName.c_str());
		if (hDll != NULL)
		{
			for (auto func : SFunctionNameArray)
			{
				uintptr_t pf = (uintptr_t)GetProcAddress(hDll, func.c_str());
				file << func.c_str() << " " << pf - (uintptr_t)hDll << endl;
			}

			//cout << file.rdbuf() << endl;
			printf("buf: %s\n", file.rdbuf());
		}

		file.close();
	}
}

void ParseCommandLine(CHAR* arg)
{
	CHAR var[128] = { 0 };
	CHAR val[128] = { 0 };
	int idx = -1;
	while (arg[++idx] != '\0')
	{
		if (arg[idx] == '=')
		{
			if (0 == strcpy_s(var, idx, arg) && 0 == strcpy_s(val, arg + idx + 1))
			{
				if (0 == strcmp(var, SModuleVarName))
				{
					SModuleName = string(val);
				}
				else if (0 == strcmp(var, SFunctionVarName))
				{
					SFunctionNameArray.push_back(string(val));
				}

				break;
			}
			else
			{
				assert(0);
			}
		}
	}
}

void ParseCommandLines(int argc, CHAR* argv[])
{
	for (int i = 0; i < argc; ++i)
	{
		ParseCommandLine(argv[i]);
	}
}

int main(int argc, char* argv[])
{
	ParseCommandLines(argc, argv);
	UpdateConfigFile();

	system("pause");
    return 0;
}

