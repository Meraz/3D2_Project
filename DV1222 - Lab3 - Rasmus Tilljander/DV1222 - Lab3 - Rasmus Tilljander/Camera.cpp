#include "Camera.h"

Camera& GetCamera()
{
	static Camera camera;
	return camera;
}

Camera::Camera()
{
	mPosition	= D3DXVECTOR3(-0.0f, 0.0f, 0.0f);
	mUp			= D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	mAim		= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mDirection	= D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	mCameraAngleXY = 0.0;		//0-90	
	mCameraAngleXZ = 0.0;		//0-360

	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
}

Camera::~Camera()
{

}

void Camera::SetPosition(D3DXVECTOR3 lPosition)
{
	mPosition = lPosition;
}


void Camera::SetYPosition(float lY)
{
	mPosition.y = lY;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	D3DXMatrixPerspectiveFovLH(&mProj, fovY, aspect, zn, zf);
}

void Camera::Strafe(float d)
{
	Update();
	D3DXVECTOR3 lVector = RotateY(mDirection, (PI/2));
	mPosition += lVector * d;
}

void Camera::Walk(float d)
{
	Update();
	mPosition += mDirection * d;
}

void Camera::Update()
{
	//Aim
	D3DXVECTOR3 lAim = D3DXVECTOR3(1,0,0);
	lAim = RotateZ(lAim, mCameraAngleXY*PI/180);
	D3DXVec3Normalize(&lAim, &lAim);
	lAim = RotateY(lAim, mCameraAngleXZ*PI/180);
	D3DXVec3Normalize(&lAim, &lAim);
	lAim *= 100;
	D3DXVECTOR3 lPosition = mPosition;
	lPosition.y = 0;
	mAim = Translation(lAim, lPosition);

	//Direction
	D3DXVec3Normalize(&lAim, &lAim);
	//lAim.y = 0;
	mDirection = lAim;
}

D3DXVECTOR3 Camera::RotateY(D3DXVECTOR3 lVector, float lAngleInPI)
{
	float a = lAngleInPI;
	D3DXMATRIX lMatrix = D3DXMATRIX(
		cos(a),		0,		-sin(a),	0,
		0,			1,		0,			0,
		sin(a),	0,		cos(a),		0,
		0,			0,		0,			1
		);													
	D3DXVECTOR3 ReturnVector = Multiply(lVector, lMatrix);
	return ReturnVector;
}

D3DXVECTOR3 Camera::RotateZ(D3DXVECTOR3 lVector, float lAngleInPI)
{
	float a = lAngleInPI;
	D3DXMATRIX lMatrix = D3DXMATRIX(
		cos(a),		sin(a),		0,		0,
		-sin(a),	cos(a),		0,		0,
		0,			0,			1,		0,
		0,			0,			0,		1
		);													
	D3DXVECTOR3 ReturnVector = Multiply(lVector, lMatrix);
	return ReturnVector;
}

D3DXVECTOR3 Camera::Translation(D3DXVECTOR3 lVector, D3DXVECTOR3 lTranslationVector)
{
	D3DXMATRIX lMatrix = D3DXMATRIX(
		1,		0,		0,		0,
		0,		1,		0,		0,
		0,		0,		1,		0,
		lTranslationVector.x,		lTranslationVector.y,		lTranslationVector.z,		1
		);
	D3DXVECTOR4 lV;
	lV.x = lVector.x;
	lV.y = lVector.y;
	lV.z = lVector.z;
	lV.w = 1;

	lV = Multiply16(lV, lMatrix);
	
	D3DXVECTOR3 lV2;
	lV2.x = lV.x;
	lV2.y = lV.y;
	lV2.z = lV.z;
	return lV2;
}

D3DXVECTOR3 Camera::Multiply(D3DXVECTOR3 v, D3DXMATRIX& m)
{
	D3DXVECTOR3 temp;
	temp.x = v.x*m._11 + v.y*m._21 + v.z*m._31;
	temp.y = v.x*m._12 + v.y*m._22 + v.z*m._32;
	temp.z = v.x*m._13 + v.y*m._23 + v.z*m._33;

	return temp;
}

D3DXVECTOR4 Camera::Multiply16(D3DXVECTOR4 v, D3DXMATRIX& m)
{
	D3DXVECTOR4 temp;
	temp.x = v.x*m._11 + v.y*m._21 + v.z*m._31 + v.w*m._41;
	temp.y = v.x*m._12 + v.y*m._22 + v.z*m._32 + v.w*m._42;
	temp.z = v.x*m._13 + v.y*m._23 + v.z*m._33 + v.w*m._43;
	temp.w = v.x*m._14 + v.y*m._24 + v.z*m._34 + v.w*m._44;
	return temp;
}

void Camera::Pitch(float lAngle)
{
	mCameraAngleXY += lAngle;
	if(mCameraAngleXY > 60)
		mCameraAngleXY = 60;
	else if(mCameraAngleXY < -60)
		mCameraAngleXY = -60;
	Update();
	D3DXMatrixLookAtLH(&mView, &mPosition, &mAim, &mUp);
}

void Camera::Yaw(float lAngle)
{
	mCameraAngleXZ += lAngle;
	if(mCameraAngleXZ > 360)
		mCameraAngleXZ = 0;
	if(mCameraAngleXZ < 0)
		mCameraAngleXZ = 360;
	Update();
	D3DXMatrixLookAtLH(&mView, &mPosition, &mAim, &mUp);
}

void Camera::RebuildView()
{
	D3DXMatrixLookAtLH(&mView, &mPosition, &mAim, &mUp);
}


void Camera::SetAim(D3DXVECTOR3 lAim)
{
	mAim = lAim;
}


void Camera::BuildViewReflection(float lOffset)
{


	//all hail www.rastertek.com/dx10tut27.html
	D3DXVECTOR3 lUp, lPos, lLookat;
	float lRadians;

	//up vektorn
	lUp = D3DXVECTOR3(0.0f, 1.0f,0.0f);

	//Eftersom den ska reflektera i z-led så gör vi som vi gör med z axeln. (inverterar och  gör nåt coolt med offseten(läs på detta din n00b!) 

	lPos.x = mPosition.x;
	lPos.y = mPosition.y;
	lPos.z = -mPosition.z + (lOffset * 2.0f);

	//gör om rotationen till radianer
	lRadians = mCameraAngleXZ *  0.0174532925f;

	lLookat.x = sinf(lRadians) + mPosition.x;
	lLookat.y = mPosition.y;
	lLookat.z = cosf(lRadians) + mPosition.z;

	D3DXMatrixLookAtLH(&mReflectionView, &lPos, &lLookat, &lUp);


}