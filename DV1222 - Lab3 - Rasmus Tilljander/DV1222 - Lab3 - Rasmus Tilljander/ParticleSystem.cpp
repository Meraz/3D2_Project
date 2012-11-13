#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
: mDevice(0), mInitVertexBuffer(0), mDrawVertexBuffer(0), mStreamOutVertexBuffer(0), mTexArrayRV(0), mRandomTexRV(0)
{
	mFirstRun = true;
	mGameTime = 0.0f;
	mDeltaTime = 0.0f;
	mAge      = 0.0f;
	mShaderObject = new ShaderObject();

	mCameraPosition  = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
}

ParticleSystem::~ParticleSystem()
{
	SAFE_RELEASE(mInitVertexBuffer);
	SAFE_RELEASE(mDrawVertexBuffer);
	SAFE_RELEASE(mStreamOutVertexBuffer);
	SAFE_DELETE(mShaderObject);

}

void ParticleSystem::Initialize(ID3D10Device* lDevice, char* lFilename, UINT maxParticles, UINT lStructSize)
{
	mDevice = lDevice;
	mShaderObject->Initialize(mDevice, lFilename, D3D10_SHADER_ENABLE_STRICTNESS);

	mMaxParticles = maxParticles;
	BuildRandomTexture();
	BuildVertexBuffer(lStructSize);
}

void ParticleSystem::AddEffect(const D3D10_INPUT_ELEMENT_DESC* lInputDescription, UINT lNumberOfElements, char* lTechniqueName)
{
	mShaderObject->AddTechniqueByName(lInputDescription, lNumberOfElements, lTechniqueName);
}

void ParticleSystem::Reset()
{
	mFirstRun = true;
	mAge      = 0.0f;
}

void ParticleSystem::BuildRandomTexture()
{
	// Create the random data.
	D3DXVECTOR4 randomValues[1024];

	for(int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = RandF(-1.0f, 1.0f);
		randomValues[i].y = RandF(-1.0f, 1.0f);
		randomValues[i].z = RandF(-1.0f, 1.0f);
		randomValues[i].w = RandF(-1.0f, 1.0f);
	}

    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024*sizeof(D3DXVECTOR4);
    initData.SysMemSlicePitch = 1024*sizeof(D3DXVECTOR4);

	// Create the texture.
    D3D10_TEXTURE1D_DESC texDesc;
    texDesc.Width = 1024;
    texDesc.MipLevels = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.Usage = D3D10_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;
    texDesc.ArraySize = 1;

	ID3D10Texture1D* randomTex = 0;
    mDevice->CreateTexture1D(&texDesc, &initData, &randomTex);
	
	// Create the resource view.
    D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE1D;
    viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;
	
    mDevice->CreateShaderResourceView(randomTex, &viewDesc, &mRandomTexRV);

	SAFE_RELEASE(randomTex);
}

void ParticleSystem::Update(float lDeltaTime, float lGameTime)
{
	mGameTime = lGameTime;
	mDeltaTime = lDeltaTime;
	mAge += lDeltaTime;
}

void ParticleSystem::BuildVertexBuffer(UINT lStructSize)
{
	// Create the buffer to kick-off the particle system.
	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(lStructSize) * 1;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

	// The initial particle emitter has type 0 and age 0.  The rest
	// of the particle attributes do not apply to an emitter.
	ParticleVertex p;
	ZeroMemory(&p, sizeof(lStructSize));
	p.mAge  = 0.0f;
	p.mType = 0; 
 
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &p;

	mDevice->CreateBuffer(&vbd, &vinitData, &mInitVertexBuffer);
	
	// Create the ping-pong buffers for stream-out and drawing.
	vbd.ByteWidth = sizeof(lStructSize) * mMaxParticles;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT;

	mDevice->CreateBuffer(&vbd, 0, &mDrawVertexBuffer);
	mDevice->CreateBuffer(&vbd, 0, &mStreamOutVertexBuffer);
}

void ParticleSystem::Draw()
{
	mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());
	mShaderObject->SetFloat("GameTime", mGameTime);
	mShaderObject->SetFloat("DeltaTime", mDeltaTime);
	mShaderObject->SetFloat4("CameraPosition", mCameraPosition);
	mShaderObject->SetFloat4("EmitPosition", mEmitPosition);
	mShaderObject->SetFloat4("EmitDirection", mEmitDirection);
	mShaderObject->SetResource("RandomTexture", mRandomTexRV);
}