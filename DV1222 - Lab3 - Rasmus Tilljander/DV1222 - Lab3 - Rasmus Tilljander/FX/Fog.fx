
cbuffer cbEveryFrame
{
	float fogStart,fogEnd;
	float fogFactor;
};
static const float4 fogColor = float4(0.5f,0.5f,0.5f,1.0f);
static const float fogDensity = 0.03f;
float4 CalculateFogFactor(float lViewSpaceDepth)
{
	return saturate((fogEnd - lViewSpaceDepth)/ (fogEnd - fogStart)) + pow(1.0f / 2.7828 ,(lViewSpaceDepth * fogDensity));
}

float4 ApplyFog(float4 lTextureColor, float4 fogFactor)
{
	return fogFactor* lTextureColor + (1.0f - fogFactor) * fogColor;
}
