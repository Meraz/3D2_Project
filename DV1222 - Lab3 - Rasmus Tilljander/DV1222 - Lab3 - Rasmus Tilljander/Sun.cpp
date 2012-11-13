#include "Sun.h"

Sun::Sun() : ParticleSystem()
{
	
} 

Sun::~Sun()
{
	SAFE_RELEASE(mInitVertexBuffer);
	SAFE_RELEASE(mDrawVertexBuffer);
	SAFE_RELEASE(mStreamOutVertexBuffer);
}

void Sun::Initialize(ID3D10Device* lDevice, char* lFilename, UINT maxParticles, UINT lStructSize)
{
	ParticleSystem::Initialize(lDevice, lFilename, maxParticles, lStructSize);
	mEmitPosition = D3DXVECTOR4(0, 512, -513 , 1.0f);
	mShaderObject->SetResource("Texture", GetResourceLoader().GetParticleTexture()); 
}

void Sun::CreateBuffer()
{
	D3D10_BUFFER_DESC PBufferDesc;
    PBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	PBufferDesc.ByteWidth = sizeof(ParticleVertex) * 1;
    PBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    PBufferDesc.CPUAccessFlags = 0;
    PBufferDesc.MiscFlags = 0;
	
	//Create emitter
	ParticleVertex lP;
	ZeroMemory(&lP, sizeof(ParticleVertex));
	lP.mAge = 0.0f;
	lP.mType = 0;	

	D3D10_SUBRESOURCE_DATA pinitData;
    pinitData.pSysMem = &lP;

	mDevice->CreateBuffer(&PBufferDesc, &pinitData, &mInitVertexBuffer);

	//create streamout and draw buffers
	PBufferDesc.ByteWidth = sizeof(ParticleVertex) * mMaxParticles;
    PBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;

	mDevice->CreateBuffer(&PBufferDesc, 0, &mDrawVertexBuffer);
	mDevice->CreateBuffer(&PBufferDesc, 0, &mStreamOutVertexBuffer);
}

void Sun::Update(float ldt, float lGameTime)
{
	mGameTime = lGameTime;
	mDeltaTime = ldt;
	mAge += ldt;
	if(mEmitPosition.z <= 513.0f)
		mEmitPosition.z += 10*ldt;
	else
		mEmitPosition.z = -513.0f;
	mEmitPosition.y = 513*cos((PI/1024)*mEmitPosition.z);
}



void Sun::Draw()
{
	ParticleSystem::Draw();	

	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	UINT stride = sizeof(ParticleVertex);
    UINT offset = 0;

	// On the first pass, use the initialization VB.  Otherwise, use
	// the VB that contains the current particle list.
	if( mFirstRun )
		mDevice->IASetVertexBuffers(0, 1, &mInitVertexBuffer, &stride, &offset);
	else
		mDevice->IASetVertexBuffers(0, 1, &mDrawVertexBuffer, &stride, &offset);

	// Draw the current particle list using stream-out only to update them.  
	// The updated vertices are streamed-out to the target VB. 
	mDevice->SOSetTargets(1, &mStreamOutVertexBuffer, &offset);

	//SteamOut
	mShaderObject->SetTechniqueByInteger(0);
	D3D10_TECHNIQUE_DESC techDesc;
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mShaderObject->Render(p);        

		if( mFirstRun )
		{
			mDevice->Draw(1, 0);
			mFirstRun = false;
		}
		else
		{
			mDevice->DrawAuto();
		}
    }

	// done streaming-out--unbind the vertex buffer
	ID3D10Buffer* bufferArray[1] = {0};
	mDevice->SOSetTargets(1, bufferArray, &offset);

	// ping-pong the vertex buffers
	std::swap(mDrawVertexBuffer, mStreamOutVertexBuffer);

	// Draw the updated particle system we just streamed-out. 
	mDevice->IASetVertexBuffers(0, 1, &mDrawVertexBuffer, &stride, &offset);

	//Draw
	mShaderObject->SetTechniqueByInteger(1);
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mShaderObject->Render(p);
		mDevice->DrawAuto();
    }
}


