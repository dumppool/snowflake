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
    float2 tco[3] : TEXCOORD;
    float4 pos : SV_POSITION;
	float4 color : COLOR0;
};

VertexOut vs_main(VertexIn in_v)
{
    VertexOut out_v;

    float4x4 mvp = mul(projection, modelview);
    out_v.pos = mul(mvp, float4(in_v.pos, 1.0f));

	//out_v.tco[0] = mul(texmat, float4(in_v.tco, 0, 1));
	out_v.color = float4(0.5f, 0.5f, 0.0f, 1.0f);
    return out_v;
}
