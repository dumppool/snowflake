
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
	float4 normal = float4(Input.Normal.xyz, 0.0f);
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
		normal = mul(normal, mat);
		//normal = mul(normal, World);

		//pos = mul(pos, Bones[sIndices.x] * Input.Weight.x);
		//if (sIndices.y >= 0)
		//{
		//	pos += mul(pos, Bones[sIndices.y] * Input.Weight.y);
		//}

		//if (sIndices.z >= 0)
		//{
		//	pos += mul(pos, Bones[sIndices.z] * Input.Weight.z);
		//}

		//if (sIndices.w >= 0)
		//{
		//	pos += mul(pos, Bones[sIndices.w] * Input.Weight.w);
		//}
	}
	else
	{
		pos = mul(pos, World);
	}

	pos = mul(pos, ViewProject);

	VertexOut o;
	o.Pos = pos;
	//o.Normal = mul(normalize(Input.Normal), World);
	o.Normal = normal;

	return o;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	const float3 s_ambientCol = float3(0.11f, 0.111f, 0.3f);
	const float3 s_lightDir = normalize(float3(0.3f, -0.8f, 0.5f));
	const float3 s_lightCol = float3(1.8f, 0.7f, 0.18f);

	float3 col = s_ambientCol;
	col = dot(-s_lightDir, Input.Normal) * s_lightCol + s_ambientCol;
	float4 final = float4(col, 1.0f);
	return final;
}