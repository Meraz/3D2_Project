#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include "Object.h"
#include "ObjectVertex.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class ObjectLoader
{
public:
	Object* LoadObject(ID3D10Device* lDevice, std::string lFilePath, char* lFXFileName);

private:

};

ObjectLoader& GetObjectLoader();


#endif