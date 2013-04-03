#ifndef TREE_H
#define TREE_H

#include "Utilities.h"
#include "ShaderObject.h"
#include "ResourceLoader.h"
#include "Camera.h"

struct BillboardVertex
{
	D3DXVECTOR4 mPosition;
	D3DXVECTOR2 mSize;
};

const D3D10_INPUT_ELEMENT_DESC BillboardVertexDescription[] =
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",		0, DXGI_FORMAT_R32G32_FLOAT,		0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0}
};

const int BillboardVertexInputLayoutNumElements = sizeof(BillboardVertexDescription)/sizeof(BillboardVertexDescription[0]);

class Tree
{
public:
	Tree();
	~Tree();
	void Initialize(ID3D10Device* lDevice, D3DXVECTOR4 lPosition);
	void Draw();


private:
	D3DXVECTOR4 mPosition;
	D3DXMATRIX mWorldMatrix;
	
	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;
	ID3D10Buffer* mVertexBuffer;
	int mNumberOfVertices;

private:
	void CreateVertexBuffer(int lSize, D3DXVECTOR4 lPosition);

};

#endif