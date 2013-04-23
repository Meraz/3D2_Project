#include "ShadowMap.fx"
#include "Fog.fx"
static const float SHADOWBIAS = 0.002f;

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
	float3 normalVS		: NORMAL;
    float2 tiledUV      : TEXCOORD0;
    float2 stretchedUV  : TEXCOORD1; 
	float4 projTexC		: TEXCOORD2;
	float4 wPos			: TEXCOORD3;
	float fogFactor		: FOG;
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
	output.fogFactor = CalculateFogFactor(mul(mul(input.Position, WorldMatrix) ,ViewMatrix).z);
	
	output.tiledUV = gTexScale*input.TexCoord;
	output.stretchedUV = input.TexCoord;

	return output;
}

//-----------------------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------------------



float4 PS(PixelShaderIn input) : SV_Target
{	
if(input.Position.x > 1024/2-5 && input.Position.x < 1024/2+5 && input.Position.y > 768/2-5 && input.Position.y < 768/2+5)
		return float4(0,1,0,0);
	
    float4 c0 = gLayer0.Sample( gTriLinearSam, input.tiledUV );
	float4 c1 = gLayer1.Sample( gTriLinearSam, input.tiledUV );
	float4 c2 = gLayer2.Sample( gTriLinearSam, input.tiledUV );
	float4 c3 = gLayer3.Sample( gTriLinearSam, input.tiledUV );

	float4 t = gBlendMap.Sample( gTriLinearSam, input.stretchedUV ); 
    float4 C = c0;
	C = lerp(C, c1, t.r);
   C = lerp(C, c2, t.g);
   C = lerp(C, c3, t.b);
   //Om marken är helt i skugga får den ambient ljus.
	float4 ambient = C/3;
	//shadowmap stuff

	//skala textur coordinaterna mellan 0 till 1
	input.projTexC.xyz /= input.projTexC.w;

	//Fixa texturkoordinater ifrån clip space coords
	float2 smTex = float2(0.5f*input.projTexC.x, -0.5f*input.projTexC.y) + 0.5f;
	

	//om positionen inte syns från ljuset, dvs utanför dess frustrum view ( händer typ bara i kanterna)
	if( input.projTexC.x < -1.0f || input.projTexC.x > 1.0f ||
	    input.projTexC.y < -1.0f || input.projTexC.y > 1.0f ||
	    input.projTexC.z < 0.0f  || input.projTexC.z > 1.0f ) return ApplyFog(ambient, input.fogFactor);

		
		
		float x,y;
		float sum = 0;

		//nästlad forloop som utför ett pcf filter på shadowmappen (5x5)
		for(y = - 2 ; y <= 2.0; y += 1.0f) 
		{
			for(x = - 2 ; x <=2.0; x += 1.0f) 
			{
				sum += gShadowMap.SampleCmpLevelZero(cmpSampler, smTex + texOffset(x,y), input.projTexC.z - SHADOWBIAS);
			}
		}
		//för att det inte ska bli så jefla ljust
		float shadowCoeff = sum/25;

	
    
	
	//fixa ljussättningen lite snyggare genom att låta normalerna skapa skuggor, gör så att det kan se lite buggat ut vid kanterna mellan den och shadowmappen
	float3 L =normalize(Sunpos - input.wPos.xyz);
	float ndotl = dot(normalize(input.normalVS), L);
	//returnera 
	ndotl *= 0.8f;
	//if(shadowCoeff > 4.0f)
	return ambient + ApplyFog(C, input.fogFactor) *  shadowCoeff ;
//	else
//	return ambient + C *  shadowCoeff * ndotl;
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


// HÄR BÖRJAR SHADOWMAP BILLBOARDEN UTRININGS 
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