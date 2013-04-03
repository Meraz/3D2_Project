cbuffer cbPerFrame
{
	float4x4 gLightWVP;
};


struct SHADOW_VS_IN
{
	float4 posL : POSITION;
	float3 normal: NORMAL;
	float2 texC : TEXCOORD;
};
SamplerState gShadowSam
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};
struct SHADOW_VS_OUT
{
	float4 posH : SV_POSITION;
};
SamplerComparisonState cmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = MIRROR;
   AddressV = MIRROR;

   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};
SHADOW_VS_OUT SHADOW_VS(SHADOW_VS_IN vIn)
{
	SHADOW_VS_OUT vOut;
	vOut.posH = mul(vIn.posL,gLightWVP);
	return vOut;
}

void SHADOW_PS(SHADOW_VS_OUT pIn)
{

}
