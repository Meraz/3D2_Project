#include "GeneralObject.h"

GeneralObject::GeneralObject(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, int lNumberOfVertices)
	: Object(lDevice, lVertexBuffer, lNumberOfVertices)
{
	mShaderObject = new ShaderObject();
	mDevice = lDevice;
	mVertexBuffer = lVertexBuffer;
	
	D3DXMatrixIdentity(&mWorldMatrix);

	mNumberOfVertices = lNumberOfVertices;
}

GeneralObject::~GeneralObject()
{

}

void GeneralObject::Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName)
{
	Object::Initialize(lWorldMatrix, lFXFileName);
	mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "ColorTech");
	mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "ColorTechWireFrame");
	mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "BuildShadowMapTech");
	mShaderObject->SetResource("Texture", ResourceLoader::GetResourceLoader()->GetBTHTexture());
}