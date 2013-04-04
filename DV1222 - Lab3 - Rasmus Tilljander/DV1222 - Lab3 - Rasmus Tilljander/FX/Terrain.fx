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
	float4 wPos			: TEXCOORD3;
};


//-----------------------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------------------
PixelShaderIn VS(VertexShaderIn input)
{
	PixelShaderIn output;
		
	output.normalVS = input.Normal;
	
	output.Position	= mul( input.Position, WorldMatrix );
	output.Position	= mul( output.Position, mul(ViewMatrix, ProjectionMatrix));
	//output.Position	= mul( output.Position, ProjectionMatrix);
	output.wPos		=   input.Position;
	output.projTexC = mul(input.Position, gLightWVP);
	
	
	output.tiledUV = gTexScale*input.TexCoord;
	output.stretchedUV = input.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------------------


float2 texOffset(int x, int y)
{
	//shadowmapsize x och y �r atm h�rdkodade v�rden i shadowmap.fx
	return float2(x * 1.0f/ShadowMapSizeX, y * 1.0f/ShadowMapSizeY);
}
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
   //Om marken �r helt i skugga f�r den ambient ljus.
	float4 ambient = C/3;
	//shadowmap stuff

	//skala textur coordinaterna mellan 0 till 1
	input.projTexC.xyz /= input.projTexC.w;

	//Fixa texturkoordinater ifr�n clip space coords
	float2 smTex = float2(0.5f*input.projTexC.x, -0.5f*input.projTexC.y) + 0.5f;
	

	//om positionen inte syns fr�n ljuset, dvs utanf�r dess frustrum view ( h�nder typ bara i kanterna)
	if( input.projTexC.x < -1.0f || input.projTexC.x > 1.0f ||
	    input.projTexC.y < -1.0f || input.projTexC.y > 1.0f ||
	    input.projTexC.z < 0.0f  || input.projTexC.z > 1.0f ) return ambient;

		
		
		float x,y;
		float sum = 0;

		//n�stlad forloop som utf�r ett pcf filter p� shadowmappen
		for(y = - 2 ; y <= 1.5; y += 1.0f) 
		{
			for(x = - 2 ; x <=1.5; x += 1.0f) 
			{
				sum += gShadowMap.SampleCmpLevelZero(cmpSampler, smTex + texOffset(x,y), input.projTexC.z - SHADOWBIAS);
			}
		}
		//f�r att det inte ska bli s� j�vla ljust
		float shadowCoeff = sum/16;



	//	float s0 = (gShadowMap.Sample(gShadowSam, smTex).r + SHADOWBIAS < input.projTexC.z) ? 0.0f : 1.0f;
	//	float s1 = (gShadowMap.Sample(gShadowSam, smTex + float2(dx, 0.0f)).r + SHADOWBIAS < input.projTexC.z) ? 0.0f : 1.0f;
	//	float s2 = (gShadowMap.Sample(gShadowSam, smTex + float2(0.0f, dx)).r + SHADOWBIAS < input.projTexC.z) ? 0.0f : 1.0f;
	//	float s3 = (gShadowMap.Sample(gShadowSam, smTex + float2(dx, dx)).r + SHADOWBIAS < input.projTexC.z) ? 0.0f : 1.0f;
	
	//float2 texelPos = smTex * SMAP_SIZE;
	//float2 lerps = frac( texelPos );
	//float shadowCoeff = lerp( lerp( s0, s1, lerps.x ),
	//					lerp( s2, s3, lerps.x ),
	//					lerps.y );
	
    
	
	//fixa ljuss�ttningen lite snyggare genom att l�ta normalerna skapa skuggor, g�r s� att det kan se lite buggat ut vid kanterna mellan den och shadowmappen
	float3 L =normalize(Sunpos - input.wPos.xyz);
	float ndotl = dot(normalize(input.normalVS), L);
	//returnera 
	ndotl *= 0.8f;
	if(shadowCoeff > 4.0f)
	return ambient + C *  shadowCoeff ;
	else
	return ambient + C *  shadowCoeff * ndotl;
}

RasterizerState Wireframe
{
    FillMode = WireFrame;
    CullMode = Back;
    FrontCounterClockwise = false;
};
RasterizerState FrontFaceCulling
{
	CullMode = Front;
};
RasterizerState BackFaceCulling
{
	CullMode = Back;
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
		SetDepthStencilState( NoDepthWrites, 0 );
		SetRasterizerState(BackFaceCulling);
	}
}


// H�R B�RJAR SHADOWMAP BILLBOARDEN UTRININGS 
SamplerState bilinearSampler
{
    Filter = min_mag_mip_linear;
    AddressU = MIRROR;
    AddressV = MIRROR;	
};
struct PS_INPUT2
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};
float4 VS2( VertexShaderIn input ) : SV_POSITION
{
	return float4(0,0,0,0);
}
[maxvertexcount(6)]
void GS( point float4 s[1] : SV_POSITION, inout TriangleStream<PS_INPUT2> triStream )
{
	PS_INPUT2 p;
	p.pos = float4(-1,0.50,0,1);
	p.uv = float2(0,1);
	triStream.Append(p);
	
	p.pos = float4(-1,1,0,1);
	p.uv = float2(0,0);
	triStream.Append(p);

	p.pos = float4(-0.50,0.50,0,1);
	p.uv = float2(1,1);
	triStream.Append(p);

	p.pos = float4(-0.50,1,0,1);
	p.uv = float2(1,0);
	triStream.Append(p);
}
float4 PS2( PS_INPUT2 input ) : SV_TARGET
{		
	float r = gShadowMap.Sample(bilinearSampler, input.uv).r;
	return float4(r, r, r, 1);	
}
RasterizerState backFaceCulling
{
	cullmode = back;
};
technique10 RenderBillboard
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS2() ) );
        SetGeometryShader( CompileShader( gs_4_0, GS() ) );
        SetPixelShader( CompileShader( ps_4_0, PS2() ) );	
		SetRasterizerState( Solidframe );		
    }
}