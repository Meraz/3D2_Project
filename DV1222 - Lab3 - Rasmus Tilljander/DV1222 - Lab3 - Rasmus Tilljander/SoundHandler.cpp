#include "SoundHandler.h"


SoundHandler::SoundHandler()
{
	mSource = std::vector<SoundSource*>();
}


SoundHandler::~SoundHandler()
{
	
}


void SoundHandler::Initialize()
{
	
}


void SoundHandler::AddSource(D3DXVECTOR4 lPosition)
{
	SoundSource* lSource = new SoundSource();
	lSource->Initialize(lPosition);
	mSource.push_back(lSource);
}