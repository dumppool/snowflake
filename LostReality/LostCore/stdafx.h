// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

#define MODULE_MSG_PREFIX "lostcore-log"
#define MODULE_WARN_PREFIX "lostcore-warn"
#define MODULE_ERR_PREFIX "lostcore-error"
#include "UtilitiesHeader.h"

#include "LostCore.h"

#include "LostCore-D3D11.h"