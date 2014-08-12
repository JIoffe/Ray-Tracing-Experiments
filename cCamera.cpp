#include "FTCommons.h"


cCamera::cCamera(void)
{
	fFov = 60.0f;
}


cCamera::~cCamera(void)
{
}

FoofMath::Vector3f cCamera::GetPos(){
	return Pos;
}
FoofMath::Vector3f cCamera::GetDirection(){
	return vDirection;
}
float cCamera::GetFov(){
	return fFov;
}