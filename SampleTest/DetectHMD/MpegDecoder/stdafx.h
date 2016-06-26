// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����

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

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
