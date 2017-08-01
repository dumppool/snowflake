
cbuffer Constant0 : register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float ScreenWidthRcp;
	float ScreenHeightRcp;
	float4x4 ViewProject;
};

cbuffer Constant1 : register(b1)
{
	float4x4 World;
}

struct VertexIn
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float3 Color : COLOR;
};

VertexOut vs_main(VertexIn Input)
{
	float4 pos = float4(Input.Pos,1.f);
	pos = mul(pos, World);
	pos = mul(pos, ViewProject);

	VertexOut o;
	o.Pos = pos;
	o.Color = Input.Color;

	return o;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	return float4(Input.Color, 1.f);
}