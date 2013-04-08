#include "ResourceLoader.h"

ResourceLoader* ResourceLoader::mResourceLoader = 0;

ResourceLoader* ResourceLoader::GetResourceLoader()
{
	if(mResourceLoader == 0)
		mResourceLoader = new ResourceLoader();
	return mResourceLoader;
}

ResourceLoader::ResourceLoader()
{
	
}

ResourceLoader::~ResourceLoader()
{

}

void ResourceLoader::Initialize(ID3D10Device* lDevice)
{
	LoadTextures(lDevice);
}

//Add memory management here
 
void ResourceLoader::LoadTextures(ID3D10Device* lDevice)
{
	CreateTextureShader(lDevice, "Textures/BlendMap.dds" , &mBlendMapTexture);
	CreateTextureShader(lDevice, "Textures/DarkGrass.dds" , &mDarkGrassTexture);
	CreateTextureShader(lDevice, "Textures/FlameTexture.dds" , &mFlameTexture);
	CreateTextureShader(lDevice, "Textures/Grass.dds" , &mGrassTexture);
	CreateTextureShader(lDevice, "Textures/Lava.dds" , &mLavaTexture);
	CreateTextureShader(lDevice, "Textures/ParticleTexture.dds" , &mParticleTexture);
	CreateTextureShader(lDevice, "Textures/Stone.dds" , &mStoneTexture);
	CreateTextureShader(lDevice, "Textures/Sun.dds" , &mSunTexture);
	CreateTextureShader(lDevice, "Textures/BTH.dds" , &mBTHTexture);
	CreateTextureShader(lDevice, "Textures/Tree.png" , &mTreeTexture);
	CreateTextureShader(lDevice, "Textures/Box.png" , &mBoxTexture);
}

void ResourceLoader::CreateTextureShader(ID3D10Device* lDevice, char* lTexturePath, ID3D10ShaderResourceView** lShaderResView)
{
	HRESULT h = D3DX10CreateShaderResourceViewFromFile(lDevice, lTexturePath,
														NULL, NULL,
														lShaderResView,
														NULL);
	if (FAILED(h))
	{
		MessageBox(0, lTexturePath, "Missing Texture", 0);
	}
}