#ifndef CONSTANT_BUFFERS_H
#define CONSTANT_BUFFERS_H

//#include "Constants.fx"// macros
#define HAS_FLAGS(flags, srcFlags) ((flags & srcFlags) == flags)

// Constants,²Î¿¼ShaderManager.h
// MAX_BONES
// LIT_MODE
// CUSTOM_BUFFER
// VERTEX_FLAGS
// VERTEX_TEXCOORD0
// VERTEX_NORMAL
// VERTEX_TANGENT
// VERTEX_COLOR
// VERTEX_SKIN
// VERTEX_TEXCOORD1
// VERTEX_COORDINATE2D

// Switches
#define VE_HAS_POS2D HAS_FLAGS(VERTEX_COORDINATE2D, VERTEX_FLAGS)
#define VE_HAS_TEXCOORD0 HAS_FLAGS(VERTEX_TEXCOORD0, VERTEX_FLAGS)
#define VE_HAS_TEXCOORD1 HAS_FLAGS(VERTEX_TEXCOORD1, VERTEX_FLAGS)
#define VE_HAS_NORMAL HAS_FLAGS(VERTEX_NORMAL, VERTEX_FLAGS)
#define VE_HAS_TANGENT HAS_FLAGS(VERTEX_TANGENT, VERTEX_FLAGS)
#define VE_HAS_COLOR HAS_FLAGS(VERTEX_COLOR, VERTEX_FLAGS)
#define VE_HAS_SKIN HAS_FLAGS(VERTEX_SKIN, VERTEX_FLAGS)

cbuffer Constant0 : register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float ScreenWidthRcp;
	float ScreenHeightRcp;
	float4x4 ViewProject;
	float3 AmbientColor;
	float3 DirectionLitColor;
	float3 DirectionLitDir;
	float3 PointLitColor;
	float3 PointLitPosition;
};

#if VE_HAS_SKIN
cbuffer Constant : register(b1)
{
	float4x4 World;
	float4x4 Bones[MAX_BONES];
}
#elif VE_HAS_POS2D
cbuffer Constant : register(b1)
{
	float2 Size;
	float2 Origin;
	float  Scale;
}
#else
cbuffer Constant : register(b1)
{
	float4x4 World;
}
#endif

cbuffer Custom : register(b2)
{
	float4 CustomColor;
};

Texture2D ColorTexture : register(t0);
sampler ColorSampler : register(s0);


#endif //CONSTANT_BUFFERS_H