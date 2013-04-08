#ifndef OBJECTBOX_H
#define OBJECTBOX_H

#include "Utilities.h"
#include "ShaderObject.h"
#include "ResourceLoader.h"
#include "Object.h"
#include <vector>


struct BoxVertex
{
	D3DXVECTOR4 mPosition;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR2 mTexCoord;

	BoxVertex () : mNormal( D3DXVECTOR3(0,0,0) )
	{			
	}
	
	BoxVertex( D3DXVECTOR3 p, D3DXVECTOR3 n ) : mPosition(p), mNormal(n)
	{		
	}	

	BoxVertex( D3DXVECTOR3 p, D3DXVECTOR2 t, D3DXVECTOR3 n ) : mPosition(p), mNormal(n), mTexCoord(t)
	{
	}	
};

const D3D10_INPUT_ELEMENT_DESC BoxVertexLayout[] =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 16,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

const int BoxVertexInputLayoutNumElements = sizeof(BoxVertexLayout)/sizeof(BoxVertexLayout[0]);

class ObjectBox : public Object
{

public:
	ObjectBox(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, int lNumberOfVertices);
	virtual ~ObjectBox();
	void Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName);


	void Update();
	void Draw();

private:

private:
	D3DXVECTOR4 mPosition;
	D3DXMATRIX mWorldMatrix;
	
	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;
	ID3D10Buffer* mVertexBuffer;
	int mNumberOfVertices;
};



#endif BOX_H