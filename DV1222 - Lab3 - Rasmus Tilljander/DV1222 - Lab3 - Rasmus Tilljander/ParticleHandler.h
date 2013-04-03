#ifndef PARTICLEHANDLER_H
#define PARTICLEHANDLER_H

#include <vector>
#include "ParticleSystem.h"
#include "ParticleVertex.h"
#include "Rain.h"
#include "Sun.h"

class ParticleHandler
{
public:
	ParticleHandler();
	~ParticleHandler();
	void Initialize(ID3D10Device* lDevice);

	D3DXVECTOR4* GetParticleSystemPosition(UINT lIndex);



	void Update(float lDeltaTime, float lGameTime);
	void Draw();

private:

private:
	ID3D10Device* mDevice;
	std::vector<ParticleSystem*> mParticleSystem;
	UINT mTotalParticleSystem;

};


#endif