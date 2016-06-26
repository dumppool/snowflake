Texture2D Texture : register(t0);
sampler   Sampler : register(s0);

struct Input
{
    float2 tco[3] : TEXCOORD;
	float4 color : COLOR0;
};

float4 main(Input in_data) : SV_TARGET
{
	return in_data.color;
    //return Texture.Sample(Sampler, in_data.tco[0]);
}
