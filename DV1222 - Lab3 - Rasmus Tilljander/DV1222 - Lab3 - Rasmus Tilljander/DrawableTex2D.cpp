#include "DrawableTex2D.h"

DrawableTex2D::DrawableTex2D(): mWidth(0), mHeight(0), mColorMapFormat(DXGI_FORMAT_UNKNOWN),
								mDevice(0), mColorMapSRV(0), mColorMapRTV(0), mDepthMapSRV(0),
								mDepthMapDSV(0)
{
	ZeroMemory(&mViewport, sizeof(D3D10_VIEWPORT));
}
DrawableTex2D::~DrawableTex2D()
{
	SAFE_RELEASE(mColorMapSRV);
	SAFE_RELEASE(mColorMapRTV);
	SAFE_RELEASE(mDepthMapSRV);
	SAFE_RELEASE(mDepthMapDSV);
}

void DrawableTex2D::Initialize(ID3D10Device* lDevice, UINT lWidth, UINT lHeight, bool lHasColorMap, DXGI_FORMAT lColorMapFormat)
{
	mWidth = lWidth;
	mHeight = lHeight;
	mDevice = lDevice;
	mColorMapFormat = lColorMapFormat;

	BuildDepthMap();

	if(lHasColorMap)
		BuildColorMap();
	mViewport.TopLeftX		= 0;
	mViewport.TopLeftY		= 0;
	mViewport.Width			= mWidth;
	mViewport.Height		= mHeight;
	mViewport.MinDepth		= 0.0f;
	mViewport.MaxDepth		= 1.0f;

}

#pragma region Build depth and color maps
void DrawableTex2D::BuildDepthMap()
{
	ID3D10Texture2D* lDepthMap = NULL;

	D3D10_TEXTURE2D_DESC texDesc;

	texDesc.Width				= mWidth;
	texDesc.Height				= mHeight;
	texDesc.MipLevels			= 1;
	texDesc.ArraySize			= 1;
	texDesc.Format				= DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count	= 1;
	texDesc.SampleDesc.Quality	= 0;
	texDesc.Usage				= D3D10_USAGE_DEFAULT;
	texDesc.BindFlags			= D3D10_BIND_DEPTH_STENCIL | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags		= 0;
	texDesc.MiscFlags			= 0;

	if(FAILED(mDevice->CreateTexture2D(&texDesc, 0, &lDepthMap)))
		MessageBox(0, "Error Creating DepthMap (DrawableTex2D", "Error", 0);

	D3D10_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format				= DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension		= D3D10_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice	= 0;
	
	if(FAILED(mDevice->CreateDepthStencilView(lDepthMap, &dsvDesc, &mDepthMapDSV)))
		MessageBox(0, "Error creating stencil view (drawabletex2d)", "Error" , 0);
	
	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	if(FAILED(mDevice->CreateShaderResourceView(lDepthMap, &srvDesc, &mDepthMapSRV)))
		MessageBox(0, "Error creating resource view (drawabletex2d)" , "Error" , 0);

	SAFE_RELEASE(lDepthMap);

}

void DrawableTex2D::BuildColorMap()
{
	ID3D10Texture2D* lColorMap = 0;
	D3D10_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = mColorMapFormat;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags = D3D10_BIND_RENDER_TARGET |
	D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D10_RESOURCE_MISC_GENERATE_MIPS;
	if(FAILED(mDevice->CreateTexture2D(&texDesc, 0, &lColorMap)))
		MessageBox(0, "Error creating colormap tex (drawabletex2d)", "error", 0);


	// Null description means to create a view to all mipmap levels
	// using the format the texture was created with.
	if(FAILED(mDevice->CreateRenderTargetView(lColorMap, 0, &mColorMapRTV)))
		MessageBox(0, "error creating colormap RTV" , "error", 0);
	if(FAILED(mDevice->CreateShaderResourceView(lColorMap, 0, &mColorMapSRV)))
		MessageBox(0, "error creating colormap RV" , "error", 0);
	// View saves a reference to the texture so we can
	// release our reference.
	SAFE_RELEASE(lColorMap);
}

#pragma endregion

void DrawableTex2D::Begin()
{
	ID3D10RenderTargetView* lRenderTargets[1] = {mColorMapRTV};
	mDevice->OMSetRenderTargets(1, lRenderTargets, mDepthMapDSV);
	mDevice->RSSetViewports(1, &mViewport);

	if(mColorMapRTV)
		mDevice->ClearRenderTargetView(mColorMapRTV, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f));

	mDevice->ClearDepthStencilView(mDepthMapDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);	
}


void DrawableTex2D::End()
{
	if(mColorMapSRV)
		mDevice->GenerateMips(mColorMapSRV);
}