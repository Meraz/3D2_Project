//Struct used by both Techniques
struct Particle
{
	float3 InitialPosition	: POSITION;
	float3 InitialVelocity	: VELOCITY;
	float2 Size				: SIZE;
	float Age				: AGE;
	uint Type				: TYPE;
};

// Random texture used to generate random numbers in shaders.
Texture1D RandomTexture;

//-----------------------------------------------------------------------------------------
// Cbuffers
//-----------------------------------------------------------------------------------------
cbuffer cbPerFrame
{
	float GameTime;
	float DeltaTime;

	float4 CameraPosition;	
	float4 EmitPosition;		// for when the emit position/direction is varying
	float4 EmitDirection;

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 gAccelW = {0.0f, 9.82f, 0.0f};
};

//-----------------------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------------------
#define PT_EMITTER 0
#define PT_FLARE 1 

SamplerState gTriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

RasterizerState Solidframe
{
	FillMode = Solid;
	CullMode = None;
	FrontCounterClockwise = false;
};
 
DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

RasterizerState Wireframe
{
	FillMode = WireFrame;
	CullMode = Back;
	FrontCounterClockwise = false;
};

//-------------------------------------------------------------------------------------------------------------------
//Random texture functions
//-------------------------------------------------------------------------------------------------------------------
float3 RandomVector3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (GameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = RandomTexture.SampleLevel(gTriLinearSample, u, 0);

	normalize(v);	
	return v;
}


//*****************************************************************************************************
// STREAM-OUT TECH                              
//*****************************************************************************************************
//-----------------------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------------------
Particle StreamOutVS(Particle input)
{
	return input;
}


//-----------------------------------------------------------------------------------------
// GeometryShader ish
//-----------------------------------------------------------------------------------------
[maxvertexcount(6)]
void StreamOutGS(point Particle input[1], inout PointStream<Particle> ptStream)
{	
	input[0].Age += DeltaTime;
	
	if( input[0].Type == PT_EMITTER )
	{	
		// time to emit a new particle?
		if( input[0].Age > 0.01f )
		{
			for(int i = 0; i < 5; ++i)
			{
				// Spread rain drops out above the camera.
				float3 vRandom = 500.0f*RandomVector3((float)i/5.0f);
				vRandom.y = 210.0f;
			
				Particle p;
				p.InitialPosition	= EmitPosition.xyz + vRandom;
				p.InitialVelocity	= float3(0.0f, -200.0f, 0.0f);
				p.Size				= float2(1.0f, 1.0f);
				p.Age				= 0.0f;
				p.Type				= PT_FLARE;
			
				ptStream.Append(p);
			}			
			// reset the time to emit
			input[0].Age = 0.0f;
		}		
		// always keep emitters
		ptStream.Append(input[0]);
	}
	else
	{
		if( input[0].Age <= 9.0f )
			ptStream.Append(input[0]);
	}	
}
GeometryShader GSStreamOut = ConstructGSWithSO( 
	CompileShader( gs_4_0, StreamOutGS() ), 
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique10 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, StreamOutVS() ) );
        SetGeometryShader( GSStreamOut );
        
        // disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
      // SetRasterizerState(Solidframe);

        // we must also disable the depth buffer for stream-out only
        //SetDepthStencilState( NoDepthWrites, 0 );
       
		SetDepthStencilState( DisableDepth, 0 );
    }
}
 
 
//*****************************************************************************************************
// DRAW TECH                              
//*****************************************************************************************************

struct DrawGeometryShaderIn
{
	float3 Position	: POSITION;
	uint   Type		: TYPE;
};

struct DrawPixelShaderIn
{
	float4 Position  : SV_Position;
};

//-----------------------------------------------------------------------------------------
// VertexShader
//-----------------------------------------------------------------------------------------
DrawGeometryShaderIn DrawVS(Particle input)
{
	DrawGeometryShaderIn output;
	
	float t = input.Age;
	// constant acceleration equation
	//output.Position = input.InitialPosition + input.InitialVelocity * t;
	output.Position = 0.5f * t * t * gAccelW + t * input.InitialVelocity + input.InitialPosition;
	output.Type  = input.Type;
	
	return output;
}

//-----------------------------------------------------------------------------------------
// GeometryShader	 //The draw GS just expands points into camera facing quads.
//-----------------------------------------------------------------------------------------
[maxvertexcount(2)]
void DrawGS(point DrawGeometryShaderIn input[1], inout LineStream<DrawPixelShaderIn> lineStream)
{	
	// do not draw emitter particles.
	if( input[0].Type != PT_EMITTER )
	{
		// Slant line in acceleration direction.
		float3 p0 = input[0].Position;
		float3 p1 = input[0].Position + 0.07f * gAccelW;
		
		float4x4 ViewProjectionMatrix = mul(ViewMatrix, ProjectionMatrix);

		DrawPixelShaderIn v0;
		v0.Position = mul(float4(p0, 1.0f), ViewProjectionMatrix);
		lineStream.Append(v0);
		
		DrawPixelShaderIn v1;
		v1.Position = mul(float4(p1, 1.0f), ViewProjectionMatrix);
		lineStream.Append(v1);
	}
}
//-----------------------------------------------------------------------------------------
// PixelShader
//-----------------------------------------------------------------------------------------
float4 DrawPS(DrawPixelShaderIn input) : SV_TARGET
{
	return float4(0.69f,0.76f,0.78f,1);
}

//-----------------------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------------------
technique10 DrawTech
{
    pass P0
    {
        SetVertexShader(	CompileShader( vs_4_0, DrawVS() ) );
        SetGeometryShader(	CompileShader( gs_4_0, DrawGS() ) );
        SetPixelShader(		CompileShader( ps_4_0, DrawPS() ) );
       //SetRasterizerState(Wireframe);

        //SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		//SetRasterizerState(Solidframe);

        SetDepthStencilState( NoDepthWrites, 0 );
    }
}