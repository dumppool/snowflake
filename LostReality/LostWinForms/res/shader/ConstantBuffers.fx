#ifndef CONSTANT_BUFFERS_H
#define CONSTANT_BUFFERS_H

#include "Constants.fx"

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


#endif //CONSTANT_BUFFERS_H