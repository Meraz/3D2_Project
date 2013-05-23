#include "SoundSource.h"

SoundSource::SoundSource()
{

}

SoundSource::~SoundSource()
{
	mSound->release();
}

void SoundSource::Initialize(char* lPath, D3DXVECTOR4 lPosition, SoundType lSoundType)
{
	mPosition = lPosition;
	mSound = FMODHandler::GetFMODHandler()->GetSound(lPath);
	mSoundType = lSoundType;
}

void SoundSource::Update()
{
	if(mSoundType == SoundType::Background)
		return;

	D3DXVECTOR3 tAim = GetCamera().GetAim();			//Get aim vector
	D3DXVECTOR3 tPos = GetCamera().GetPosition();		//Get position vector

	D3DXVECTOR2 lAim = D3DXVECTOR2(tAim.x, tAim.z);		// Store only positions in 2D
	D3DXVECTOR2 lToSource = D3DXVECTOR2(mPosition.x - tPos.x, mPosition.z - tPos.z);	//Vector from positon to source

 	D3DXVECTOR2 nVector;

	D3DXVECTOR3 temp2 = RotateY(D3DXVECTOR3(lToSource.x, 0, lToSource.y), PI/2);

	double b = GetSignedAngle(lAim, lToSource, D3DXVECTOR2(temp2.x, temp2.z));

	nVector = ConstantPower(-b);

	float lLength = D3DXVec3Length(&D3DXVECTOR3(mPosition.x - tPos.x,mPosition.y - tPos.y, mPosition.z - tPos.z));
	float lAmp = 1 - lLength/100;
	if(lAmp < 0)
		lAmp = 0;
	mChannel->setVolume(lAmp);

	mChannel->setMixLevelsOutput(nVector.x, nVector.y, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

D3DXVECTOR3 SoundSource::RotateY(D3DXVECTOR3 lVector, float lAngleInPI)
{
	float a = lAngleInPI;
	D3DXMATRIX lMatrix = D3DXMATRIX(
		cos(a),		0,		-sin(a),	0,
		0,			1,		0,			0,
		sin(a),	0,		cos(a),		0,
		0,			0,		0,			1
		);													
	D3DXVECTOR3 ReturnVector = Multiply(lVector, lMatrix);
	return ReturnVector;
}

D3DXVECTOR3 SoundSource::Multiply(D3DXVECTOR3 v, D3DXMATRIX& m)
{
	D3DXVECTOR3 temp;
	temp.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	temp.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	temp.z = v.x*m._13 + v.y*m._23 + v.z*m._33;

	return temp;
}

double SoundSource::GetSignedAngle(D3DXVECTOR2 FromVector, D3DXVECTOR2 DestVector, D3DXVECTOR2 DestVectorsRight)
{
	D3DXVec2Normalize(&FromVector, &FromVector);
	D3DXVec2Normalize(&DestVector, &DestVector);
	D3DXVec2Normalize(&DestVectorsRight, &DestVectorsRight);
 
	
    float forwardDot = D3DXVec2Dot(&FromVector, &DestVector);
    float rightDot = D3DXVec2Dot(&FromVector, &DestVectorsRight);
 
    // Keep dot in range to prevent rounding errors
	
    forwardDot = CLAMP(forwardDot, -1.0f, 1.0f);
 
    double angleBetween = acos(forwardDot);
 
    if (rightDot < 0.0f)
        angleBetween *= -1.0f;
 
    return angleBetween;
}

D3DXVECTOR2 SoundSource::ConstantPower(double lPosition)
{
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

void SoundSource::PlaySoundCustom()
{
	FMODHandler::GetFMODHandler()->GetFMODSystem()->playSound(mSound, 0, false, &mChannel);

	if(mSoundType == SoundType::Background)
		mChannel->setVolume(0.8);
	//FMODHandler::GetFMODHandler()->GetFMODSystem()->

	/*
	signed short* sptr = (signed short *)(void*)mSound; //Raw data ??
	for(int i = 0; i < 100; i++)
	{
		signed short b = sptr[i];
		int c = 5;
	}
	*/
}