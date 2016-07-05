// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <assert.h>

// d3d includes
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

// d3dx includes
#include <DirectXMath.h>
#include <DirectXColors.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef int int32;

typedef DirectX::XMFLOAT2 LVVec2;
typedef DirectX::XMFLOAT3 LVVec3;
typedef DirectX::XMFLOAT4 LVVec;
typedef DirectX::XMMATRIX LVMatrix;

#define ALIGNED_LOSTVR(x) __declspec(align(x))
#define VERIFY_HRESULT(result, cmd, info)\
{\
	if (!SUCCEEDED(result))\
	{\
		assert(0 && info);\
		cmd;\
	}\
}

// Vertex data for a colored cube.
ALIGNED_LOSTVR(16) struct MeshVertex
{
	LVVec3 Position;
	LVVec2 Texcoord;
	MeshVertex() = default;
	MeshVertex(LVVec3 p, LVVec2 t) : Position(p), Texcoord(t) { }
};

ALIGNED_LOSTVR(16) struct FrameBufferWVP
{
	LVMatrix modelview;
	LVMatrix texmat;
	LVMatrix Projection;
	FrameBufferWVP() = default;
	FrameBufferWVP(const LVMatrix& modelview, const LVMatrix& texmat, const LVMatrix& proj)
		: modelview(modelview), texmat(texmat), Projection(proj) {}
};

#include <thread>
#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <iostream>

#include <al.h>
#include <alc.h>

#pragma comment(lib, "OpenAL32.lib")

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavresample/avresample.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
}

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avresample.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swscale.lib")

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
