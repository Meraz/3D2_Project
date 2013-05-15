#include "WorldHandler.h"

WorldHandler::WorldHandler()
: mNumRows(0), mNumCols(0), mNumVertices(0), mNumFaces(0),
  mDevice(0), mVB(0), mIB(0)
{
	mShaderObject = new ShaderObject();
	mObject = std::vector<Object*>();
	mTree = std::vector<Tree*>();
	mSoundHandler = new SoundHandler();
	
}

WorldHandler::~WorldHandler()
{
	delete mShaderObject;
	delete mSoundHandler;
	SAFE_RELEASE(mVB);
	SAFE_RELEASE(mIB);
}

void WorldHandler::ChangeTechnique(int lVariable)
{
	mShaderObject->SetTechniqueByInteger(lVariable);
}

void WorldHandler::Initialize(ID3D10Device* lDevice, UINT m, UINT n, float dx) 
{
	
	mLeft = SoundListener();
	mRight = SoundListener();
	mLeft.Initialize(GetCamera().GetPosition(), Direction::Left);
	mRight.Initialize(GetCamera().GetPosition(), Direction::Right);

	/*
		2,0.5f, 55, 1
		55,0.5f, 55, 1
		50,0.5f,2, 1
	*/
	mSoundHandler->AddSource("Sound/Sound1.wav", D3DXVECTOR4(2, 0.5, 55, 1));
	mSoundHandler->AddSource("Sound/Sound2.wav", D3DXVECTOR4(55, 0.5, 55, 1));
	mSoundHandler->AddSource("Sound/Sound3.wav", D3DXVECTOR4(50, 0.5, 2, 1));

	mDevice = lDevice;
	mShaderObject->Initialize( mDevice, "FX/Terrain.fx", D3D10_SHADER_ENABLE_STRICTNESS );
	mShaderObject->AddTechniqueByName(VertexLayout, VertexInputLayoutNumElements, "ColorTech");
	mShaderObject->AddTechniqueByName(VertexLayout, VertexInputLayoutNumElements, "ColorTechWireFrame");
	mShaderObject->AddTechniqueByName(VertexLayout, VertexInputLayoutNumElements, "BuildShadowMapTech");
	mShaderObject->AddTechniqueByName(VertexLayout, VertexInputLayoutNumElements, "RenderBillboard");
	mShaderObject->SetTechniqueByInteger(0);

	CreatObjects();
	
	SetResources();
	mNumRows  = m;
	mNumCols  = n;
	cellSpace = dx;
	mNumVertices = mNumRows*mNumCols;
	mNumFaces    = (mNumRows-1)*(mNumCols-1)*2;

	// Create the geometry and fill the vertex buffer.
	LoadHeightmap();

	int smoothLevel = 5;
	for(int i = 0; i < smoothLevel; i++)
	{
		Smooth();
	}

	std::vector<Vertex> vertices(mNumVertices);
	float halfWidth = (mNumCols-1)*dx*0.5f;
	float halfDepth = (mNumRows-1)*dx*0.5f;

	float du = 1.0f / (mNumCols-1);
	float dv = 1.0f / (mNumRows-1);

	for(UINT i = 0; i < mNumRows; ++i)
	{
		float z = halfDepth - i*dx;
		for(UINT j = 0; j < mNumCols; ++j)
		{
			float x = -halfWidth + j*dx;

			float y = mHeightmap[i*mNumCols+j];
			vertices[i*mNumCols+j].mPosition    = D3DXVECTOR4(x, y, z, 1);
			
			// Stretch texture over grid.
			vertices[i*mNumCols+j].mTexCoord.x = j*du;
			vertices[i*mNumCols+j].mTexCoord.y = i*dv;
		}
	}
 
	// Estimate normals for interior nodes using central difference.
	float invTwoDX = 1.0f / (2.0f*cellSpace);
	float invTwoDZ = 1.0f / (2.0f*cellSpace);
	for(UINT i = 2; i < mNumRows-1; ++i)
	{
		for(UINT j = 2; j < mNumCols-1; ++j)
		{
			float t = mHeightmap[(i-1)*mNumCols + j];
			float b = mHeightmap[(i+1)*mNumCols + j];
			float l = mHeightmap[i*mNumCols + j - 1];
			float r = mHeightmap[i*mNumCols + j + 1];

			D3DXVECTOR3 tanZ(0.0f, (t-b)*invTwoDZ, 1.0f);
			D3DXVECTOR3 tanX(1.0f, (r-l)*invTwoDX, 0.0f);

			D3DXVECTOR3 N;
			D3DXVec3Cross(&N, &tanZ, &tanX);
			D3DXVec3Normalize(&N, &N);
			vertices[i*mNumCols+j].mNormal = N;
		}
	}

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    mDevice->CreateBuffer(&vbd, &vinitData, &mVB);

	// Create the index buffer.  The index buffer is fixed, so we only 
	// need to create and set once.

	std::vector<DWORD> indices(mNumFaces*3); // 3 indices per face

	// Iterate over each quad and compute indices.
	int k = 0;
	for(UINT i = 0; i < m-1; ++i)
	{
		for(UINT j = 0; j < n-1; ++j)
		{
			indices[k]   = i*n+j;
			indices[k+1] = i*n+j+1;
			indices[k+2] = (i+1)*n+j;

			indices[k+3] = (i+1)*n+j;
			indices[k+4] = i*n+j+1;
			indices[k+5] = (i+1)*n+j+1;

			k += 6; // next quad
		}
	}

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    mDevice->CreateBuffer(&ibd, &iinitData, &mIB);

	CreateTrees();
}

