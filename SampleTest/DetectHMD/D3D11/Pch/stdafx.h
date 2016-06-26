// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

#include <Windows.h>

#include <assert.h>

// d3d includes
#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// d3dx includes
#include <DirectXMath.h>
#include <DirectXColors.h>

typedef unsigned short uint16;
typedef unsigned int uint32;
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


// TODO:  在此处引用程序需要的其他头文件
