#pragma once

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#include <dxgi.h>
#pragma comment(lib, "dxgi.lib")
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

#define MODULE_MSG_PREFIX "lostcore-d3d11-log"
#define MODULE_WARN_PREFIX "lostcore-d3d11-warn"
#define MODULE_ERR_PREFIX "lostcore-d3d11-error"
#define MODULE_DEBUG_PREFIX "lostcore-d3d11-debug"
#include "LostCoreIncludes.h"

#include "Inc/LostCore-D3D11.h"

#include "Src/RenderContextBase.h"
#include "Implements/RenderContext.h"
