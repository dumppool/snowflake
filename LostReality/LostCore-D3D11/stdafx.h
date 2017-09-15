// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
//// file system includes
//#include "File/json.hpp"
//using FJson = nlohmann::json;
//
//
//// interface includes
//#include "RenderContextInterface.h"
//#include "TextureInterface.h"
//#include "MaterialInterface.h"
//#include "PrimitiveGroupInterface.h"
//#include "FontInterface.h"
//
//#include "Inc/LostCore-D3D11.h"
//
//#include "Keywords.h"
//
//// math includes
//#include "Math/Vector2.h"
//#include "Math/Vector3.h"
//#include "Math/Vector4.h"
//#include "Math/Matrix.h"
//#include "Math/Color.h"
//
//#include "File/DirectoryHelper.h"
//
//// vertex detail includes
//#include "VertexTypes.h"

// local includes

#define MODULE_MSG_PREFIX "lostcore-d3d11-log"
#define MODULE_WARN_PREFIX "lostcore-d3d11-warn"
#define MODULE_ERR_PREFIX "lostcore-d3d11-error"
#include "LostCore.h"

#include "Inc/LostCore-D3D11.h"

#include "Src/RenderContextBase.h"
#include "Src/RenderContext.h"
