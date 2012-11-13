#include "ShaderObject.h"


ShaderObject::ShaderObject():	
mEffect(NULL), mEffectTechnique(NULL), mInputLayout(NULL)
{
	mEffectTechnique = std::vector<ID3D10EffectTechnique*>();
	mEffectTechnique.resize(1, 0);
	mInputLayout = std::vector<ID3D10InputLayout*>();
	mInputLayout.resize(1, 0);
	mCurrentEffectTechnique = 0;
}

ShaderObject::~ShaderObject()
{
	for(UINT i = 0; i < mInputLayout.size(); i++) 
		SAFE_RELEASE(mInputLayout.at(i));
	SAFE_RELEASE(mEffect);
}

HRESULT ShaderObject::Initialize(ID3D10Device* lDevice, char* lFilename, DWORD lShaderFlags) 
{
	mDevice = lDevice;

	HRESULT hr = S_OK;

	ID3DBlob*	pBlobEffect = NULL;
	ID3DBlob*	pBlobErrors = NULL;
	ID3D10Blob* compilationErrors = 0;
	

#if defined(DEBUG) || defined(_DEBUG)
	//dwShaderFlags |= D3D10_SHADER_DEBUG;
	lShaderFlags |= D3D10_SHADER_DEBUG;
	//shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	
	if(FAILED(D3DX10CreateEffectFromFile(lFilename,
											NULL,
											NULL,
											"fx_4_0",
											lShaderFlags,
											0,
											mDevice,
											NULL,
											NULL,
											&mEffect,
											&compilationErrors,
											NULL)))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			compilationErrors->Release();
		}
	}

	return hr;
}

int ShaderObject::AddTechniqueByName(const D3D10_INPUT_ELEMENT_DESC* lInputDesc, UINT lNumberOfElements, char* lTechniqueName)
{
	mEffectTechnique[mEffectTechnique.size()-1] = mEffect->GetTechniqueByName(lTechniqueName);
	ID3D10InputLayout* lInputLayout;

	if(lInputDesc)
	{
		D3D10_PASS_DESC PassDesc;
		mEffectTechnique[mEffectTechnique.size()-1]->GetPassByIndex(0)->GetDesc(&PassDesc);
		HRESULT hr = S_OK;
		if(FAILED(hr = mDevice->CreateInputLayout(
			lInputDesc,
			lNumberOfElements,
			PassDesc.pIAInputSignature,
			PassDesc.IAInputSignatureSize,
			&lInputLayout
			)))
		{
			MessageBox(0, "Cannot create input layout.", "CreateInputLayout error", MB_OK | MB_ICONERROR);
		}
	}
	mInputLayout[mInputLayout.size()-1] = lInputLayout;

	mInputLayout.resize(mInputLayout.size()+1, 0);
	mEffectTechnique.resize(mEffectTechnique.size()+1, 0);
	return mEffectTechnique.size()-2;
}

void ShaderObject::Render(UINT lPass) 
{
	ID3D10EffectPass* p = mEffectTechnique[mCurrentEffectTechnique]->GetPassByIndex(lPass);
	if(p)
	{
		mEffectTechnique[mCurrentEffectTechnique]->GetPassByIndex(lPass)->Apply(0);

		if(mInputLayout[mCurrentEffectTechnique])
		{
			mDevice->IASetInputLayout(mInputLayout[mCurrentEffectTechnique]);
		}
	}
}

void ShaderObject::SetMatrix(char* lVariable, D3DXMATRIX& mD3DXMatrix) 
{
	mEffect->GetVariableByName(lVariable)->AsMatrix()->SetMatrix((float*)&mD3DXMatrix);
}

void ShaderObject::SetFloat(char* lVariable, float lFloat) 
{
	mEffect->GetVariableByName(lVariable)->AsScalar()->SetFloat(lFloat);
}

void ShaderObject::SetFloat3(char* lVariable, D3DXVECTOR3 lVector) 
{
	mEffect->GetVariableByName(lVariable)->AsVector()->SetFloatVector((float*)&lVector);
}

void ShaderObject::SetFloat4(char* lVariable, D3DXVECTOR4 lVector) 
{
	mEffect->GetVariableByName(lVariable)->AsVector()->SetFloatVector((float*)&lVector);
}

void ShaderObject::Setint(char* lVariable, int lInt) 
{
	mEffect->GetVariableByName(lVariable)->AsScalar()->SetInt(lInt);
}

void ShaderObject::SetBool(char* lVariable, bool lBool) 
{
	mEffect->GetVariableByName(lVariable)->AsScalar()->SetBool(lBool);
}

void ShaderObject::SetRawData(char* lVariable, void* lData, size_t lSize) 
{
	mEffect->GetVariableByName(lVariable)->SetRawValue(lData, 0, (UINT)lSize);
}

void ShaderObject::SetResource(char* lVariable, ID3D10ShaderResourceView* lShaderResourceView) 
{
	mEffect->GetVariableByName(lVariable)->AsShaderResource()->SetResource(lShaderResourceView);
}

void ShaderObject::SetTechniqueByInteger(int lVariable) 
{
	mCurrentEffectTechnique = lVariable;
}	