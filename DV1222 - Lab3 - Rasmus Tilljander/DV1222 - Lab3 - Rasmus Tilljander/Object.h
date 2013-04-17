#ifndef OBJECT_H
#define OBJECT_H

#include "Utilities.h"
#include "ShaderObject.h"
#include "Camera.h"
#include "ObjectVertex.h"
#include "ResourceLoader.h"

class Object
{
public:
	Object(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, int lNumberOfVertices);
	virtual ~Object();
	virtual void Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName);

	virtual void Update(float lDeltaTime);
	virtual void ShadowDraw(D3DXMATRIX lLightProj, D3DXMATRIX lLightView);
	virtual void Draw(D3DXVECTOR4 lSunPos,D3DXMATRIX lLightProj, D3DXMATRIX lLightView,ID3D10ShaderResourceView* lShadowmap);
	
protected:
	D3DXMATRIX mWorldMatrix;
	
	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;
	ID3D10Buffer* mVertexBuffer;
	int mNumberOfVertices;

};

#endif
