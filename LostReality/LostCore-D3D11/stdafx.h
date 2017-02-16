// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
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
