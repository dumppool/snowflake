// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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