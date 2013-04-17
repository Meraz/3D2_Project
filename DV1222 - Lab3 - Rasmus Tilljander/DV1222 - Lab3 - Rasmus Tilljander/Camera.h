#ifndef CAMERA_H
#define CAMERA_H

#include "Utilities.h"

class Camera
{
public:
	Camera();
	~Camera();
	
	void SetLens(float fovY, float aspect, float zn, float zf);

	void Strafe(float d);
	void Walk(float d);

	
	void SetYPosition(float lY);
	void Pitch(float lAngle);
	void Yaw(float lAngle);
	void RebuildView();
	void SetPosition(D3DXVECTOR3 lPosition);

	//Move down again
	float mCameraAngleXY;	
	float mCameraAngleXZ;
	void Update();

	void BuildViewReflection(float lOffset);
	void SetAim(D3DXVECTOR3 lAim);

public:
	D3DXVECTOR3		GetPosition()		 const { return mPosition; }
	D3DXMATRIX		GetViewMatrix()		 const { return mView; }
	D3DXMATRIX		GetReflectionViewMatrix() const {return mReflectionView;};
	D3DXMATRIX		GetProjectionMatrix() const { return mProj; }
	D3DXVECTOR3 RotateY(D3DXVECTOR3 lVector, float lAngleInPI);
	D3DXVECTOR3 RotateZ(D3DXVECTOR3 lVector, float lAngleInPI);


private:
	D3DXVECTOR3 Translation(D3DXVECTOR3 lVector, D3DXVECTOR3 lTranslationVector);
	D3DXVECTOR3 Multiply(D3DXVECTOR3 v, D3DXMATRIX& m);
	D3DXVECTOR4 Multiply16(D3DXVECTOR4 v, D3DXMATRIX& m);

private:
	D3DXVECTOR3 mPosition;
	D3DXVECTOR3 mUp;
	D3DXVECTOR3 mAim;
	D3DXVECTOR3 mDirection;

	D3DXMATRIX mReflectionView;
	D3DXMATRIX mView;
	D3DXMATRIX mProj;
};

Camera& GetCamera();

#endif