
#include "ConstantBuffers.fx"
#include "Vertices.fx"

VertexOut VsMain(VertexIn input)
{
	VertexOut output;

#if VE_HAS_SKIN
	float4x4 mat = Bones[input.Indices.x] * input.Weights.x;

	if (input.Indices.y >= 0)
	{
		mat += Bones[input.Indices.y] * input.Weights.y;
		if (input.Indices.z >= 0)
		{
			mat += Bones[input.Indices.z] * input.Weights.z;
			if (input.Indices.w >= 0)
			{
				mat += Bones[input.Indices.w] * input.Weights.w;
			}
		}
	}
#else
	float4x4 mat = World;
#endif

	output.Pos = mul(float4(input.Pos, 1.0f), mat);
	output.Pos = mul(output.Pos, ViewProject);

#if VE_HAS_NORMAL
	output.Normal = mul(float4(input.Normal, 0.0f), mat);
#endif

#if VE_HAS_TANGENT
	output.Tangent = mul(float4(input.Tangent, 0.0f), mat);
	output.Binormal = mul(float4(input.Binormal, 0.0f), mat);
#endif

#if VE_HAS_VERTEXCOLOR
	output.Color = input.Color;
#endif

	
#if VE_HAS_TEXCOORD
	for (int i = 0; i < VE_NUM_TEXCOORDS; ++i)
	{
		output.TexCoord[i] = input.TexCoord[i];
	}
#endif

	return output;
}

float4 PsMain(VertexOut input) : SV_TARGET
{
	//const float3 s_ambientCol = float3(0.11f, 0.111f, 0.3f);
	//const float3 s_lightDir = normalize(float3(0.3f, -0.8f, 0.5f));
	//const float3 s_lightCol = float3(1.8f, 0.7f, 0.18f);

	//float3 col = s_ambientCol;

	//float4x4 ts = {
	//	float4(input.Binormal.xyz, 0.0f),
	//	float4(input.Normal.xyz, 0.0f),
	//	float4(input.Tangent.xyz, 0.0f),
	//	float4(0.0f, 0.0f, 0.0f, 1.0f)
	//};

	//float3 n = mul(float3(0.0f, 1.0f, 0.0f), ts);

	float4 col = AmbientColor;
#if VE_HAS_NORMAL
	col = col + DirectionLit(DirectionLitColor, DirectionLitDir, float3(1.0f, 1.0f, 1.0f), input.Normal);
#endif
	return col;
}