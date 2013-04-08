#include "ObjectFactory.h"


ObjectFactory* ObjectFactory::mObjectFactory = 0;

ObjectFactory* ObjectFactory::GetObjectFactory()
{
	if(mObjectFactory == 0)
		mObjectFactory = new ObjectFactory();
	return mObjectFactory;
}


void ObjectFactory::Initialize(ID3D10Device* lDevice)
{
	mDevice = lDevice;
}


Object* ObjectFactory::LoadObject(ID3D10Device* lDevice, std::string lFilePath, LoadableObject lObject)
{
	BufferWrapper* a =  GetObjectLoader().LoadObject(lDevice, lFilePath);
	if(lObject == LoadableObject::General)
	{
		return new GeneralObject(lDevice, a->mBuffer, a->mSize);
	}
	if(lObject == LoadableObject::Box)
	{
		return new ObjectBox(lDevice, a->mBuffer, a->mSize);
	}
}