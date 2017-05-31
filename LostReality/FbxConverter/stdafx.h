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

#define MODULE_MSG_PREFIX "converter-log"
#define MODULE_WARN_PREFIX "converter-warn"
#define MODULE_ERR_PREFIX "converter-error"
#include "UtilitiesHeader.h"

#include "VertexTypes.h"

#include "File/json.hpp"
using FJson = nlohmann::json;

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quat.h"
#include "Math/Matrix.h"
#include "Math/Transform.h"

#include "Serialize/Serialization.h"
#include "Serialize/StructSerialize.h"

// sdk includes
#include <fbxsdk.h>

#include "FbxSamples/Common/Common.h"
