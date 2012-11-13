#include "Utilities.h"

#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	D3DXVECTOR4 mPosition;
	D3DXVECTOR3 mNormal;
	D3DXVECTOR2 mTexCoord;

	Vertex () : mNormal( D3DXVECTOR3(0,0,0) )
	{			
	}
	
	Vertex( D3DXVECTOR3 p, D3DXVECTOR3 n ) : mPosition(p), mNormal(n)
	{		
	}	

	Vertex( D3DXVECTOR3 p, D3DXVECTOR2 t, D3DXVECTOR3 n ) : mPosition(p), mNormal(n), mTexCoord(t)
	{
	}	
};

const D3D10_INPUT_ELEMENT_DESC VertexLayout[] =
{
	{ "POSITION",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 0,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 16,	D3D10_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0, 28,	D3D10_INPUT_PER_VERTEX_DATA, 0 }
};

const int VertexInputLayoutNumElements = sizeof(VertexLayout)/sizeof(VertexLayout[0]);

#endif