#ifndef SCREEN_H
#define SCREEN_H

#include "Screen.h"
#include "Utilities.h"
#include "Camera.h"
#include "ParticleHandler.h"
#include "WorldHandler.h"
#include "GameTimer.h"
#include "DrawableTex2D.h"

enum state
{
	walking,
	falling
};

class Screen
{
public:
	Screen();
	~Screen();
	void Initialize(ID3D10Device* lDevice, ID3D10RenderTargetView *lRenderTargetView, ID3D10DepthStencilView *lDepthStencilView, UINT lClientHeight, UINT lClientWidth);



	void Update();
	void KeyBoardMovement(float lWalkingSpeed, float lDeltaTime);
		
	void Draw();

private:
	void ResetOMTargetsAndViewport();
	void UpdateSunWVP();

private:
	ID3D10DepthStencilView* mDepthStencilView;
	ID3D10RenderTargetView* mRenderTargetView;
	UINT mClientWidth, mClientHeight;
	WorldHandler* mWorldHandler;
	GameTimer* mGameTimer;
	ID3D10Device* mDevice;
	ParticleHandler* mParticleHandler;
	POINT mLastMousePosition;
	DrawableTex2D* mShadowMap;
	D3DXMATRIX		mLightProj;
	D3DXMATRIX		mLightView;
	D3DXVECTOR4* mSunPosition;

	
	//Gravity stuff
	float mVelocityY;
	float mGravity;
	state mState;

	void Jump();
	void UpdateY();
	void GroundCollision();
	float mDeltaTime;
	float mGameTime;


};

#endif
