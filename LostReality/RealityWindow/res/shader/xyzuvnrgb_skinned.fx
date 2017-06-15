
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
	float4x4 Bones[96];
}

//Texture2D ColorTexture : register(t0);
//sampler ColorSampler : register(s0);

struct VertexIn
{
	float3 pos : POSITION;
	float2 tc  : TEXCOORD;
	float3 normal : NORMAL;
	float3 col : COLOR;
	float4 weight : BLENDWEIGHTS;
	float4 indices : BLENDINDICES;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
	float2 tc  : TEXCOORD;
	float3 normal : NORMAL;
	float3 col : COLOR;
};

VertexOut vs_main(VertexIn Input)
{
	int4 sIndices = floor(Input.indices);

	float4 pos = float4(Input.pos.xyz, 1.0f);
	pos = 
		mul(pos, Bones[sIndices.x]) * Input.weight.x +
		mul(pos, Bones[sIndices.y]) * Input.weight.y +
		mul(pos, Bones[sIndices.z]) * Input.weight.z +
		mul(pos, Bones[sIndices.w]) * Input.weight.w;

	//pos = mul(pos, World);
	pos = mul(pos, ViewProject);

	VertexOut o;
	o.pos = pos;
	o.tc = Input.tc;
	o.normal = Input.normal;
	o.col = Input.col;

	return o;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	//float4 final = ColorTexture.Sample(ColorSampler, Input.tc);
	const float3 s_ambientCol = float3(0.01f, 0.01f, 0.2f);
	const float3 s_lightDir = float3(0.3f, -0.8f, 0.5f);
const float3 s_lightCol = float3(0.8f, 0.7f, 0.0f);
float3 col = dot(s_lightDir, Input.normal) * s_lightCol + s_ambientCol;
	float4 final = float4(col, 1.0f);
	return final;
}