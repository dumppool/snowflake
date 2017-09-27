#ifndef LIGHTING_H
#define LIGHTING_H

float3 DirectionLit(float3 litColor, float3 litDir, float3 surfaceDiffuse, float3 surfaceNormal)
{
	return dot(-litDir, surfaceNormal) * litColor * surfaceDiffuse;
}


#endif