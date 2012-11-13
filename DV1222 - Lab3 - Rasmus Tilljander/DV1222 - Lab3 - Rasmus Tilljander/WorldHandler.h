#ifndef WORLDHANDLER_H
#define WORLDHANDLER_H

#include <vector>
#include <string>
#include <fstream>
#include "Utilities.h"
#include "ShaderObject.h"
#include "Vertex.h"
#include "Camera.h"
#include "ResourceLoader.h"
#include "ObjectLoader.h"

class WorldHandler
{
public:
	WorldHandler();
	virtual ~WorldHandler();
	void Initialize(ID3D10Device* lDevice, UINT m, UINT n, float dx);
	void Update(float lDeltaTime);
	void Draw();

	float Width() const;
	float Depth() const;
	float GetHeight(float x, float z) const;
	void ChangeTechnique(int lInt);

private:		
	void SetResources();

	void LoadHeightmap();
	void Smooth();

	bool	InBounds(UINT i, UINT j);
	float	Average(UINT i, UINT j);

private:

	ID3D10Device* mDevice;
	ShaderObject* mShaderObject;
	D3DXMATRIX mWorldMatrix;
	std::vector<Object*> mObject;

	ID3D10ShaderResourceView* CreateTex(std::wstring filename);

	UINT mNumRows;
	UINT mNumCols;
	float cellSpace;
	UINT mNumVertices;
	UINT mNumFaces;

	std::vector<float> mHeightmap;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	ID3D10InputLayout* mVertexLayout;
	ID3D10ShaderResourceView* mLayer0;
	ID3D10ShaderResourceView* mLayer1;
	ID3D10ShaderResourceView* mLayer2;
	ID3D10ShaderResourceView* mLayer3;
	
	ID3D10ShaderResourceView* mBlendMap;
};

#endif
