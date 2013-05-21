#include "SoundSource.h"

SoundSource::SoundSource()
{

}

SoundSource::~SoundSource()
{
	mSound->release();
}

void SoundSource::Initialize(char* lPath, D3DXVECTOR4 lPosition)
{
	mPosition = lPosition;
	mSound = FMODHandler::GetFMODHandler()->GetSound(lPath);
}

void SoundSource::Update()
{
	D3DXVECTOR3 tAim = GetCamera().GetAim();			//Get aim vector
	D3DXVECTOR3 tPos = GetCamera().GetPosition();		//Get aim vector

	D3DXVECTOR2 lAim = D3DXVECTOR2(tAim.x, tAim.z);		// Store only positions in 2D
	D3DXVECTOR2 lToSource = D3DXVECTOR2(mPosition.x - tPos.x, mPosition.z - tPos.z);	//Vector from positon to source

	D3DXVec2Normalize(&lAim, &lAim);
	D3DXVec2Normalize(&lToSource, &lToSource);
	
	double a = D3DXVec2Dot(&lAim, &lToSource);
	double lAngle = acos(a);

	D3DXVECTOR2 nVector;

	if(lToSource.x > 0.5 || lToSource.y > 0.5)
		nVector = ConstantPower(lAngle);
	else
		nVector = ConstantPower(-lAngle);
		


	//nVector = ConstantPower(-1.0f);swsssssaaaaaaawwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww


	mChannel->setMixLevelsOutput(nVector.x, nVector.y, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

void SoundSource::PlaySoundCustom()
{
	FMODHandler::GetFMODHandler()->GetFMODSystem()->playSound(mSound, 0, false, &mChannel);
	//FMODHandler::GetFMODHandler()->GetFMODSystem()->GetSoundR

	/*
	signed short* sptr = (signed short *)(void*)mSound; //Raw data ??
	for(int i = 0; i < 100; i++)
	{
		signed short b = sptr[i];
		int c = 5;
	}
	*/
}

D3DXVECTOR2 SoundSource::ConstantPower(double lPosition)
{
	/*
		lPosition spans from -1 to +1
		in which the values represent the pan position ranging from -1 - full left, to +1.0 : full right.
		if(lPosition == 0.0) then we need to specify a special case as the following is true:
			sqrt( 2 ) / 2 * (cos(  π/4 ) - sin( π/4 )) = 0; //vänster kanal
			sqrt( 2 ) / 2 * (cos(  π/4 ) + sin( π/4 )) = 1; //höger kanal
	*/
	
	D3DXVECTOR2 pos = D3DXVECTOR2(0,0); // Return value
	
	const double piover2 = 4.0 * atan(1.0) * 0.5; //Equals PI / 2 as the following is true: double PI = 4.0 * atan(1.0);
		
	/* scale position to fit Lh'e pi/2 range */
	double thispos = lPosition;// * piover2;
	
	/* each channel uses a 1/4 of a cycle */
	double angle = thispos * 0.5;

	const double root2over2 = sqrt(2.0) * 0.5;
	pos.x = root2over2 * (cos(angle) - sin(angle) );
	pos.y = root2over2 * (cos(angle) + sin(angle) );
	return pos;
}