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

static const float3x3 YUV_TO_RGB_JPEG =
{
	1.000000, 0.000000, 1.402000,
	1.000000,-0.344140,-0.714140,
	1.000000, 1.772000, 0.000000,
};

#define DIRECT_YUV 0

#if DIRECT_YUV
VertexOut vs_main(VertexIn Input)
{
	float4 Vec = float4(Input.pos, 1.0f);

	VertexOut Output;
	Output.pos = Vec;
	Output.tco[0] = Input.tco;

    return Output;
}
#else
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
#endif

float sRGBToLinearChannel(float ColorChannel)
{
	return ColorChannel > 0.04045 ? pow(ColorChannel * (1.0 / 1.055) + 0.0521327, 2.4) : ColorChannel * (1.0 / 12.92);
}

float3 sRGBToLinear(float3 Color)
{
	return float3(sRGBToLinearChannel(Color.r),
		sRGBToLinearChannel(Color.g),
		sRGBToLinearChannel(Color.b));
}

#if DIRECT_YUV
float4 ps_main(VertexOut in_data) : SV_TARGET
{

	//channels.xyz -= float3(0.0625, 0.5, 0.5);
	//RGB = mul(YUV_TO_RGB_JPEG, channels.xyz);

	//RGB = sRGBToLinear(RGB);
	return Texture.Sample(Sampler, in_data.tco[0]);
}
#else
float4 ps_main(VertexOut in_data) : SV_TARGET
{
	float y = Texture.Sample(Sampler, in_data.tco[0]).r;
float u = Texture.Sample(Sampler, in_data.tco[1]).r;
float v = Texture.Sample(Sampler, in_data.tco[2]).r;
float3 RGB;
float4 channels = float4(y, u, v, 1.0);
float4x4 conversion = float4x4(
	1.000,  0.000,  1.402, -0.701,
	1.000, -0.344, -0.714,  0.529,
	1.000,  1.772,  0.000, -0.886,
	0.000,  0.000,  0.000,  0.000);

RGB = mul(conversion, channels).xyz;

//channels.xyz -= float3(0.0625, 0.5, 0.5);
//RGB = mul(YUV_TO_RGB_JPEG, channels.xyz);

//RGB = sRGBToLinear(RGB);
return float4(RGB, 1.0f);
}
#endif

float4 ps_main_cursor(VertexOut in_data) : SV_TARGET
{
	float4 final;
	in_data.tco[0] = 0.5 - abs(0.5 - in_data.tco[0]);
	final = 2 * dot(in_data.tco[0], in_data.tco[0]);
	final *= final;
	final.rgb *= float3(0.2, 0.8, 0.0);
	return final;
}
