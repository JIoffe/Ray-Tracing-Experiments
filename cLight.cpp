#include "FTCommons.h"

using namespace FoofMath;
cLight::cLight(void)
{
	fRadius = 50;
	LightPosition.Set(0,0,0);
	LightColor.Set(255,255,255);
	uType = Light_Type::POINT_LIGHT;
}


cLight::~cLight(void)
{
}

void cLight::SetType(unsigned char nType){
	uType = nType;
	return;
}
unsigned char cLight::GetType(){
	return uType;
}

void cLight::SetRadius(float newRadius){
	fRadius = newRadius;
	return;
}
float cLight::GetRadius(){
	return fRadius;
}

void cLight::SetPosition(float x, float y, float z){
	LightPosition.Set(x,y,z);
	return;
}
FoofMath::Vector3f cLight::GetPosition(){
	return LightPosition;
}
FoofMath::Vector3f cLight::GetLightV(FoofMath::Vector3f * pPosition){
	return LightPosition - *pPosition;
}