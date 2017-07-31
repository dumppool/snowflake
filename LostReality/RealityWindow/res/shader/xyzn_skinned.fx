
cbuffer Constant0 : register(b0)
{
	float ScreenWidth;
	float ScreenHeight;
	float ScreenWidthRcp;
	float ScreenHeightRcp;
	float4x4 ViewProject;
};

cbuffer Constant : register(b1)
{
	float4x4 World;
	float4x4 Bones[128];
}

//Texture2D ColorTexture : register(t0);
//sampler ColorSampler : register(s0);

struct VertexIn
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float4 Weight : BLENDWEIGHTS;
	int4 Indices : BLENDINDICES;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
};

VertexOut vs_main(VertexIn Input)
{
	int4 sIndices = int4(Input.Indices);

	float4 pos = float4(Input.Pos.xyz, 1.0f);
	if (1)
	{
		float4x4 mat = Bones[sIndices.x] * Input.Weight.x;

		if (sIndices.y >= 0)
		{
			mat += Bones[sIndices.y] * Input.Weight.y;
		}

		if (sIndices.z >= 0)
		{
			mat += Bones[sIndices.z] * Input.Weight.z;
		}

		if (sIndices.w >= 0)
		{
			mat += Bones[sIndices.w] * Input.Weight.w;
		}

		pos = mul(pos, mat);

		//Pos =
		//	sIndices.x >= 0 ? (mul(Pos, Bones[sIndices.x]) * Input.Weight.x) : 0.f +
		//	sIndices.y >= 0 ? (mul(Pos, Bones[sIndices.y]) * Input.Weight.y) : 0.f +
		//	sIndices.z >= 0 ? (mul(Pos, Bones[sIndices.z]) * Input.Weight.z) : 0.f +
		//	sIndices.w >= 0 ? (mul(Pos, Bones[sIndices.w]) * Input.Weight.w) : 0.f;
	}
	else
	{
		pos = mul(pos, World);
	}

	pos = mul(pos, ViewProject);

	VertexOut o;
	o.Pos = pos;
	o.Normal = mul(normalize(Input.Normal), World);
	//o.Normal = Input.Normal;

	return o;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	const float3 s_ambientCol = float3(0.11f, 0.111f, 0.3f);
	const float3 s_lightDir = normalize(float3(0.3f, -0.8f, 0.5f));
	const float3 s_lightCol = float3(1.8f, 0.7f, 0.18f);

	float3 col = dot(-s_lightDir, Input.Normal) * s_lightCol + s_ambientCol;
	float4 final = float4(col, 1.0f);
	return final;
}