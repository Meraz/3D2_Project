#include "WorldHandler.h"

WorldHandler::WorldHandler()
: mNumRows(0), mNumCols(0), mNumVertices(0), mNumFaces(0),
  mDevice(0), mVB(0), mIB(0)
{
	mShaderObject = new ShaderObject();
	mObject = std::vector<Object*>();
}

WorldHandler::~WorldHandler()
{
	delete mShaderObject;
	SAFE_RELEASE(mVB);
	SAFE_RELEASE(mIB);
}

void WorldHandler::ChangeTechnique(int lVariable)
{
	mShaderObject->SetTechniqueByInteger(lVariable);
}

void WorldHandler::Initialize(ID3D10Device* lDevice, UINT m, UINT n, float dx) 
{
	mDevice = lDevice;

	mShaderObject->Initialize( mDevice, "Terrain.fx", D3D10_SHADER_ENABLE_STRICTNESS );
	mShaderObject->AddTechniqueByName(VertexLayout, VertexInputLayoutNumElements, "ColorTech");
	mShaderObject->AddTechniqueByName(VertexLayout, VertexInputLayoutNumElements, "ColorTechWireFrame");
	mShaderObject->SetTechniqueByInteger(0);

	mObject.push_back(GetObjectLoader().LoadObject(mDevice, "bth.obj", "Object.fx"));
	
	SetResources();

	mNumRows  = m;
	mNumCols  = n;
	cellSpace = dx;
	mNumVertices = mNumRows*mNumCols;
	mNumFaces    = (mNumRows-1)*(mNumCols-1)*2;
	SetResources();

	// Create the geometry and fill the vertex buffer. 
	LoadHeightmap();

	int smoothLevel = 10;
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
}

void WorldHandler::SetResources()
{
	D3DXMatrixIdentity(&mWorldMatrix);
	mShaderObject->SetMatrix("WorldMatrix", mWorldMatrix);
	mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());

	mShaderObject->SetResource("gLayer0", GetResourceLoader().GetGrassTexture());
	mShaderObject->SetResource("gLayer1", GetResourceLoader().GetStoneTexture());
	mShaderObject->SetResource("gLayer2", GetResourceLoader().GetDarkGrassTexture());
	mShaderObject->SetResource("gLayer3", GetResourceLoader().GetLavaTexture());
	mShaderObject->SetResource("gBlendMap", GetResourceLoader().GetBlendMapTexture());
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
		mHeightmap[i] = (float)in[i] * 0.20f/*heightScale*/ + -20.0f/*heightOffset*/;
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
	mObject.at(0)->Update(lDeltaTime);
}

void WorldHandler::Draw()
{
	mShaderObject->SetMatrix("WorldMatrix", mWorldMatrix);
	mShaderObject->SetMatrix("ViewMatrix", GetCamera().GetViewMatrix());
	mShaderObject->SetMatrix("ProjectionMatrix", GetCamera().GetProjectionMatrix());
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

	mObject.at(0)->Draw();
}