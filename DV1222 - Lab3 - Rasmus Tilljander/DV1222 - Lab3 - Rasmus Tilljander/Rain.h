#ifndef RAIN_H
#define RAIN_H

#include "ParticleSystem.h"

class Rain : public ParticleSystem
{
public:
	Rain();
	virtual ~Rain();

	void Initialize(ID3D10Device* lDevice, char* lFilename, UINT maxParticles, UINT lStructSize);

	void Reset();
	void Update(float lDeltaTime, float lGameTime);
	void Draw();
};


#endif