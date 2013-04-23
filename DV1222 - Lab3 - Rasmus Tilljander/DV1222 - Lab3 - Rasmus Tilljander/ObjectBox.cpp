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

//DO NOT ADD ROTATION ON lWorldMatrix before init
void ObjectBox::Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName)
{
	mPosition = D3DXVECTOR4(lWorldMatrix._41, lWorldMatrix._42, lWorldMatrix._43, 1);

	mX = D3DXVECTOR3(
		1
		,0,0);

	mY = D3DXVECTOR3(0,
		1
		,0);

	mZ = D3DXVECTOR3(0,0,
		1
		);

	mAxisLength = D3DXVECTOR3(mX.x/2*lWorldMatrix._11, mY.y/2*lWorldMatrix._22, mZ.z/2*lWorldMatrix._33);
//	D3DXVec4Transform();

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
	//float lRot = (PI*5)/360;
	//D3DXMATRIX lMatrix = D3DXMATRIX
	//	(
	//		cos(lRot), 0, -sin(lRot), 0,
	//		0,1,0,0,
	//		sin(lRot), 0, cos(lRot), 0,
	//		0,0,0,1
	//	);
	//D3DXMatrixMultiply(&mWorldMatrix, &mWorldMatrix, &lMatrix);

	if(Collision())
		mShaderObject->SetFloat("c", 1);
	else
		mShaderObject->SetFloat("c", 0);

}       

void ObjectBox::Draw(D3DXVECTOR4 lSunPos,D3DXMATRIX lLightProj, D3DXMATRIX lLightView,ID3D10ShaderResourceView* lShadowmap)
{
	//mShaderObject->SetFloat("c", 1);
	Object::Draw(lSunPos, lLightProj, lLightView, lShadowmap);
}

bool ObjectBox::Collision()
{
	D3DXVECTOR3 lRayOrin = GetCamera().GetPosition();
	D3DXVECTOR3 lRayDirection = GetCamera().GetAim();	
	D3DXVec3Normalize(&lRayDirection, &lRayDirection);

	double tmin = -900000;
	double tmax =  900000;
	float smallNumber = 0.00000001;
	D3DXVECTOR3 p = D3DXVECTOR3(mPosition) - lRayOrin;
	
	D3DXVECTOR3 lArray[3];
	lArray[0] = mX;
	lArray[1] = mY;
	lArray[2] = mZ;
	float lFloatArray[3];
	lFloatArray[0] = mAxisLength.x;
	lFloatArray[1] = mAxisLength.y;
	lFloatArray[2] = mAxisLength.z;
	float e, f;

	for(int i = 0; i < 3; i++)
	{
		e = D3DXVec3Dot(&lArray[i], &p);
		f = D3DXVec3Dot(&lArray[i], &lRayDirection);
		if(abs(f) > smallNumber)
		{
			float t1 = (e + lFloatArray[i]) / f;
			float t2 = (e - lFloatArray[i]) / f;

			if(t1 > t2) //10
				std::swap(t1, t2);
			if(t1 > tmin) //11
				tmin = t1;
			if(t2 < tmax) //12
				tmax = t2;
			if(tmin > tmax)
				return false;
			if(tmax < 0)
				return false;
		}
		else if(-e - lFloatArray[i] > 0 || - e + lFloatArray[i] < 0)
			return false;
	}		
	if(tmin > 0)
		{
			return true; //tmin
		}
	return true; //tmax
}