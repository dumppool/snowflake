// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

#include <assert.h>
#include <functional>
#include <string>
#include <vector>
#include <deque>

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

#include <al.h>
#include <alc.h>

extern "C"
{
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libavresample/avresample.h>
	#include <libavutil/opt.h>
	#include <libavutil/imgutils.h>
}

// TODO:  在此处引用程序需要的其他头文件