void WorldHandler::CreatObjects()
{
	D3DXMATRIX lMatrix;
	
	mObject.push_back(ObjectFactory::GetObjectFactory()->LoadObject(mDevice, "Objects/bth.obj", LoadableObject::General));
	lMatrix = D3DXMATRIX(
		0.3,0,0,0,
		0,0.3,0,0,
		0, 0, 0.3, 0,
		30,40, 30, 1);
	mObject.at(0)->Initialize(lMatrix, "FX/Object.fx");


	mObject.push_back(ObjectFactory::GetObjectFactory()->LoadObject(mDevice, "Objects/plane.obj", LoadableObject::General));
	lMatrix = D3DXMATRIX(
		5,0,0,0,
		0,5,0,0,
		0, 0, 5, 0,
		0,0, 0, 1);
	mObject.at(1)->Initialize(lMatrix, "FX/Object.fx");

	mObject.push_back(ObjectFactory::GetObjectFactory()->LoadObject(mDevice, "Objects/Box.obj", LoadableObject::Box));
	lMatrix = D3DXMATRIX(
		1,0,0,0,
		0,1,0,0,
		0, 0, 1, 0,
		2,0.5f, 55, 1);
	mObject.at(2)->Initialize(lMatrix, "FX/Box.fx");

	mObject.push_back(ObjectFactory::GetObjectFactory()->LoadObject(mDevice, "Objects/Box.obj", LoadableObject::Box));
	lMatrix = D3DXMATRIX(
		1,0,0,0,
		0,1,0,0,
		0, 0, 1, 0,
		55,0.5f, 55, 1);
	mObject.at(3)->Initialize(lMatrix, "FX/Box.fx");

	mObject.push_back(ObjectFactory::GetObjectFactory()->LoadObject(mDevice, "Objects/Box.obj", LoadableObject::Box));
	lMatrix = D3DXMATRIX(
		1,0,0,0,
		0,1,0,0,
		0, 0, 1, 0,
		50,0.5f, 2, 1);
	mObject.at(4)->Initialize(lMatrix, "FX/Box.fx");
}

void WorldHandler::CreateTrees()
{
	srand(time(NULL));

	for(int i = 0; i < 50; i++)
	{
		float x, y, z;
		x = (rand() % 200);// - 256;
		z = (rand() % 200);// - 256;
		y = GetHeight(x, z);
		Tree* a = new Tree();
		a->Initialize(mDevice, D3DXVECTOR4(x, y, z, 1));
		mTree.push_back(a);
	}
}

void WorldHandler::SetResources()
{
	D3DXMatrixIdentity(&mWorldMatrix);
	mShaderObject->SetMatrix("WorldMatrix", mWorldMatrix);
	mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());

	ResourceLoader* lResourceLoader = ResourceLoader::GetResourceLoader();

	mShaderObject->SetResource("gLayer0", lResourceLoader->GetGrassTexture());
	mShaderObject->SetResource("gLayer1", lResourceLoader->GetStoneTexture());
	mShaderObject->SetResource("gLayer2", lResourceLoader->GetDarkGrassTexture());
	mShaderObject->SetResource("gLayer3", lResourceLoader->GetLavaTexture());
	mShaderObject->SetResource("gBlendMap", lResourceLoader->GetBlendMapTexture());
}

