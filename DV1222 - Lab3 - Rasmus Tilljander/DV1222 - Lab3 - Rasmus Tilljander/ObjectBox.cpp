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

//DO NOT ADD ROTATION ON lWorldMatrix
void ObjectBox::Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName)
{
	float lX, lY, lZ;

	lX = lWorldMatrix._41 + (lWorldMatrix._11 / 2);
	lY = lWorldMatrix._42 + (lWorldMatrix._22 / 2);
	lZ = lWorldMatrix._43 + (lWorldMatrix._33 / 2);

	mPosition = 
	D3DXVECTOR4(lX, lY, lZ, 1);

	mX = D3DXVECTOR3(
		lWorldMatrix._11
		,0,0);

	mY = D3DXVECTOR3(0,
		lWorldMatrix._22
		,0);

	mZ = D3DXVECTOR3(0,0,
		lWorldMatrix._33
		);

	Object::Initialize(lWorldMatrix, lFXFileName);
	mShaderObject->AddTechniqueByName(BoxVertexLayout, BoxVertexInputLayoutNumElements, "ColorTech");
	mShaderObject->AddTechniqueByName(BoxVertexLayout, BoxVertexInputLayoutNumElements, "ColorTechWireFrame");
	mShaderObject->SetResource("Texture", ResourceLoader::GetResourceLoader()->GetBoxTexture());
}

float ObjectBox::NotZero(float lFloat)
{
	if(lFloat == 0)
		return 1;
	return lFloat;
}

void ObjectBox::Update(float lDeltaTime)
{
	float lRot = 1440*5/PI;
	D3DXMATRIX lMatrix = D3DXMATRIX
		(
			cos(lRot), 0, -sin(lRot), 0,
			0,1,0,0,
			sin(lRot), 0, cos(lRot), 0,
			0,0,0,1
		);
	D3DXMatrixMultiply(&mWorldMatrix, &mWorldMatrix, &lMatrix);
}
                     