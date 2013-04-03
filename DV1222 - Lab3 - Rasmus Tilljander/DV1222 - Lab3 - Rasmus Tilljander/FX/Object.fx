#include "ShadowMap.fx"

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
    float2 TexCoord      : TEXCOORD0;
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

	output.Position	= mul( input.Position, WorldMatrix );
	output.Position	= mul( output.Position, ViewMatrix );
	output.Position	= mul( output.Position, ProjectionMatrix);
	
	output.TexCoord = input.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------------------
float4 PS(PixelShaderIn input) : SV_Target
{
	float4 a = Texture.Sample(TrilinearSample, input.TexCoord); 
	return a;
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
