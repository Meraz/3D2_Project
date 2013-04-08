#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include "Object.h"
#include "ObjectVertex.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct BufferWrapper //Used to be able to return both the buffer and the size of the buffer
{
	ID3D10Buffer* mBuffer;
	int mSize;
	BufferWrapper(ID3D10Buffer* lBuffer, UINT lSize) : mBuffer(lBuffer), mSize(lSize){}
};

class ObjectLoader
{
public:
	BufferWrapper* LoadObject(ID3D10Device* lDevice, std::string lFilePath);

private:

};

ObjectLoader& GetObjectLoader();


#endif