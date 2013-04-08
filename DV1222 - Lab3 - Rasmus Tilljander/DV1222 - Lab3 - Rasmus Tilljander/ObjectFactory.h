#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H

#include "Utilities.h"
#include "Object.h"
#include "GeneralObject.h"
#include "ObjectBox.h"
#include "ObjectLoader.h"

enum LoadableObject
{
	General,
	Box
};

class ObjectFactory
{
public:
	static ObjectFactory* GetObjectFactory();

	Object* LoadObject(ID3D10Device* lDevice, std::string lFilePath, LoadableObject lObject);



private:
	static ObjectFactory* mObjectFactory;
	ID3D10Device* mDevice;
	void Initialize(ID3D10Device* lDevice);
	
};

#endif