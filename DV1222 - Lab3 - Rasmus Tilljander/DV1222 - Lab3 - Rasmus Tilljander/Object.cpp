#include "Object.h"

Object::Object(ID3D10Device* lDevice, ID3D10Buffer* lVertexBuffer, int lNumberOfVertices)
{
	mShaderObject = new ShaderObject();
	mDevice = lDevice;
	mVertexBuffer = lVertexBuffer;

	D3DXMatrixIdentity(&mWorldMatrix);

	mNumberOfVertices = lNumberOfVertices;
}

Object::~Object()
{
	SAFE_RELEASE(mVertexBuffer);
	SAFE_DELETE(mShaderObject);
}

void Object::Initialize(D3DXMATRIX lWorldMatrix, char* lFXFileName)
{
	mWorldMatrix = lWorldMatrix;
	mShaderObject->Initialize(mDevice, lFXFileName, D3D10_SHADER_ENABLE_STRICTNESS);

	//mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "ColorTech");
	//mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "ColorTechWireFrame");
	//mShaderObject->AddTechniqueByName(ObjectVertexDescription, ObjectVertexInputLayoutNumElements, "BuildShadowMapTech");
	//mShaderObject->SetResource("Texture", ResourceLoader::GetResourceLoader()->GetBTHTexture());
}

void Object::Update(float lDeltaTime)
{

}

void Object::ShadowDraw(D3DXMATRIX lLightProj, D3DXMATRIX lLightView)
{
	mShaderObject->SetTechniqueByInteger(2);
	D3DXMATRIX lLightWVP;
	D3DXMatrixMultiply(&lLightWVP, &mWorldMatrix, &lLightView);
	D3DXMatrixMultiply(&lLightWVP, &lLightWVP, &lLightProj);
	mShaderObject->SetMatrix("gLightWVP", lLightWVP);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(ObjectVertex);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	D3D10_TECHNIQUE_DESC techDesc;	
	mShaderObject->GetTechnique()->GetDesc( &techDesc );

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mShaderObject->Render(p);
		mDevice->Draw(mNumberOfVertices, 0);
    }
}


void Object::Draw()
{
	mShaderObject->SetTechniqueByInteger(2);
	mShaderObject->SetMatrix("WorldMatrix", mWorldMatrix);
	mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());


	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	UINT stride = sizeof(ObjectVertex);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	
	D3D10_TECHNIQUE_DESC techDesc;
	mShaderObject->SetTechniqueByInteger(0);
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mShaderObject->Render(p);
		mDevice->Draw(mNumberOfVertices, 0);
    }
}


