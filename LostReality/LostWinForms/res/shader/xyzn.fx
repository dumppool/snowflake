
cbuffer Custom : register(b2)
{
	float4 CustomColor;
};

cbuffer Constant0 : register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float ScreenWidthRcp;
	float ScreenHeightRcp;
	float4x4 ViewProject;
};

cbuffer Constant : register(b1)
{
	float4x4 World;
}

//Texture2D ColorTexture : register(t0);
//sampler ColorSampler : register(s0);

struct VertexIn
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
};

VertexOut vs_main(VertexIn Input)
{
	float4 pos = float4(Input.Pos.xyz, 1.0f);
	float4 normal = float4(Input.Normal.xyz, 0.0f);
	pos = mul(pos, World);
	normal = mul(normal, World);

	pos = mul(pos, ViewProject);

	VertexOut o;
	o.Pos = pos;
	//o.Normal = mul(normalize(Input.Normal), World);
	o.Normal = normal;

	return o;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	const float3 s_ambientCol = float3(0.11f, 0.111f, 0.3f);
	const float3 s_lightDir = normalize(float3(0.3f, -0.8f, 0.5f));
	const float3 s_lightCol = float3(1.8f, 0.7f, 0.18f);

	float3 col = lerp(float3(1.0f,1.0f,1.0f), CustomColor.xyz, CustomColor.w);
	col = col.xyz * (dot(-s_lightDir, Input.Normal) * s_lightCol + s_ambientCol);
	float4 final = float4(col, 1.0f);
	return final;
}