void WorldHandler::LoadHeightmap()
{
	// A height for each vertex
	std::vector<unsigned char> in = std::vector<unsigned char>(mNumRows * mNumCols);

	// Open the file.
	std::ifstream inFile;
	inFile.open(L"Textures/HeightMap.raw", std::ios_base::binary);

	if(inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	mHeightmap.resize(mNumRows * mNumCols, 0);
	for(UINT i = 0; i < mNumRows * mNumCols; ++i)
	{
		//mHeightmap[i] = (float)in[i] * 0.20f/*heightScale*/ + -20.0f/*heightOffset*/;
	}
}

void WorldHandler::Smooth()
{
	std::vector<float> dest( mHeightmap.size() );

	for(UINT i = 0; i < mNumRows; ++i)
	{
		for(UINT j = 0; j < mNumCols; ++j)
		{
			dest[i*mNumCols+j] = Average(i,j);
		}
	}

	// Replace the old heightmap with the filtered one.
	mHeightmap = dest;
}

bool WorldHandler::InBounds(UINT i, UINT j)
{
	// True if ij are valid indices; false otherwise.
	return 
		i >= 0 && i < mNumRows && 
		j >= 0 && j < mNumCols;
}

float WorldHandler::Average(UINT i, UINT j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for(UINT m = i-1; m <= i+1; ++m)
	{
		for(UINT n = j-1; n <= j+1; ++n)
		{
			if( InBounds(m,n) )
			{
				avg += mHeightmap[m*mNumCols + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

float WorldHandler::Width()const
{
	return (mNumCols-1)*cellSpace;
}

float WorldHandler::Depth()const
{
	return (mNumRows-1)*cellSpace;
}

float WorldHandler::GetHeight(float x, float z) const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*Width()) /  cellSpace;
	float d = (z - 0.5f*Depth()) / -cellSpace;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap[row*mNumCols + col];
	float B = mHeightmap[row*mNumCols + col + 1];
	float C = mHeightmap[(row+1)*mNumCols + col];
	float D = mHeightmap[(row+1)*mNumCols + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if( s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}

void WorldHandler::Update(float lDeltaTime)
{
	mObject.at(3)->Update(lDeltaTime);

	/* Sound */
	//Ears
	mLeft.Update(GetCamera().GetPosition());
	mRight.Update(GetCamera().GetPosition());
	
	//Play sound
	mSoundHandler->PlaySoundCustom();

	//Keep engine running
	FMODHandler::GetFMODHandler()->Update();
}


void WorldHandler::ShadowDraw( D3DXMATRIX lLightProj, D3DXMATRIX lLightView)
{
	mShaderObject->SetTechniqueByInteger(2);
	D3DXMATRIX lLightWVP; 
	D3DXMatrixMultiply(&lLightWVP, &mWorldMatrix, &lLightView);
	D3DXMatrixMultiply(&lLightWVP, &lLightWVP, &lLightProj);
	mShaderObject->SetMatrix("gLightWVP", lLightWVP);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0,1, &mVB, &stride, &offset);
	mDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	

	D3D10_TECHNIQUE_DESC techDesc;
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
	
	for( UINT p = 0; p < techDesc.Passes; p++ )
	{
		mShaderObject->Render(p);
		mDevice->DrawIndexed(mNumFaces*3,0,0);
	}

	mObject.at(0)->ShadowDraw(lLightProj, lLightView);
	mShaderObject->SetTechniqueByInteger(0);
}


void WorldHandler::Draw(D3DXVECTOR4 lSunPos,  D3DXMATRIX lLightProj, D3DXMATRIX lLightView,ID3D10ShaderResourceView* lShadowmap, bool lCreateReflectionMap)
{

	float fogStart = 100.0f;
	float fogEnd   = 150.0f;
	mShaderObject->SetFloat("fogStart", fogStart);
	mShaderObject->SetFloat("fogEnd", fogEnd);
	mShaderObject->SetMatrix("WorldMatrix", mWorldMatrix);
	if(lCreateReflectionMap == false)
		mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	else
		mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetReflectionViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());
	D3DXMATRIX lLightWVP; 
	D3DXMatrixMultiply(&lLightWVP, &mWorldMatrix, &lLightView);
	D3DXMatrixMultiply(&lLightWVP, &lLightWVP, &lLightProj);
	mShaderObject->SetMatrix("gLightWVP", lLightWVP);
	mShaderObject->SetFloat4("Sunpos", &lSunPos);
	mShaderObject->SetResource("gShadowMap", lShadowmap);
	mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	mDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	mDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	D3D10_TECHNIQUE_DESC techDesc;
	mShaderObject->GetTechnique()->GetDesc( &techDesc );
    for(UINT p = 0; p < techDesc.Passes; p++)
    {
		mShaderObject->Render(p);
		mDevice->DrawIndexed(mNumFaces*3, 0, 0);
    }


	mObject.at(0)->Draw(lSunPos, lLightProj,  lLightView, lShadowmap);
	//mObject.at(1)->Draw(lSunPos, lLightProj,  lLightView, lShadowmap);
	mObject.at(2)->Draw(lSunPos, lLightProj,  lLightView, lShadowmap);
	mObject.at(3)->Draw(lSunPos, lLightProj,  lLightView, lShadowmap);
	mObject.at(4)->Draw(lSunPos, lLightProj,  lLightView, lShadowmap);


	for(int i = 0; i < 50; i++)
	{
		mTree.at(i)->Draw();
	}
	if(lCreateReflectionMap == false)
	{
		//Rita ut shadowmap texturen i högra hörnet
		mShaderObject->SetTechniqueByInteger(3);
		mDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
		mShaderObject->Render(0);
		mDevice->DrawIndexed(1, 0, 0);
	}
}
