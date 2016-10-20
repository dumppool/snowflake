cbuffer WorldViewProject : register(b0)
{
	float4x4 World;
	float4x4 View;
	float4x4 Project;
};

struct VertexIn
{
    float3 pos : POSITION;
    float2 tco : TEXCOORD;
};

struct VertexOut
{
    float2 tco : TEXCOORD;
    float4 pos : SV_POSITION;
};

struct VertexOut2
{
	float4 pos : SV_POSITION;
};

Texture2D Texture : register(t0);
sampler   Sampler : register(s0);

VertexOut vs_main(VertexIn Input)
{

	float4 Vec = float4(Input.pos, 1.0f);
	float4x4 wvp;
	if (1)
	{
		Vec = mul(Vec, World);
		Vec = mul(Vec, Project);
	}
	else
	{
		wvp = mul(World, Project);
		Vec = mul(Vec, wvp);
	}

	VertexOut Output;
	Output.pos = Vec;
	//Output.pos = float4(Vec.xyz / Vec.w, Vec.w);
	Output.tco = Input.tco;
    return Output;
}

float4 ps_main(VertexOut in_data) : SV_TARGET
{
	//return float4(1.5f, 0.8f, 0.280f, 1.0f);
	//return float4(in_data.tco, 0.f, 1.f);
	return Texture.Sample(Sampler, in_data.tco);
}
