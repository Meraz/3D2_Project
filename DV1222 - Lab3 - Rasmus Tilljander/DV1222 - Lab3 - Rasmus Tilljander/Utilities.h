#ifndef UTILITIES_H
#define UTILITIES_H

#include <D3DX10.h>
#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = NULL; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = NULL; }
#define PI (3.14159265358979323846f)
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.01745329251994329576923690768489

//ParticleSystem variables
#define AmountOfParticleSystems 2

//Camera Specific Things
#define FieldOfView 45
#define ScreenAspect (float)1024/768
#define NearPlane 0.1f
#define FarPlane 2000.0f
#define ScreenWidth 1024
#define ScreenHeight 768
#define HeightOffset 2

//Game Specific Things
#define gPlayerMovementSpeed 24

template<typename T>
D3DX10INLINE T Min(const T& a, const T& b)
{
	return a < b ? a : b;
};

// Returns random float in [0, 1).
D3DX10INLINE float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

// Returns random float in [a, b).
D3DX10INLINE float RandF(float a, float b)
{
	return a + RandF()*(b-a);
}

#endif
