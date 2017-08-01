
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

struct VertexIn
{
	float3 Pos : POSITION;
	float3 Color : COLOR;
	float4 Weight : BLENDWEIGHTS;
	int4 Indices : BLENDINDICES;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float3 Color : COLOR;
};

VertexOut vs_main(VertexIn Input)
{
	float4 pos = float4(Input.Pos,1.f);
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
	}
	else
	{
		pos = mul(pos, World);
	}

	pos = mul(pos, ViewProject);

	VertexOut o;
	o.Pos = pos;
	o.Color = Input.Color;

	return o;
}

float4 ps_main(VertexOut Input) : SV_TARGET
{
	return float4(Input.Color, 1.f);
}