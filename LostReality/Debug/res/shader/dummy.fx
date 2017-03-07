cbuffer World : register(b1)
{
	float4x4 World;
}; 

cbuffer ViewProject : register(b0)
{
	float4x4 ViewProject;
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
		Vec = mul(Vec, ViewProject);
	}
	else
	{
		wvp = mul(World, ViewProject);
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
	//return Texture.Sample(Sampler, in_data.tco);
	return float4(1.f, 1.f, 0.f, 1.f);
}

float4 ps_main_cursor(VertexOut in_data) : SV_TARGET
{
	float4 final;
	in_data.tco = 0.5 - abs(0.5 - in_data.tco);
	final.a = 2 * dot(in_data.tco, in_data.tco);
	final.a *= final.a;
	final.rgb = float3(0.2, 0.8, 0.0);
	return final;
}
