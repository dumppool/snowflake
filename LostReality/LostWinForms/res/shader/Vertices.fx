#ifndef VERTICES_H
#define VERTICES_H

#include "Constants.fx"

struct VertexIn
{
#if VE_HAS_POS2D
	float2 Pos : POSITION;
#else
	float3 Pos : POSITION;
#endif

#if VE_HAS_TEXCOORD0
	float2 TexCoord0 : TEXCOORD0;
#endif

#if VE_HAS_TEXCOORD1
	float2 TexCoord1 : TEXCOORD1;
#endif

#if VE_HAS_NORMAL 
	float3 Normal : NORMAL;
#endif

#if VE_HAS_TANGENT
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
#endif

#if VE_HAS_VERTEXCOLOR
	float3 Color : COLOR;
#endif

#if VE_HAS_SKIN
	float4 Weights : BLENDWEIGHTS;
	int4 Indices : BLENDINDICES;
#endif
};

struct VertexOut
{
	float4 Pos : SV_POSITION;

#if VE_HAS_TEXCOORD
	float2 TexCoord[VE_NUM_TEXCOORDS] : TEXCOORD;
#endif

#if VE_HAS_NORMAL 
	float3 Normal : NORMAL;
#endif

#if VE_HAS_TANGENT
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
#endif

#if VE_HAS_VERTEXCOLOR
	float3 Color : COLOR;
#endif
};


#endif //VERTICES_H