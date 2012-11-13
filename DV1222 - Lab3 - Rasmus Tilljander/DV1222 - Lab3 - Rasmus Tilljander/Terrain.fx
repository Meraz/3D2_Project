cbuffer PerObject
{
	float4x4 WorldMatrix;
	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
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
    float2 tiledUV      : TEXCOORD0;
    float2 stretchedUV  : TEXCOORD1; 
};


//-----------------------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------------------
PixelShaderIn VS(VertexShaderIn input)
{
	PixelShaderIn output;
	
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

    return C;
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
