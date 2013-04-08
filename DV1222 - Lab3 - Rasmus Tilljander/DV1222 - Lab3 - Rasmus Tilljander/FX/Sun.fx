
//***********************************************
// GLOBALS                                      *
//***********************************************

cbuffer cbPerFrame
{
	float4 CameraPosition;	
	float4 EmitPosition;
	float4 EmitDirection;
	
	float GameTime;
	float DeltaTime;

	float4x4 ViewMatrix;
	float4x4 ProjectionMatrix;
};

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	float3 gAccelW = {0.0f, 7.8f, 0.0f};
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};

// Random texture used to generate random numbers in shaders.
Texture2D Texture;
Texture1D RandomTexture;
 
SamplerState gTriLinearSample
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
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

//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (GameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = RandomTexture.SampleLevel(gTriLinearSample, u, 0);
	
	// project onto unit sphere
	return normalize(v);
}
 
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_EMITTER 0
#define PT_FLARE 1
 
struct Particle
{
	float3 initialPosW : POSITION;
	float3 initialVelW : VELOCITY;
	float2 sizeW       : SIZE;
	float age          : AGE;
	uint type          : TYPE;
};
  
Particle StreamOutVS(Particle vIn)
{
	return vIn;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(2)]
void StreamOutGS(point Particle gIn[1], 
                 inout PointStream<Particle> ptStream)
{	
	gIn[0].age += DeltaTime;
	
	if( gIn[0].type == PT_EMITTER )
	{	
		// time to emit a new particle
		if( gIn[0].age > 0.005f )
		{
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= 0.8f;
			vRandom.z *= 0.8f;
			
			Particle p;
			p.initialPosW = EmitPosition.xyz;
			//p.initialVelW = 70.0f*vRandom;
			p.initialVelW	= 0.0f;
			p.sizeW       = float2(40.0f, 40.0f);
			p.age         = 0.0f;
			p.type        = PT_FLARE;
			
			ptStream.Append(p);
			
			// reset the time to emit
			gIn[0].age = 0.0f;
		}
		
		// always keep emitters
		ptStream.Append(gIn[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( gIn[0].age <= 0.2f )
			ptStream.Append(gIn[0]);
	}		
}

GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_4_0, StreamOutGS() ),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique10 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, StreamOutVS() ) );
        SetGeometryShader( gsStreamOut );
        
        // disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
        // we must also disable the depth buffer for stream-out only
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//***********************************************
// DRAW TECH                                    *
//***********************************************

struct VS_OUT
{
	float4 posW  : POSITION;
	float2 sizeW : SIZE;
	float4 color : COLOR;
	uint   type  : TYPE;
};

VS_OUT DrawVS(Particle vIn)
{
	VS_OUT vOut;
	
	float t = vIn.age;
	
	// constant acceleration equation
	vOut.posW = float4(0.5f*t*t*gAccelW + t*vIn.initialVelW + vIn.initialPosW, 1.0f);
	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t/1.0f);
	vOut.color = float4( 1.0f, 0.5f, 0.5f, opacity);
	
	vOut.sizeW = vIn.sizeW;
	vOut.type  = vIn.type;
	
	return vOut;
}

struct GS_OUT
{
	float4 posH  : SV_Position;
	float4 color : COLOR;
	float2 texC  : TEXCOORD;
};

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void DrawGS(point VS_OUT gIn[1], 
            inout TriangleStream<GS_OUT> triStream)
{	
	// do not draw emitter particles.
	if( gIn[0].type != PT_EMITTER )
	{
		// Compute world matrix so that billboard faces the camera.
		float3 look  = normalize(CameraPosition.xyz - gIn[0].posW);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up    = cross(look, right);
		
		float4x4 W;
		W[0] = float4(right,       0.0f);
		W[1] = float4(up,          0.0f);
		W[2] = float4(look,        0.0f);
		W[3] = float4(gIn[0].posW);

		float4x4 WVP = mul(W, mul(ViewMatrix, ProjectionMatrix));
		
		//
		// Compute 4 triangle strip vertices (quad) in local space.
		// The quad faces down the +z axis in local space.
		//
		float halfWidth  = 0.5f*gIn[0].sizeW.x;
		float halfHeight = 0.5f*gIn[0].sizeW.y;
	
		float4 v[4];
		v[0] = float4(-halfWidth, -halfHeight, 0.0f, 1.0f);
		v[1] = float4(+halfWidth, -halfHeight, 0.0f, 1.0f);
		v[2] = float4(-halfWidth, +halfHeight, 0.0f, 1.0f);
		v[3] = float4(+halfWidth, +halfHeight, 0.0f, 1.0f);
		
		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
		GS_OUT gOut;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			gOut.posH  = mul(v[i], WVP);
			gOut.texC  = gQuadTexC[i];
			gOut.color = gIn[0].color;
			triStream.Append(gOut);
		}	
	}
}

float4 DrawPS(GS_OUT pIn) : SV_TARGET
{
	//return float4(1,1,1,1);
	return Texture.Sample(gTriLinearSample, float3(pIn.texC, 0))*pIn.color;
}

RasterizerState Wireframe
{
        FillMode = WireFrame;
        CullMode = Back;
        FrontCounterClockwise = false;
};

technique10 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_4_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_4_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_4_0, DrawPS() ) );

        SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}