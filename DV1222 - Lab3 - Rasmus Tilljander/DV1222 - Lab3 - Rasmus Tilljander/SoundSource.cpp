#include "SoundSource.h"

SoundSource::SoundSource()
{

}

SoundSource::~SoundSource()
{
	mSound->release();
}

void SoundSource::Initialize(char* lPath, D3DXVECTOR4 lPosition)
{
	mPosition = lPosition;
	mSound = FMODHandler::GetFMODHandler()->GetSound(lPath);
}


void SoundSource::Update()
{
	
}

void SoundSource::PlaySoundCustom()
{
	FMODHandler::GetFMODHandler()->GetFMODSystem()->playSound(mSound, 0, false, 0);
}