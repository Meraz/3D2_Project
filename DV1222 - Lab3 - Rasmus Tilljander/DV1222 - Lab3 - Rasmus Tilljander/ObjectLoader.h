#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include "Object.h"
#include "ObjectVertex.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

class ObjectLoader
{
public:
	//ObjectLoader();
	//virtual ~ObjectLoader();

	Object* LoadObject(ID3D10Device* lDevice, char* lObjFileName, char* lFXFileName);

private:
//	std::vector<

};

ObjectLoader& GetObjectLoader();


#endif