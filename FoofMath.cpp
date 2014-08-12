#include "FTCommons.h"

using namespace FoofMath;

//Scene object base
void SceneObject::SetColor(unsigned char R, unsigned char G, unsigned char B){
	r = R;
	g = G;
	b = B;
	return;
}
//Vec 3 f
Vector3f Vector3f::operator+ (Vector3f input){
	Vector3f vTemp;
	vTemp.x = x + input.x;
	vTemp.y = y + input.y;
	vTemp.z = z + input.z;
	return vTemp;
}
Vector3f Vector3f::operator- (Vector3f input){
	Vector3f vTemp;
	vTemp.x = x - input.x;
	vTemp.y = y - input.y;
	vTemp.z = z - input.z;
	return vTemp;
}
Vector3f Vector3f::operator* (float input){
	Vector3f vTemp;
	vTemp.x = x * input;
	vTemp.y = y * input;
	vTemp.z = z * input;
	return vTemp;
}
Vector3f Vector3f::operator / (float input){
	Vector3f vTemp;
	vTemp.x = x / input;
	vTemp.y = y / input;
	vTemp.z = z / input;
	return vTemp;
}
Vector3f::Vector3f(void){
	x = 0;
	y = 0;
	z = 0;
}
Vector3f::Vector3f(float X, float Y, float Z){
	x = X;
	y = Y;
	z = Z;
}
Vector3f::~Vector3f(){
}
void Vector3f::Set(float X, float Y, float Z){
	x = X;
	y = Y;
	z = Z;
	return;
}
float FoofMath::Dot(Vector3f * A, Vector3f * b){   //Best to normalize first.
	return ( (A->x * b->x) + (A->y * b->y) + (A->z * b->z) );
}
Vector3f * FoofMath::Normalize(Vector3f * input){  //Returns the normalized vector as well so it can be used as an argument.
	float Length = sqrt( (input->x * input->x) + (input->y * input->y) + (input->z * input->z) );
	input->x /= Length;
	input->y /= Length;
	input->z /= Length;
	return input;
}
float FoofMath::GetLength(Vector3f * input){
	return sqrt( (input->x * input->x) + (input->y * input->y) + (input->z * input->z) );
}

//MATRICES
//3x3
Vector3f Matrix3x3::operator * (Vector3f input){
	Vector3f Product;
	Product.x = m1 * input.x + m2 * input.y + m3 * input.z;
	Product.y = m4 * input.x + m5 * input.y + m6 * input.z;
	Product.z = m7 * input.x + m8 * input.y + m9 * input.z;
	return Product;
}
void Matrix3x3::SetRows(Vector3f * pX, Vector3f * pY, Vector3f * pZ){
	if(pX){
		m1 = pX->x;
		m2 = pX->y;
		m3 = pX->z;
	}
	if(pY){
		m4 = pY->x;
		m5 = pY->y;
		m6 = pY->z;
	}
	if(pX){
		m7 = pZ->x;
		m8 = pZ->y;
		m9 = pZ->z;
	}
	return;
}
//Ray
Ray::Ray(void)
{
	Origin.x = 0;
	Origin.y = 0;
	Origin.z = 0;
}


Ray::~Ray(void)
{
}

