#include "Screen.h"

Screen::Screen()
{	
	mWorldHandler = new WorldHandler();
	mGameTimer = new GameTimer();
	mParticleHandler = new ParticleHandler();
}

Screen::~Screen()
{
	delete mWorldHandler;
	delete mGameTimer;
	delete mParticleHandler;
}

void Screen::Initialize(ID3D10Device* lDevice)
{
	mDevice = lDevice;

	mWorldHandler->Initialize(lDevice, 513, 513, 1.0f);

	mGameTimer->Start();
	mGameTimer->Tick();

	mParticleHandler->Initialize(lDevice);

	GetResourceLoader().LoadTextures(lDevice);
	GetCamera().SetYPosition(mWorldHandler->GetHeight(GetCamera().GetPosition().x, GetCamera().GetPosition().z) + HeightOffset);
	GetCamera().RebuildView();
	ShowCursor(false);
}


void Screen::Update()
{	
	mGameTimer->Tick();
	float lDeltaTime = mGameTimer->GetDeltaTime();
	float lGameTime = mGameTimer->GetGameTime();

	float lWalkingSpeed = gPlayerMovementSpeed;
	bool lWalking = false;

	if(GetAsyncKeyState(VK_LSHIFT) & 0x8000)
		lWalkingSpeed = gPlayerMovementSpeed*2;

	if(GetAsyncKeyState('W') || GetAsyncKeyState('A') || GetAsyncKeyState('S') || GetAsyncKeyState('D') & 0x8000) 
		lWalking = true;
		
	KeyBoardMovement(lWalkingSpeed, lDeltaTime);

	if(lWalking)
	{
		GetCamera().SetYPosition(mWorldHandler->GetHeight(GetCamera().GetPosition().x, GetCamera().GetPosition().z) + HeightOffset);
	}

	mWorldHandler->Update(lDeltaTime);
	mParticleHandler->Update(lDeltaTime, lGameTime);
	
	GetCamera().RebuildView();	
}

void Screen::KeyBoardMovement(float lWalkingSpeed, float lDeltaTime)
{
	if(GetAsyncKeyState('A') & 0x8000) GetCamera().Strafe	(-lWalkingSpeed * lDeltaTime);
	if(GetAsyncKeyState('D') & 0x8000) GetCamera().Strafe	(+lWalkingSpeed * lDeltaTime);
	if(GetAsyncKeyState('W') & 0x8000) GetCamera().Walk		(+lWalkingSpeed * lDeltaTime);
	if(GetAsyncKeyState('S') & 0x8000) GetCamera().Walk		(-lWalkingSpeed * lDeltaTime);
	
	if(GetAsyncKeyState('Q') & 0x8000) 
	{
		GetCamera().mCameraAngleXZ--;
		GetCamera().Update();
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
}
void Screen::Draw()
{
	mWorldHandler->Draw();
	mParticleHandler->Draw();
}