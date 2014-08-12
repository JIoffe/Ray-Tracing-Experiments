#pragma once
#include "FTCommons.h"
class cCamera
{
	FoofMath::Vector3f Pos;
	FoofMath::Vector3f vDirection;
	float fFov;

public:

	FoofMath::Vector3f GetPos();
	FoofMath::Vector3f GetDirection();
	float GetFov();
	cCamera(void);
	~cCamera(void);
};

