#include "Tree.h"

Tree::Tree()
{

}

Tree::~Tree()
{

}

void Tree::Initialize(ID3D10Device* lDevice, D3DXVECTOR4 lPosition)
{
	mDevice = lDevice;
	mShaderObject->Initialize(lDevice, "FX/Tree.fx", D3D10_SHADER_ENABLE_STRICTNESS);
	mShaderObject->AddTechniqueByName(BillboardVertexDescription, BillboardVertexInputLayoutNumElements, "DrawTech");
	mShaderObject->SetResource("tex2D", GetResourceLoader().GetTreeTexture());
	mShaderObject->SetTechniqueByInteger(0);
	CreateVertexBuffer(lDevice,  &mVertexBuffer, 1, lPosition);

	D3DXMatrixIdentity(&mWorldMatrix);
}

void Tree::CreateVertexBuffer(ID3D10Device* lDevice, ID3D10Buffer** lVB, int lSize, D3DXVECTOR4 lPosition)
{	
	ID3D10Buffer* lVBuffer = 0;

	BillboardVertex temp;
	temp.mPosition = lPosition;
	temp.mSize = D3DXVECTOR2(400, 237);


	// Create the buffer to kick-off the particle system.
	D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(BillboardVertex) * lSize;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &temp;
	lDevice->CreateBuffer(&vbd, &vinitData, &lVBuffer);


	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof( BillboardVertex ) * lSize;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;

	mDevice->CreateBuffer( &bd, 0, lVB );
}



void Tree::Draw()
{
	D3DXMATRIXA16 a = GetCamera().GetViewMatrix() * GetCamera().GetProjectionMatrix();
	mShaderObject->SetMatrix("viewProj", a);
	mShaderObject->SetFloat3("eyePosW", GetCamera().GetPosition());
	

	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	UINT stride = sizeof(BillboardVertex);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	D3D10_TECHNIQUE_DESC techDesc;
	
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		mShaderObject->Render(p);
		mDevice->Draw(1,0);
    }
}