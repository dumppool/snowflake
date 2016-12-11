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
    float2 tco[4] : TEXCOORD;
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

	VertexOut Output;
	Output.pos = Vec;

	float4 coord = float4(Input.tco, 0, 1);
	float y = (coord.y / 3.0f) * 2.0f;
	float y2 = (coord.y + 2) / 3.0f;
	Output.tco[0] = float2(coord.x, y);
	Output.tco[1] = float2(coord.x * 0.5, y2);
	Output.tco[2] = float2(coord.x * 0.5 + 0.5, y2);
	Output.tco[3] = Input.tco;

    return Output;
}

float4 ps_main(VertexOut in_data) : SV_TARGET
{
	float y = Texture.Sample(Sampler, in_data.tco[0]).r;
	float u = Texture.Sample(Sampler, in_data.tco[1]).r;
	float v = Texture.Sample(Sampler, in_data.tco[2]).r;
	float4 channels = float4(y, u, v, 1.0);
	float4x4 conversion = float4x4(
		1.000,  0.000,  1.402, -0.701,
		1.000, -0.344, -0.714,  0.529,
		1.000,  1.772,  0.000, -0.886,
		0.000,  0.000,  0.000,  0.000);

	//return u.rrrr;
	//return Texture.Sample(Sampler, in_data.tco[3]);
	//return float4(
	//	dot(channels, float4(1.0, 0.0, 1.402, -0.7)),
	//	dot(channels, float4(1.0, -0.344, -0.714, 0.529)),
	//	dot(channels, float4(1.0, 1.772, 0.0, -0.886)),
	//	1.0);
	//return in_data[1].rgr;
	return mul(conversion, channels);
}

float4 ps_main_cursor(VertexOut in_data) : SV_TARGET
{
	float4 final;
	in_data.tco[0] = 0.5 - abs(0.5 - in_data.tco[0]);
	final = 2 * dot(in_data.tco[0], in_data.tco[0]);
	final *= final;
	final.rgb *= float3(0.2, 0.8, 0.0);
	return final;
}