//Some geometric definitions
float Sphere::GetRadius(){
	return fRadius;
}
float Sphere::GetRadiusSquared(){
	return fRadiusSquared;
}
void Sphere::SetRadius(float r){
	fRadius = r;
	fRadiusSquared = r*r;
	return;
}
//Collisions
bool FoofMath::Intersection(Ray * pRay, Sphere * pSphere, CollisionData * pCD){
	//First we need to see if the Sphere is in front of the Ray or not.
	//C = Center of Sphere, O = Ray Origin
	//PC = Vector going from Ray Origin to Center of sphere

	Vector3f PC = pSphere->Origin - pRay->Origin;
	Vector3f PCN = PC; //Save a non-normalized copy...
	Normalize(&PCN);
	float PCdotRD = Dot(&PCN, &pRay->Direction);
	if(PCdotRD > 0.0f ){ 
		float RadiusSquared = pSphere->GetRadiusSquared();
		//Run a line to the ray, see if it's within the radius.
		Vector3f RayToSphere = pRay->Direction * Dot(&pRay->Direction, &PC);
		RayToSphere = pRay->Origin + RayToSphere;
		float SQDistance = SquaredDistance(&RayToSphere, &pSphere->Origin);
		if(SQDistance < RadiusSquared){
			//We only want collisions if the ray origin is outside of the sphere.
			if(SquaredDistance(&pRay->Origin, &pSphere->Origin) < SQDistance){
				return false;
			}
			else{
				//We only are concerned with the nearest intersection point.
				float i1 = sqrtf(RadiusSquared - SQDistance);
				i1 = sqrtf(SquaredDistance(&RayToSphere, &pRay->Origin)) - i1;
				Vector3f Intersection = pRay->Origin + pRay->Direction*i1;
				if(pCD){
					float OldDistance = SquaredDistance(&pCD->IntersectionPoint, &pRay->Origin);
					float NewDistance =  SquaredDistance(&Intersection, &pRay->Origin);
					if(!pCD->bHoldsCollision || NewDistance < OldDistance){
						pCD->IntersectionPoint = Intersection;
						pCD->Norm = Intersection - pSphere->Origin;
						Normalize(&pCD->Norm);
						pCD->u = (atan2f(pCD->Norm.x, pCD->Norm.z) / (2.0f * FoofMath::Pi) + 0.5f) * pSphere->uvTexScale;
						pCD->v = (asinf(pCD->Norm.y) / FoofMath::Pi + 0.5f) * pSphere->uvTexScale;
						pCD->bHoldsCollision = true;
						pCD->pObject = pSphere;
					}
				}
				return true;
			}
		}
		else{
			if(SQDistance == RadiusSquared){
				Vector3f Intersection = RayToSphere;
				if(pCD){
					float OldDistance = SquaredDistance(&pCD->IntersectionPoint, &pRay->Origin);
					float NewDistance =  SquaredDistance(&Intersection, &pRay->Origin);
					if(!pCD->bHoldsCollision || NewDistance < OldDistance){
						pCD->IntersectionPoint = Intersection;
						pCD->Norm = Intersection - pSphere->Origin;
						Normalize(&pCD->Norm);
						pCD->u = (atan2f(pCD->Norm.x, pCD->Norm.z) / (2.0f * FoofMath::Pi) + 0.5f) * pSphere->uvTexScale;
						pCD->v = (asinf(pCD->Norm.y) / FoofMath::Pi + 0.5f) * pSphere->uvTexScale;
						pCD->bHoldsCollision = true;
						pCD->pObject = pSphere;
					}
				}
				return true;
			}
			else{
				return false; // Close but no cigar.
			}
		}
	}else{  //Behind origin of ray, ignore.
		return false;
	}
}
bool FoofMath::Intersection(Ray * pRay, Plane * pPlane, CollisionData * pCD){
	float incidence = Dot(&pRay->Direction, &pPlane->Norm);
	if(incidence != 0.0f){ // Only consider case if the plane is facing the ray.
		float PNdotRO = Dot(&pPlane->Norm, &pRay->Origin);
		//t = how far along the ray we hit the plane.
		float t = -(PNdotRO + pPlane->D) / incidence;
		if( t < 0){
			//Collision behind = ignore
			return false;
		}
		else{
			Vector3f IntersectionPoint = pRay->Origin + pRay->Direction * t;
			if(pCD){
				if(!pCD->bHoldsCollision || GetLength(&IntersectionPoint) < GetLength(&pCD->IntersectionPoint)){
					pCD->IntersectionPoint = IntersectionPoint;
					pCD->Norm = pPlane->Norm;
					pCD->pObject = pPlane;
					pCD->bHoldsCollision = true;
					pCD->u = IntersectionPoint.x * pPlane->uvTexScale;
					pCD->v = IntersectionPoint.z * pPlane->uvTexScale;
				}
			}
			return true;
		}
	}
	else{
		return false;
	}
}

