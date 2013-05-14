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
