#ifndef SCREEN_H
#define SCREEN_H

#include "Screen.h"
#include "Utilities.h"
#include "Camera.h"
#include "ParticleHandler.h"
#include "WorldHandler.h"
#include "GameTimer.h"

class Screen
{
public:
	Screen();
	~Screen();
	void Initialize(ID3D10Device* lDevice);

	void Update();
	void KeyBoardMovement(float lWalkingSpeed, float lDeltaTime);
		
	void Draw();

private:

private:
	WorldHandler* mWorldHandler;
	GameTimer* mGameTimer;
	ID3D10Device* mDevice;
	ParticleHandler* mParticleHandler;

	POINT mLastMousePosition;

};

#endif
