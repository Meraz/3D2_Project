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
	Object();
	virtual ~Object();
	virtual void Object::Initialize(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, char* lFXFileName, int lNumberOfVertices);

	virtual void Update(float lDeltaTime);
	virtual void ShadowDraw(D3DXMATRIX lLightWVP);
	virtual void Draw();

protected:
	D3DXVECTOR4 mPosition;
	D3DXMATRIX mWorldMatrix;
	
	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;
	ID3D10Buffer* mVertexBuffer;
	int mNumberOfVertices;

};

#endif
