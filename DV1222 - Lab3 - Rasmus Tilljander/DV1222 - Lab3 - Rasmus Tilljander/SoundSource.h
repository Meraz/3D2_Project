#ifndef SOUNDSOURCE_H
#define SOUNDSOURCE_H

#include "fmod.h"
#include "Utilities.h"

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

	void Initialize(D3DXVECTOR4 lPosition);

private:
	D3DXVECTOR4 mPosition;
};


#endif