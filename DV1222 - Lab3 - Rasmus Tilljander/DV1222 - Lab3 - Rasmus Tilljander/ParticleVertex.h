#ifndef PARTICLEVERTEX_H
#define PARTICLEVERTEX_H

#include "Utilities.h"

struct ParticleVertex
{
	D3DXVECTOR3 mInitialPosition;
	D3DXVECTOR3 mInitialVelocity;
	D3DXVECTOR2 mSize;
	float mAge;
	unsigned int mType;
};

const D3D10_INPUT_ELEMENT_DESC ParticleVertexDescription[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"AGE",      0, DXGI_FORMAT_R32_FLOAT,       0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE",     0, DXGI_FORMAT_R32_UINT,        0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0}
};

const int ParticleVertexInputLayoutNumElements = sizeof(ParticleVertexDescription)/sizeof(ParticleVertexDescription[0]);

#endif