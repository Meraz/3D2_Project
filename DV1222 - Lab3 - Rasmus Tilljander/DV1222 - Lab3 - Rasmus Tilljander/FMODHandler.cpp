#include "FMODHandler.h"

FMODHandler* FMODHandler::Handler = 0;


FMODHandler* FMODHandler::GetFMODHandler()
{
	if (Handler == 0)
	{
		Handler = new FMODHandler();
	}
	return Handler;
}

FMODHandler::FMODHandler()
{
	FMOD::System_Create(&mSystem);
	mSystem->init(32, FMOD_INIT_NORMAL, 0);
}

FMODHandler::~FMODHandler()
{
	mSystem->close();
	mSystem->release();
}

void FMODHandler::Update()
{
	mSystem->update();
}

FMOD::Sound* FMODHandler::GetSound(char* lPointer)
{
	FMOD::Sound* lSound;
	mSystem->createSound(lPointer, FMOD_HARDWARE, 0, &lSound);
	lSound->setMode(FMOD_LOOP_NORMAL);
	return lSound;
}