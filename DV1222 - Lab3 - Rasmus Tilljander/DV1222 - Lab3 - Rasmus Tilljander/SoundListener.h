#ifndef SOUNDLISTENER_H
#define SOUNDLISTENER_H

#include "fmod.h"
#include "Utilities.h"
#include "Camera.h"

enum Direction
{
	 Left, Right
};


class SoundListener
{
public:
	SoundListener();
	~SoundListener();
	void Initialize(D3DXVECTOR3 lPosition, Direction lDirection);
	
	void Update(D3DXVECTOR3 lPosition);

private:
	D3DXVECTOR4 mPosition;
	Direction mDirection;

	void UpdateSoundLocation();


};

#endif