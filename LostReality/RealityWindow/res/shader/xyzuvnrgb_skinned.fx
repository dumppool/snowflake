
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
	float3 pos : POSITION;
	float2 tc  : TEXCOORD;
	float3 normal : NORMAL;
	float3 col : COLOR;
float4 weight : TEXCOORD1;
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
	float4 pos = float4(Input.pos.xyz, 1.0f);
	pos = mul(pos, World);
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
	float4 final = float4(1.0f, 1.0f, 1.0f, 1.0f);
	return final;
}