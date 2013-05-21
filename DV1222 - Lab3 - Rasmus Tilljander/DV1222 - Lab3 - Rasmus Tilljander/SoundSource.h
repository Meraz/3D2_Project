#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include "Utilities.h"
#include "Camera.h"
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

	void Initialize(char* lPath, D3DXVECTOR4 lPosition);

	void Update();	
	void PlaySoundCustom();


private:
	D3DXVECTOR4 mPosition;
	FMOD::Sound* mSound;
	FMOD::Channel* mChannel;
	D3DXVECTOR2 ConstantPower(double lPosition);	


};


#endif