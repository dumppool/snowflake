
#include "ConstantBuffers.fx"
#include "Vertices.fx"
#include "Lighting.fx"

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
	output.Pos = mul(float4(input.Pos, 1.0f), mat);
	output.Pos = mul(output.Pos, ViewProject);
#elif VE_HAS_POS2D
	float2 pos = input.Pos.xy;
	pos *= Size * Scale;
	pos += Origin;

	pos = 2 * pos * float2(ScreenWidthRcp, -ScreenHeightRcp) + float2(-1.0, 1.0);
	output.Pos = float4(pos, 0.0f, 1.0f);
#else
	float4x4 mat = World;
	output.Pos = mul(float4(input.Pos, 1.0f), mat);
	output.Pos = mul(output.Pos, ViewProject);
#endif

#if VE_HAS_NORMAL
	output.Normal = mul(float4(input.Normal, 0.0f), mat);
#endif

#if VE_HAS_TANGENT
	output.Tangent = mul(float4(input.Tangent, 0.0f), mat);
	output.Binormal = mul(float4(input.Binormal, 0.0f), mat);
#endif

#if VE_HAS_COLOR
	output.Color = input.Color;
#endif
	
#if VE_HAS_TEXCOORD0
	output.TexCoord0 = input.TexCoord0;
#endif

#if VE_HAS_TEXCOORD1
	output.TexCoord1 = input.TexCoord1;
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

	float4 col;
#if VE_HAS_TEXCOORD0
	col = ColorTexture.Sample(ColorSampler, input.TexCoord0);
#else
	col = float4(1.0f, 0.0f, 0.0f, 1.0f);
#endif

#if VE_HAS_POS2D
	col.a = 1.0f;
#endif

#if VE_HAS_COLOR
	col = col * input.Color;
#endif

#if VE_HAS_NORMAL
	col.rgb = AmbientColor + DirectionLit(DirectionLitColor, DirectionLitDir, col.rgb, input.Normal);
#endif

	return col;
}