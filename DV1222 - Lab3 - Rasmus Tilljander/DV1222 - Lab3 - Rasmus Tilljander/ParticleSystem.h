#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "Camera.h"
#include "ParticleVertex.h"
#include "ShaderObject.h"
#include "ResourceLoader.h"
#include <string>

class ParticleSystem
{
public:	
	ParticleSystem();
	virtual ~ParticleSystem();

	virtual void Initialize(ID3D10Device* lDevice, char* lFilename, UINT maxParticles, UINT lStructSize);
	void AddEffect(const D3D10_INPUT_ELEMENT_DESC* lInputDescription, UINT lNumberOfElements, char* lTechniqueName);

	virtual void Reset();
	virtual void Update(float lDeltaTime, float lGameTime);
	virtual void Draw();

public:
	float GetAge() const { return mAge; }
	void SetEyePosition(D3DXVECTOR3 lCameraPosition) 
	{
		mCameraPosition = D3DXVECTOR4(lCameraPosition.x, lCameraPosition.y, lCameraPosition.z, 1.0f);
	}
	void SetEmitPosition(D3DXVECTOR3 lEmitPosition) 
	{
		mEmitPosition = D3DXVECTOR4(lEmitPosition.x, lEmitPosition.y, lEmitPosition.z, 1.0f);
	}
	void SetEmitDirection(D3DXVECTOR3 lEmitDirection)
	{
		mEmitDirection = D3DXVECTOR4(lEmitDirection.x, lEmitDirection.y, lEmitDirection.z, 0.0f);
	}

private:
	void BuildVertexBuffer(UINT lStructSize);
 
protected:
	void BuildRandomTexture();
	void LoadTexture(char* lFileName);


protected:
	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;

	D3DXMATRIX mWorldMatrix;

	UINT mMaxParticles;
	bool mFirstRun;

	float mGameTime;
	float mDeltaTime;
	float mAge;

	D3DXVECTOR4 mCameraPosition;
	D3DXVECTOR4 mEmitPosition;
	D3DXVECTOR4 mEmitDirection;

	ID3D10Buffer* mInitVertexBuffer;	
	ID3D10Buffer* mDrawVertexBuffer;
	ID3D10Buffer* mStreamOutVertexBuffer;
 
	ID3D10ShaderResourceView* mTexArrayRV;
	ID3D10ShaderResourceView* mRandomTexRV;
};

#endif 