//****************************************
// OPTIMIZED FOR RAYS FROM ORIGIN (0,0,0)
//****************************************
bool FoofMath::IntersectFromOrigin(Vector3f * pRayDirection, Sphere * pSphere, CollisionData * pCD){
	//See the other intersection func for comments
	Vector3f PCN = pSphere->Origin; 
	Normalize(&PCN);
	float PCdotRD = Dot(&PCN, pRayDirection);
	if(PCdotRD > 0.0f ){ 
		float RadiusSquared = pSphere->GetRadiusSquared();
		//Run a line to the ray, see if it's within the radius.
		Vector3f RayToSphere = *pRayDirection * Dot(pRayDirection, &pSphere->Origin);

		float SQDistance = SquaredDistance(&RayToSphere, &pSphere->Origin);
		if(SQDistance < RadiusSquared){
			//We only want collisions if the ray origin is outside of the sphere.
			float Length = GetLength(&pSphere->Origin);
			if(Length < pSphere->GetRadius()){
				return false;
			}
			else{
				//We only are concerned with the nearest intersection point.
				float i1 = sqrtf(RadiusSquared - SQDistance);
				i1 = GetLength(&RayToSphere) - i1;
				Vector3f Intersection = *pRayDirection*i1;
				if(pCD){
					if(!pCD->bHoldsCollision || GetLength(&Intersection) < GetLength(&pCD->IntersectionPoint)){
						pCD->IntersectionPoint = Intersection;
						pCD->Norm = Intersection - pSphere->Origin;
						Normalize(&pCD->Norm);
						pCD->u = (atan2f(pCD->Norm.x, pCD->Norm.z) / (2.0f * FoofMath::Pi) + 0.5f) * pSphere->uvTexScale;
						pCD->v = (asinf(pCD->Norm.y) / FoofMath::Pi + 0.5f) * pSphere->uvTexScale;
						pCD->bHoldsCollision = true;
						pCD->pObject = pSphere;
					}
				}
				return true;
			}
		}
		else{
			if(SQDistance == RadiusSquared){
				Vector3f Intersection = RayToSphere;
				if(pCD){
					if(!pCD->bHoldsCollision || GetLength(&Intersection) < GetLength(&pCD->IntersectionPoint)){
						pCD->IntersectionPoint = Intersection;
						pCD->Norm = Intersection - pSphere->Origin;
						Normalize(&pCD->Norm);
						pCD->u = (atan2f(pCD->Norm.x, pCD->Norm.z) / (2.0f * FoofMath::Pi) + 0.5f) * pSphere->uvTexScale;
						pCD->v = (asinf(pCD->Norm.y) / FoofMath::Pi + 0.5f) * pSphere->uvTexScale;
						pCD->bHoldsCollision = true;
						pCD->pObject = pSphere;
					}
				}
				return true;
			}
			else{
				return false; // Close but no cigar.
			}
		}
	}else{  //Behind origin of ray, ignore.
		return false;
	}
}
bool FoofMath::IntersectFromOrigin(Vector3f * pRayDirection, Plane * pPlane, CollisionData * pCD){
	float incidence = Dot(pRayDirection, &pPlane->Norm);
	if(incidence != 0.0f){ // Only consider case if the plane is facing the ray.
		float t = -pPlane->D / incidence;
		if( t < 0){
			//Collision behind = ignore
			return false;
		}
		else{
			Vector3f IntersectionPoint = *pRayDirection * t;
			if(pCD){
				if(!pCD->bHoldsCollision || GetLength(&IntersectionPoint) < GetLength(&pCD->IntersectionPoint)){
					pCD->IntersectionPoint = IntersectionPoint;
					pCD->Norm = pPlane->Norm;
					pCD->pObject = pPlane;
					pCD->bHoldsCollision = true;
					pCD->u = IntersectionPoint.x * pPlane->uvTexScale;
					pCD->v = IntersectionPoint.z * pPlane->uvTexScale;
				}
			}
			return true;
		}
	}
	else{
		return false;
	}
}
//Misc
float FoofMath::DegToRad(float d){
	return d * 0.0174532925f;
}
float FoofMath::RadToDeg(float r){
	return r * 57.295779578f;
}
float FoofMath::Lerp(float a, float b, float s){
	return b - ((1.0f - s) * (b - a));
}
float FoofMath::SquaredDistance(Vector3f * A, Vector3f * B){
	float a = A->x - B->x;
	float b = A->y - B->y;
	float c = A->z - B->z;

	return (a*a) + (b*b) + (c*c);
}
float FoofMath::DistanceFalloffCosine(float Distance, float Domain){
	return cosf( (Distance*2 / Domain) * Pi);
}

//Misc Geometry funcs/constructors
unsigned char FoofMath::SceneObject::GetType(){
	return uGeometryType;
}
FoofMath::Sphere::Sphere(){
	uGeometryType = SceneObject::TYPE_SPHERE;
}
FoofMath::Sphere::~Sphere(){
}
FoofMath::Plane::Plane(){
	uGeometryType = SceneObject::TYPE_PLANE;
}
FoofMath::Plane::~Plane(){
}
FoofMath::CollisionData::CollisionData(){
	this->bHasReflectionVector = 0;
	this->bHoldsCollision = 0;
}
FoofMath::CollisionData::~CollisionData(){
}