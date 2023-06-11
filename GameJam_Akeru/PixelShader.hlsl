Texture2D		g_Texture : register (t0);
SamplerState	g_Sampler : register (s0);

// 頂点シェーダから受け取る値
struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL0;
};

float4 main(PS_IN input) : SV_TARGET
{
	float4 tex = float4(0.0f, 0.5f, 1.0f, 1.0f);
	tex = g_Texture.Sample(g_Sampler, input.tex);
	return tex;
}