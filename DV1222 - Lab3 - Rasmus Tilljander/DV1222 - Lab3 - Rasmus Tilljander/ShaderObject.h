#ifndef ShaderObject_H
#define ShaderObject_H

#include "Utilities.h"
#include <string>
#include <vector>

class ShaderObject
{

public:
	ShaderObject();
	~ShaderObject();

	HRESULT Initialize(ID3D10Device* lDevice, char* lFilename, DWORD lShaderFlags);
	int AddTechniqueByName(const D3D10_INPUT_ELEMENT_DESC* lInputDesc, UINT lNumberOfElements, char* lTechniqueName);


	void Render(UINT lPass);
	void SetMatrix(char* lVariable, D3DXMATRIX& mD3DXMatrix);
	void SetFloat(char* lVariable, float lFloat);
	void SetFloat3(char* lVariable, D3DXVECTOR3 lVector);
	void SetFloat4(char* lVariable, D3DXVECTOR4* lVector);
	void Setint(char* lVariable, int lInt);
	void SetBool(char* lVariable, bool lBool);
	void SetRawData(char* lVariable, void* lData, size_t lSize);
	void SetResource(char* lVariable, ID3D10ShaderResourceView* lShaderResourceView);
	void SetTechniqueByInteger(int lVariable);

	
public: //Get Functions
	ID3D10EffectTechnique* GetTechnique() const
	{
		return mEffectTechnique.at(mCurrentEffectTechnique);
	}
	ID3D10EffectTechnique* GetTechniqueByName(char* lVariable) const
	{
		return mEffect->GetTechniqueByName(lVariable);
	}

	int GetCurrentTechnique()
	{
		return mCurrentEffectTechnique;
	}
	
private:
	ID3D10Device* mDevice;
	ID3D10Effect* mEffect;
	std::vector<ID3D10EffectTechnique*> mEffectTechnique;
	std::vector<ID3D10InputLayout*> mInputLayout;
	int mCurrentEffectTechnique;
};

#endif
