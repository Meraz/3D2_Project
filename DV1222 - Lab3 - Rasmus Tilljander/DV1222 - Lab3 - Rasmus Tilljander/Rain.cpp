#include "Rain.h"

Rain::Rain()
{
	mEmitPosition = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	mEmitDirection = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 0.0f);
}

Rain::~Rain()
{

}

void Rain::Initialize(ID3D10Device* lDevice, char* lFilename, UINT maxParticles, UINT lStructSize)
{
	ParticleSystem::Initialize(lDevice, lFilename, maxParticles, lStructSize);
}

void Rain::Reset()
{
	ParticleSystem::Reset();
}

void Rain::Update(float lDeltaTime, float lGameTime)
{
	ParticleSystem::Update(lDeltaTime, lGameTime);
	ParticleSystem::BuildRandomTexture();
}

void Rain::Draw()
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