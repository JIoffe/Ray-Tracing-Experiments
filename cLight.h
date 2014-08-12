#pragma once
#include "FTCommons.h"

struct Light_Type{ //Just because I don't wanna type cLight
	static const unsigned char POINT_LIGHT = 0x11;
	static const unsigned char SPOT_LIGHT = 0x12;
	static const unsigned char DIRECTIONAL_LIGHT = 0x14;
};
class cLight
{
	float fRadius;
	FoofMath::Vector3f LightPosition; // if(DIRECTIONAL_LIGHT)
	cColor LightColor;
	unsigned char uType;
public:
	void SetType(unsigned char nType);
	unsigned char GetType();

	void SetRadius(float newRadius);
	float GetRadius();

	void SetPosition(float x, float y, float z);
	FoofMath::Vector3f GetPosition();
	FoofMath::Vector3f GetLightV(FoofMath::Vector3f * pPosition);
	cLight(void);
	~cLight(void);
};

