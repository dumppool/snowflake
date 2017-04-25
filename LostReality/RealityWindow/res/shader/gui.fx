
cbuffer Constant0 : register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float ScreenWidthRcp;
	float ScreenHeightRcp;
}

cbuffer Constant : register(b1)
{
	float2 Origin;
	float  Scale;
}

Texture2D ColorTexture : register(t0);
sampler ColorSampler : register(s0);

struct VertexIn
{
	float4 col : COLOR;
    float2 pos : POSITION;
	float2 tc  : TEXCOORD;
};

struct VertexOut
{
	float4 col : COLOR;
	float4 pos : SV_POSITION;
	float2 tc  : TEXCOORD;
};

VertexOut vs_main(VertexIn Input)
{
	float2 pos = Input.pos.xy;
	pos *= Scale;
	pos += Origin;

	pos = 2 * pos * float2(ScreenWidthRcp, -ScreenHeightRcp) + float2(-1.0, 1.0);

	VertexOut Output;
	Output.col = Input.col;
	Output.pos = float4(pos, 0.1f, 1.f);
	Output.tc = Input.tc;
    return Output;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	float4 col = ColorTexture.Sample(ColorSampler, Input.tc);
	return Input.col * col;
}