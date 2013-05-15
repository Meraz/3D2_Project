#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include "Utilities.h"
#include "FMODHandler.h"

enum SoundType
{
	FixedPosition,
	Background
};

class SoundSource
{
public:
	SoundSource();
	~SoundSource();
	D3DXVECTOR4		GetPosition()		 const { return mPosition; }
	FMOD::Sound* mSound;
	void Initialize(char* lPath, D3DXVECTOR4 lPosition);

	void Update();	
	void PlaySoundCustom();


private:
	D3DXVECTOR4 mPosition;
};


#endif