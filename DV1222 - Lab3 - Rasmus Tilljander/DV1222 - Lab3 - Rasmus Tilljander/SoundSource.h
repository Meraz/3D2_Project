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

#define CLAMP(value, low, high) (((value)<(low))?(low):(((value)>(high))?(high):(value)))
class SoundSource
{
public:
	SoundSource();
	~SoundSource();
	D3DXVECTOR4		GetPosition()		 const { return mPosition; }

	void Initialize(char* lPath, D3DXVECTOR4 lPosition, SoundType lSoundType);
	void Update();	
	void PlaySoundCustom();

private:
	D3DXVECTOR4 mPosition;
	FMOD::Sound* mSound;
	FMOD::Channel* mChannel;
	D3DXVECTOR2 ConstantPower(double lPosition);	
	double GetSignedAngle(D3DXVECTOR2 FromVector, D3DXVECTOR2 DestVector, D3DXVECTOR2 DestVectorsRight);
	D3DXVECTOR3 RotateY(D3DXVECTOR3 lVector, float lAngleInPI);
	D3DXVECTOR3 Multiply(D3DXVECTOR3 v, D3DXMATRIX& m);
	SoundType mSoundType;

};

#endif