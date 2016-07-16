cbuffer perFrame : register(b0)
{
    float4x4 modelview;
    float4x4 texmat;
	float4x4 projection;
};

struct VertexIn
{
    float3 pos : POSITION;
    float2 tco : TEXCOORD;
};

struct VertexOut
{
    float2 tco[4] : TEXCOORD;
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

    float4x4 mvp = mul(projection, modelview);
    //out_v.pos = mul(mvp, float4(in_v.pos, 1.0f));
	out_v.pos = float4(in_v.pos, 1.0f);

	float2 coord = in_v.tco;
	//float4 coord = mul(texmat, float4(in_v.tco, 0, 1));
	float y = (coord.y / 3.0f) * 2.0f;
	float y2 = (coord.y + 2) / 3.0f;
	out_v.tco[0] = float2(coord.x, y);
	out_v.tco[1] = float2(coord.x * 0.5, y2);
	out_v.tco[2] = float2(coord.x * 0.5 + 0.5, y2);
	out_v.tco[3] = coord.xy;

	//out_v.tco[0] = mul(texmat, float4(in_v.tco, 0, 1));
    return out_v;
}

float4 ps_main(VertexOut in_data) : SV_TARGET
{
	//return float4(0.5f, 0.8f, 0.80f, 1.0f);
	float y = Texture.Sample(Sampler, in_data.tco[0]).r;
	float u = Texture.Sample(Sampler, in_data.tco[1]).r;
	float v = Texture.Sample(Sampler, in_data.tco[2]).r;
	float4x4 conversion = float4x4(
		1.000, 0.000, 1.402, -0.701,
		1.000, -0.344, -0.714, 0.529,
		1.000, 1.772, 0.000, -0.886,
		0.000, 0.000, 0.000, 0.000);
	//return in_data.tco[3].x;
	//return Texture.Sample(Sampler, in_data.tco[3]);
	return mul(conversion, float4(y, u, v, 1.0));
}
