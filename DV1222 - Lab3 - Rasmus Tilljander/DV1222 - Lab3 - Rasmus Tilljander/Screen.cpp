#include "Screen.h"

Screen::Screen()
{	
	mWorldHandler		= new WorldHandler();
	mGameTimer			= new GameTimer();
	mParticleHandler	= new ParticleHandler();
	mShadowMap			= new DrawableTex2D();
	mReflectionMap		= new DrawableTex2D();
	mGravity = 9.82f*2;
	mState = walking;
	mSunPosition = 0;
}

Screen::~Screen()
{
	delete mWorldHandler;
	delete mGameTimer;
	delete mParticleHandler;
	delete mShadowMap;
}

void Screen::Initialize(ID3D10Device* lDevice, ID3D10RenderTargetView *lRenderTargetView, ID3D10DepthStencilView *lDepthStencilView, UINT lClientHeight, UINT lClientWidth)
{
	mDevice = lDevice;
	mClientHeight = lClientHeight;
	mClientWidth = lClientWidth;
	mRenderTargetView = lRenderTargetView;
	mDepthStencilView = lDepthStencilView;

	mWorldHandler->Initialize(lDevice, 513, 513, 1.0f);
	
	mGameTimer->Start();
	mGameTimer->Tick();
	mDeltaTime = mGameTimer->GetDeltaTime();
	mGameTime = mGameTimer->GetGameTime();

	mParticleHandler->Initialize(lDevice);
	mSunPosition = mParticleHandler->GetParticleSystemPosition(1);
	mReflectionMap->Initialize(mDevice,lClientWidth, lClientHeight, true, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mShadowMap->Initialize(mDevice, lClientWidth, lClientHeight, false, DXGI_FORMAT_UNKNOWN);

	
	

//#define HeightOffset -20
	GetCamera().SetYPosition(mWorldHandler->GetHeight(GetCamera().GetPosition().x, GetCamera().GetPosition().z) + HeightOffset);
	GetCamera().RebuildView();
	ShowCursor(false);
}


void Screen::Update()
{	
	UpdateY();

	float lWalkingSpeed = gPlayerMovementSpeed;

	if(GetAsyncKeyState(VK_LSHIFT) & 0x8000 && mState != falling)
		lWalkingSpeed = gPlayerMovementSpeed*2;
		
	KeyBoardMovement(lWalkingSpeed, mDeltaTime);
		
	mWorldHandler->Update(mDeltaTime);
	mParticleHandler->Update(mDeltaTime, mGameTime);
	
	UpdateSunWVP();
	mGameTimer->Tick();
	mDeltaTime = mGameTimer->GetDeltaTime();
	mGameTime = mGameTimer->GetGameTime();

	//tror det är skillnaden i position i z ledet som ska in som offset, might be wrong, kör på 50 sålänge
	GetCamera().BuildViewReflection(50.0f);
	GetCamera().RebuildView();	
}

void Screen::UpdateY()
{
	float x, y, z;
	x = GetCamera().GetPosition().x;
	y = GetCamera().GetPosition().y;
	z = GetCamera().GetPosition().z;

	mVelocityY -= mGravity*mDeltaTime;

	if(mState == walking)
		GetCamera().SetYPosition(mWorldHandler->GetHeight(x, z) + HeightOffset);
	else
	{
		float newYPos = mWorldHandler->GetHeight(x, z) + HeightOffset;
		if(y + (mVelocityY * mDeltaTime) <= newYPos)
		{
			//Reached ground
			float lDeltaTime = (newYPos - y)/mVelocityY;
			//Calculate new delta time
			y += lDeltaTime*mVelocityY;
			GetCamera().SetYPosition(y);
			//Stop falling
			mVelocityY = 0;
			mState = walking;
			return;
		}
		//Keep falling
		y += mDeltaTime*mVelocityY;
		GetCamera().SetYPosition(y);
	}
}

void Screen::GroundCollision()
{

}

void Screen::Jump()
{
	if(mState != falling)
	{
		mVelocityY = 40;
		mState = falling;
	}
}

void Screen::KeyBoardMovement(float lWalkingSpeed, float lDeltaTime)
{
		if(GetAsyncKeyState('A') & 0x8000)
		{
			GetCamera().Strafe(-lWalkingSpeed * lDeltaTime);
		}
		if(GetAsyncKeyState('D') & 0x8000) GetCamera().Strafe	(+lWalkingSpeed * lDeltaTime);
		if(GetAsyncKeyState('W') & 0x8000) GetCamera().Walk		(+lWalkingSpeed * lDeltaTime);
		if(GetAsyncKeyState('S') & 0x8000) GetCamera().Walk		(-lWalkingSpeed * lDeltaTime);

	
	if(GetAsyncKeyState('Q') & 0x8000) 
	{
		GetCamera().mCameraAngleXZ--;
		GetCamera().Update();
	}
	
	if(GetAsyncKeyState('R') & 0x8000) 
	{
		GetCamera().SetAim((D3DXVECTOR3)*mSunPosition);
	}

	if(GetAsyncKeyState('E') & 0x8000)
	{
		GetCamera().mCameraAngleXZ++;
		GetCamera().Update();
	}
		if(GetAsyncKeyState('1') & 0x8000) 
	{
		GetCamera().mCameraAngleXY--;
		GetCamera().Update();
	}

	if(GetAsyncKeyState('2') & 0x8000)
	{
		GetCamera().mCameraAngleXY++;
		GetCamera().Update();
	}

	if(GetAsyncKeyState('3') & 0x8000) 
		mWorldHandler->ChangeTechnique(0);
	if(GetAsyncKeyState('4') & 0x8000) 
		mWorldHandler->ChangeTechnique(1);
	if(GetAsyncKeyState(VK_SPACE) & 0x8000)
		Jump();
}

void Screen::Draw()
{
	//create the shadowmap texture
	mShadowMap->Begin();
	mWorldHandler->ShadowDraw(mLightProj, mLightView);
	mShadowMap->End();
	/*mReflectionMap->Begin();
	mWorldHandler->Draw(*mSunPosition, mLightProj, mLightView,  mShadowMap->GetDepthMap(), true);
	mReflectionMap->End();*/
	

	ResetOMTargetsAndViewport();


	mWorldHandler->Draw(*mSunPosition, mLightProj, mLightView,  mShadowMap->GetDepthMap(), false);
	mParticleHandler->Draw();
}

void Screen::ResetOMTargetsAndViewport()
{
	mDevice->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	D3D10_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = mClientWidth;
	vp.Height = mClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	mDevice->RSSetViewports(1, &vp);
}

void Screen::UpdateSunWVP()
{
	D3DXVECTOR3 lSunPos = (D3DXVECTOR3)*mSunPosition;
	D3DXVECTOR3 lLookAt = D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 lRight = D3DXVECTOR3(1,0,0);
	D3DXVECTOR3 lUp = D3DXVECTOR3(0,1,0);
	//D3DXVec3Cross(&lUp, &lRight, D3DXVec3Normalize(&lSunPos,&lSunPos));
		
	D3DXMatrixPerspectiveFovLH(&mLightProj, 0.25f*PI, (float)(mClientWidth/mClientHeight), 40.0f, 1000.0f);
	D3DXMatrixLookAtLH(&mLightView, &lSunPos, &lLookAt, &lUp);
	
}
