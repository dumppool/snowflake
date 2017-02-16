/*
* file BufferDef.h
*
* author luoxw
* date 2017/01/22
*
*
*/

#pragma once

#define ALIGNED_LOSTVR(x) __declspec(align(x))

// Vertex data for a colored cube.
ALIGNED_LOSTVR(16) struct MeshVertex
{
	LostCore::FVec3 Position;
	LostCore::FVec2 Texcoord;
	MeshVertex() = default;
	MeshVertex(LostCore::FVec3 p, LostCore::FVec2 t) : Position(p), Texcoord(t) { }
};

ALIGNED_LOSTVR(16) struct FrameBufferWVP
{
	LVMatrix W;
	LVMatrix V;
	LVMatrix P;
	FrameBufferWVP() = default;
	FrameBufferWVP(const LVMatrix& w, const LVMatrix& v, const LVMatrix& p)
		: W(w), V(v), P(p) {}
};