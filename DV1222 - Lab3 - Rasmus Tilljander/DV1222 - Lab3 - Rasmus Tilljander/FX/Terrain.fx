#include "ShadowMap.fx"
cbuffer PerObject
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
	float4	 Sunpos;
};

cbuffer cbFixed
{
	float gTexScale = 50;
};

Texture2D gLayer0;	
Texture2D gLayer1;	
Texture2D gLayer2;	
Texture2D gLayer3;
Texture2D gBlendMap;
Texture2D gShadowMap;

cbuffer cbPerObject
{
	float4x4 gWVP; 
	float4x4 gWorld;
};

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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
    float2 tiledUV      : TEXCOORD0;
    float2 stretchedUV  : TEXCOORD1; 
	float4 projTexC		: TEXCOORD2;
};


//-----------------------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------------------
PixelShaderIn VS(VertexShaderIn input)
{
	PixelShaderIn output;
	
	
	output.projTexC = mul(input.Position, gLightWVP);
	output.normalVS = input.Normal;
	
	input.Position	= mul( input.Position, WorldMatrix );
	output.Position	= mul( input.Position, ViewMatrix );
	output.Position	= mul( output.Position, ProjectionMatrix);
	
	
	output.tiledUV = gTexScale*input.TexCoord;
	output.stretchedUV = input.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------------------
float4 PS(PixelShaderIn input) : SV_Target
{
    float4 c0 = gLayer0.Sample( gTriLinearSam, input.tiledUV );
	float4 c1 = gLayer1.Sample( gTriLinearSam, input.tiledUV );
	float4 c2 = gLayer2.Sample( gTriLinearSam, input.tiledUV );
	float4 c3 = gLayer3.Sample( gTriLinearSam, input.tiledUV );


	float4 t = gBlendMap.Sample( gTriLinearSam, input.stretchedUV ); 
    float4 C = c0;
	C = lerp(C, c1, t.r);
    C = lerp(C, c2, t.g);
    C = lerp(C, c3, t.b);
	//shadowmap stuff
	input.projTexC.xyz/=input.projTexC.w;
	
	//transform clip space coords to texture space coords (-1:1 to 0:1)
    input.projTexC.x = input.projTexC.x/2 + 0.5;
	input.projTexC.y = input.projTexC.y/-2 + 0.5;
	
	//om positionen inte syns från ljuset, dvs utanför dess frustrum view ( händer typ bara i kanterna)
	if( input.projTexC.x < -1.0f || input.projTexC.x > 1.0f ||
	    input.projTexC.y < -1.0f || input.projTexC.y > 1.0f ||
	    input.projTexC.z < 0.0f  || input.projTexC.z > 1.0f ) return (C - float4(0.5,0.5,0.5,0));
	
	//shadowmap bias, tar bort alla roliga artefakter som dyker upp annars	
	input.projTexC.z -= 0.2;
	
	float shadowMapDepth = gShadowMap.Sample(gShadowSam, input.projTexC.xy).r;
	//Completly in shadow
	if ( shadowMapDepth < input.projTexC.z) return (float4(1,1,1,1));
	
	//simpel pcf på en texel
	float shadowFactor = gShadowMap.SampleCmpLevelZero( cmpSampler, input.projTexC.xy, input.projTexC.z);
    
	
	//fixa ljussättningen
	float3 L =normalize(Sunpos - input.projTexC.xyz);
	float ndotl = dot(normalize(input.normalVS), L);
	
	return float4(0,0,0,0) + C * ndotl * shadowFactor;
}

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
};

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
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
        SetRasterizerState(Wireframe);
		SetDepthStencilState( NoDepthWrites, 0 );
    }
}
technique10 BuildShadowMapTech
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, SHADOW_VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, SHADOW_PS() ) );
	}
}
