#include "ShadowMap.fx"
static const float SHADOWBIAS = 0.006f;
cbuffer PerObject
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};

struct VertexShaderIn
{
	float4 Position		: POSITION;
	float3 Normal		: NORMAL;
	float2 TexCoord		: TEXCOORD;
};

struct PixelShaderIn
{
	float4 Position		: SV_POSITION;
	float3 normalVS		: NORMAL;
    float2 TexCoord      : TEXCOORD0;
	float4 projTexC		: TEXCOORD1;
	float4 wPos			:TEXCOORD2;
};

Texture2D Texture;

SamplerState TrilinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//-----------------------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------------------
PixelShaderIn VS(VertexShaderIn input)
{
	PixelShaderIn output;

	//float4x4 lWorldMatrix
	output.normalVS = input.Normal;
	output.Position	= mul( input.Position, WorldMatrix );
	output.Position	= mul( output.Position, ViewMatrix );
	output.Position	= mul( output.Position, ProjectionMatrix);
	output.projTexC = mul(input.Position, gLightWVP);
	output.wPos		=  input.Position;
	output.TexCoord = input.TexCoord;

	return output;
}


//-----------------------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------------------
float4 PS(PixelShaderIn input) : SV_Target
{
	float4 a = Texture.Sample(TrilinearSample, input.TexCoord); 
	float4 ambient = a/3;

	input.projTexC.xyz /= input.projTexC.w;
	//Fixa texturkoordinater ifrån clip space coords
	float2 smTex = float2(0.5f*input.projTexC.x, -0.5f*input.projTexC.y) + 0.5f;

	float x,y;
	float sum = 0;

		//nästlad forloop som utför ett pcf filter på shadowmappen i ett 5x5 fält
		for(y = - 2 ; y <= 2.0; y += 1.0f) 
		{
			for(x = - 2 ; x <=2.0; x += 1.0f) 
			{
				sum += gShadowMap.SampleCmpLevelZero(cmpSampler, smTex + texOffset(x,y), input.projTexC.z - SHADOWBIAS);
			}
		}
		//för att det inte ska bli så jävla ljust
		float shadowCoeff = sum/25;
		//float shadowCoeff = gShadowMap.SampleCmpLevelZero(cmpSampler, smTex, input.projTexC.z - SHADOWBIAS);
		float3 L =normalize(Sunpos - input.wPos.xyz);
		float ndotl = dot(normalize(input.normalVS), L);
		//de normaluträknade skuggorna (aka de precis ovanför denna kommentaren) är mest till för att bth logan inte blir snyggt skuggad annars pga att
		//shadowbiasen är lagom inte lätt att få till snyggt på den.
	return ambient + a  * ndotl * shadowCoeff ;
}

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

RasterizerState Wireframe
{
    FillMode = WireFrame;
    CullMode = Back;
    FrontCounterClockwise = false;
};

RasterizerState Solidframe
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
	DepthFunc = LESS_EQUAL;
};

//-----------------------------------------------------------------------------------------
// Techniques
//-----------------------------------------------------------------------------------------
technique10 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
		SetRasterizerState(Solidframe);
		SetDepthStencilState( NoDepthWrites, 0 );
   }
}

technique10 ColorTechWireFrame
{
    pass P1
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        SetRasterizerState(Wireframe);
		SetDepthStencilState( NoDepthWrites, 0 );
    }
}


