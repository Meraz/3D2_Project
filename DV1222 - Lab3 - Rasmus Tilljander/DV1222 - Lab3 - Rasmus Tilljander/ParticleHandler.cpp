#include "ParticleHandler.h"

ParticleHandler::ParticleHandler()
{
	mParticleSystem = std::vector<ParticleSystem*>();
	mParticleSystem.resize(AmountOfParticleSystems, 0);
	mTotalParticleSystem = 0;
}

ParticleHandler::~ParticleHandler()
{
	for(UINT i = 0; i < mParticleSystem.size(); i++) 
		SAFE_DELETE(mParticleSystem.at(i));
}

void ParticleHandler::Initialize(ID3D10Device* lDevice)
{
	mDevice = lDevice;
	mParticleSystem[0] = new Rain();
	mParticleSystem[0]->Initialize(lDevice, "FX/Rain.fx", 100000, sizeof(ParticleVertex));
	mParticleSystem[0]->AddEffect(ParticleVertexDescription, ParticleVertexInputLayoutNumElements, "StreamOutTech");
	mParticleSystem[0]->AddEffect(ParticleVertexDescription, ParticleVertexInputLayoutNumElements, "DrawTech");

	mParticleSystem[1] = new Sun();
	mParticleSystem[1]->Initialize(lDevice, "FX/Sun.fx", 1000, sizeof(ParticleVertex));
	mParticleSystem[1]->AddEffect(ParticleVertexDescription, ParticleVertexInputLayoutNumElements, "StreamOutTech");
	mParticleSystem[1]->AddEffect(ParticleVertexDescription, ParticleVertexInputLayoutNumElements, "DrawTech");
}

void ParticleHandler::Update(float lDeltaTime, float lGameTime)
{
	for(int i = 0; i < AmountOfParticleSystems; i++)
		mParticleSystem[i]->Update(lDeltaTime, lGameTime);
}

void ParticleHandler::Draw()
{
	for(int i = 0; i < AmountOfParticleSystems; i++)
		mParticleSystem[i]->Draw();
}


D3DXVECTOR4* ParticleHandler::GetParticleSystemPosition(UINT lIndex)
{
	return mParticleSystem[lIndex]->GetPosition();
}
