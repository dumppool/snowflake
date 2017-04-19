
cbuffer Constant : register(b1)
{
	float2 Origin;
	float  Scale;
}

struct VertexIn
{
	float4 col : COLOR;
    float2 pos : POSITION;
};

struct VertexOut
{
	float4 col : COLOR;
	float4 pos : SV_POSITION;
};

VertexOut vs_main(VertexIn Input)
{
	float2 pos = Input.pos.xy;
	pos *= Scale;
	pos += Origin;

	VertexOut Output;
	Output.col = Input.col;
	Output.pos = float4(pos, 1.f, 1.f);
    return Output;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	return Input.col;
}