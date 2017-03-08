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

struct VertexNormalIn
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 tco : TEXCOORD;
};

struct VertexRGBNormalIn
{
	float3 rgb : COLOR;
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 tco : TEXCOORD;
};

struct VertexOut
{
	float4 pos : SV_POSITION;
    float2 tco : TEXCOORD;
};

struct VertexNormalOut
{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 tco : TEXCOORD;
};

struct VertexRGBNormalOut
{
	float4 pos : SV_POSITION;
	float3 rgb : COLOR;
	float3 normal : NORMAL;
	float2 tco : TEXCOORD;
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

VertexRGBNormalOut vs_normal_main(VertexRGBNormalIn Input)
{
	float4 Vec = float4(Input.pos, 1.0f);
	Vec = mul(Vec, World);
	Vec = mul(Vec, ViewProject);
	//Vec = mul(Vec, mul(ViewProject, World));
	VertexRGBNormalOut Output;
	Output.pos = Vec;
	Output.normal = Input.normal;
	Output.tco = Input.tco;
	Output.rgb = Input.rgb;
	return Output;
}

float4 ps_main(VertexOut in_data) : SV_TARGET
{
	//return float4(1.5f, 0.8f, 0.280f, 1.0f);
	//return float4(in_data.tco, 0.f, 1.f);
	//return Texture.Sample(Sampler, in_data.tco);
	return float4(1.f, 1.f, 0.f, 1.f);
}

float4 ps_normal_main(VertexRGBNormalOut in_data) : SV_TARGET
{
	const float3 Ambient = {0.4f, 0.3f, 0.2f};
	const float3 LightColor = {1.f, 1.f, 1.f};
	const float3 LightDir = { -0.3f, -1.f, 0.6f };
	float4 final;
	final.xyz = (Ambient + dot(normalize(LightDir), normalize(in_data.normal))) * LightColor * in_data.rgb;
	//final.xyz = Ambient * in_data.rgb;
	return final;
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
