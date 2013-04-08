#ifndef GENERALOBJECT_H
#define GENERALOBJECT_H

#include "Object.h"
#include "Utilities.h"

class GeneralObject : public Object
{
public:
	GeneralObject(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, int lNumberOfVertices);
	~GeneralObject();
	void Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName);
	
private:

private:


};
#endif