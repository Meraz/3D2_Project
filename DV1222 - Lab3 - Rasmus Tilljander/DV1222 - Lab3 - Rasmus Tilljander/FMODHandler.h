#ifndef FMODHANBDLER_H
#define FMODHANBDLER_H

#include "fmod.hpp" //fmod c++ header
#pragma comment( lib, "fmod_vc.lib" ) // fmod library

//http://glasnost.itcarlow.ie/~powerk/audio/AddFMODtoaproject.html
class FMODHandler
{
public:
	static FMODHandler* GetFMODHandler();
	FMOD::System* GetFMODSystem() const {return mSystem};

private:
	FMODHandler();
	~FMODHandler();
	FMOD::System* mSystem;
	static FMODHandler* Handler;
};

#endif