#ifndef SUN_H
#define SUN_H
#include "ParticleSystem.h"
class Sun: public ParticleSystem
{
public:
	Sun();
	~Sun();
	void	Initialize(ID3D10Device* lDevice, char* lFilename, UINT maxParticles, UINT lStructSize);
	void	Update(float ldt, float lGameTime);
	void	Draw();

private:
	void	CreateBuffer();
	
	void	CreateTexture();
public:
private:
	
};

#endif