#include "SoundListener.h"

SoundListener::SoundListener()
{

}

SoundListener::~SoundListener()
{

}

void SoundListener::Initialize(D3DXVECTOR3 lPosition, Direction lDirection)
{
	mPosition = D3DXVECTOR4(lPosition,1.0f);
	mDirection = lDirection;
	UpdateSoundLocation();
}
	
void SoundListener::Update(D3DXVECTOR3 lPosition)
{
	mPosition = D3DXVECTOR4(lPosition,1.0f);
	UpdateSoundLocation();
}

void SoundListener::UpdateSoundLocation()
{
	D3DXVECTOR3 lAim =  GetCamera().GetAim();
	D3DXVECTOR3 temp;
	D3DXVec3Cross(&temp, &lAim, &D3DXVECTOR3(0,1,0));
	D3DXVec3Normalize(&temp, &temp);
	temp *= 2;
	temp.y = 0; //No movement in Y

	if(mDirection == Direction::Left)
		mPosition -= D3DXVECTOR4(temp, 1.0f);
	else if(mDirection == Direction::Right)
		mPosition += D3DXVECTOR4(temp, 1.0f);
}