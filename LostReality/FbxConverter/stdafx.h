// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>

#include <Windows.h>

// std includes
#include <string>
#include <iostream>
#include <fstream>
#include <functional>

using namespace std;

// internal includes
#include "Keywords.h"
#include "json.hpp"
using FJson = nlohmann::json;

// sdk includes
#include <fbxsdk.h>

#include "FbxSamples/Common/Common.h"

inline string WideToUTF8(const wstring &wide) {
	if (wide.length() == 0) {
		return string();
	}

	// compute the length of the buffer we'll need
	int charcount = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1,
		NULL, 0, NULL, NULL);
	if (charcount == 0) {
		return string();
	}

	// convert
	char *buf = new char[charcount];
	WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), -1, buf, charcount,
		NULL, NULL);

	string result(buf);
	delete[] buf;
	return result;
}