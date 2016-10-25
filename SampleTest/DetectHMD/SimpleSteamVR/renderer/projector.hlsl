cbuffer WorldViewProject : register(b0)
{
    float4x4 w;
    float4x4 v;
	float4x4 p;
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

VertexOut vs_main(VertexIn in_v)
{
	VertexOut out_v;

	out_v.pos = float4(in_v.pos, 1.0f);
	if (1)
	{
		out_v.pos = mul(out_v.pos, w);
		out_v.pos = mul(out_v.pos, p);
	}
	else
	{
		float4x4 wvp = w;
		wvp = mul(wvp, p);
		out_v.pos = mul(out_v.pos, wvp);
	}
	
	out_v.pos /= out_v.pos.w;

	out_v.tco = in_v.tco;

    return out_v;
}

float4 ps_main(VertexOut in_data) : SV_TARGET
{
	//return float4(1.5f, 0.8f, 0.280f, 1.0f);
	//return float4(in_data.tco, 0.f, 1.f);
	return Texture.Sample(Sampler, in_data.tco);
}
