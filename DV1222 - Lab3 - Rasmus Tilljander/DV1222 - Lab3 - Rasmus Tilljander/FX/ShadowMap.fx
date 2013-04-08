
static const float ShadowMapSizeX = 1024.0f;
static const float ShadowMapSizeY = 768.0f;
cbuffer cbPerFrame
{
	float4x4 gLightWVP;
	float4	 Sunpos;
};

Texture2D gShadowMap;
struct SHADOW_VS_IN
{
	float4 posL : POSITION;
	//float3 normal: NORMAL;
	//float2 texC : TEXCOORD;
};

SamplerState gShadowSam
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

// SamplerState gShadowSam
// {
	// Filter = MIN_MAG_MIP_POINT;
	// AddressU = Clamp;
	// AddressV = Clamp;
// };

struct SHADOW_VS_OUT
{
	float4 posH : SV_POSITION;
};

SamplerComparisonState cmpSampler
{
   // sampler state
   Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
   AddressU = CLAMP;
   AddressV = CLAMP;

   // sampler comparison state
   ComparisonFunc = LESS_EQUAL;
};
SHADOW_VS_OUT SHADOW_VS(SHADOW_VS_IN vIn)
{
	SHADOW_VS_OUT vOut;
	//vOut.posH = float4(0,0,0,0);
	vOut.posH = mul(vIn.posL,gLightWVP);
	return vOut;
}

void SHADOW_PS(SHADOW_VS_OUT pIn)
{

}
RasterizerState FrontFaceCulling
{
	CullMode = Front;
};
RasterizerState BackFaceCulling
{
	CullMode = Back;
};


float2 texOffset(int x, int y)
{
	//shadowmapsize x och y är atm hårdkodade värden, borde skickas in istället så man kan ändra storlek på shadowmappen
	return float2(x * 1.0f/ShadowMapSizeX, y * 1.0f/ShadowMapSizeY);
}

technique10 BuildShadowMapTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, SHADOW_VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, SHADOW_PS() ) );
		//SetDepthStencilState( NoDepthWrites, 0 );
		SetRasterizerState(BackFaceCulling);
	}
}
