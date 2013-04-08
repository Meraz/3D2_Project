#include "ObjectBox.h"


ObjectBox::ObjectBox(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, int lNumberOfVertices)
	: Object(lDevice, lVertexBuffer, lNumberOfVertices)
{
	mShaderObject = new ShaderObject();
	mDevice = lDevice;
	mVertexBuffer = lVertexBuffer;
	
	D3DXMatrixIdentity(&mWorldMatrix);

	mNumberOfVertices = lNumberOfVertices;
}

ObjectBox::~ObjectBox()
{

}

void ObjectBox::Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName)
{
	Object::Initialize(lWorldMatrix, lFXFileName);
	mShaderObject->SetResource("Texture", ResourceLoader::GetResourceLoader()->GetBoxTexture());
	mShaderObject->AddTechniqueByName(BoxVertexLayout, BoxVertexInputLayoutNumElements, "ColorTech");
	mShaderObject->AddTechniqueByName(BoxVertexLayout, BoxVertexInputLayoutNumElements, "ColorTechWireFrame");
}
