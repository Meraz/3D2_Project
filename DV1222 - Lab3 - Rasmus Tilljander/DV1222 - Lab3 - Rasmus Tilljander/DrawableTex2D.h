#ifndef DRAWABLETEX2D_H
#define DRAWABLETEX2D_H
#include "Utilities.h"

class DrawableTex2D
{
public:
	DrawableTex2D();
	~DrawableTex2D();

	void Initialize(ID3D10Device* lDevice, UINT lWidth, UINT lHeight, bool lHasColorMap, DXGI_FORMAT lColorMapFormat);

	ID3D10ShaderResourceView* GetColorMap() const { return mColorMapSRV;};
	ID3D10ShaderResourceView* GetDepthMap() const { return mDepthMapSRV;};
	void Begin();
	void End();

private:
	DrawableTex2D(const DrawableTex2D& lRhs);
	DrawableTex2D& operator =(const DrawableTex2D& lRhs);

	void BuildColorMap();
	void BuildDepthMap();

public:
private:
	UINT mWidth;
	UINT mHeight;
	DXGI_FORMAT mColorMapFormat;
	ID3D10Device* mDevice;
	ID3D10ShaderResourceView* mColorMapSRV;
	ID3D10RenderTargetView* mColorMapRTV;
	ID3D10ShaderResourceView* mDepthMapSRV;
	ID3D10DepthStencilView* mDepthMapDSV;
	D3D10_VIEWPORT mViewport;
};

#endif