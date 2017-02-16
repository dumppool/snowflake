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

// utility includes
#include "UtilitiesHeader.h"

// math includes
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix.h"

// interface indcludes
#include "RenderContextInterface.h"
#include "TextureInterface.h"
#include "ShaderParameterInterface.h"
#include "ShaderInterface.h"
#include "MaterialInterface.h"
#include "PrimitiveGroupInterface.h"

// local includes
#include "Src/RenderContextBase.h"
