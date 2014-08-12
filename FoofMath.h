#pragma once

// (C) Foofles
// Some basic 3D Math definitions

namespace FoofMath{
	
	//Vec 3 f
	struct Vector3f{
		float x;
		float y;
		float z;

		//Const. + Deconst
		Vector3f();
		Vector3f(float X, float Y, float Z);
		~Vector3f();

		//Overloaded Ops
		Vector3f operator + (Vector3f);
		Vector3f operator - (Vector3f);
		Vector3f operator * (float); //Scalar
		Vector3f operator / (float);
		//Helper methods
		void Set(float X, float Y, float Z);
	};

	float Dot(Vector3f * A, Vector3f * b);
	Vector3f * Normalize(Vector3f * input);
	float GetLength(Vector3f * input);

	//MATRICES
	//Matrix 3x3
	struct Matrix3x3{
		float m1;
		float m2;
		float m3;
		float m4;
		float m5;
		float m6;
		float m7;
		float m8;
		float m9;

		Vector3f operator * (Vector3f input);
		void SetRows(Vector3f * pX, Vector3f * pY, Vector3f * pZ);
	};
	//Geometric structures
	struct Ray{
		Vector3f Origin;
		Vector3f Direction;

		Ray();
		~Ray();
	};
	
	struct SceneObject{
		static const unsigned short HAS_DIFFUSE_TEXTURE = 0x0001;
		static const unsigned short HAS_BUMP_TEXTURE = 0x0002;
		static const unsigned short IS_REFLECTIVE = 0x0004;
		static const unsigned short IS_REFRACTIVE = 0x0010;
		//static const unsigned short IS_REFRACTIVE
		unsigned short uFlags; 


		unsigned char r;
		unsigned char g;
		unsigned char b;
		float ReflectionFactor; //How much reflection is blended.
		float fSpecularExponent; // Hardness of material for highlights.
		unsigned long uDiffuseTexIndex;
		unsigned long uBumpTexIndex;
		void SetColor(unsigned char R, unsigned char G, unsigned char B);
		unsigned char GetType(void);

		static const unsigned char TYPE_SPHERE = 0x01;
		static const unsigned char TYPE_PLANE  = 0x02;

		float uvTexScale; // How much to scale a texture
		protected:
			unsigned char uGeometryType;
	};
	struct Sphere : SceneObject{
	private:
		float fRadius;
		float fRadiusSquared;
	public:
		Vector3f Origin;

		float GetRadius();
		float GetRadiusSquared();
		void SetRadius(float r);
		Sphere();
		~Sphere();
	};
	struct Plane : SceneObject{
		Vector3f Norm;
		float D;
		Plane();
		~Plane();
	};
	struct Box : SceneObject{
		Vector3f vTopLeft;
		Vector3f vBottomRight;
	};
	//**Collisions**
	struct CollisionData{
		Vector3f IntersectionPoint;
		Vector3f Norm;
		Vector3f vReflection;

		//Tex Coords
		float u;
		float v;

		bool bHasReflectionVector;
		bool bHoldsCollision;
		SceneObject * pObject;
		CollisionData();
		~CollisionData();
	};
	bool Intersection(Ray * pRay, Sphere * pSphere, CollisionData * pCD);
	bool Intersection(Ray * pRay, Plane * pPlane, CollisionData * pCD);
	//****************************************
	// OPTIMIZED FOR RAYS FROM ORIGIN (0,0,0)
	//****************************************
	bool IntersectFromOrigin(Vector3f * pRayDirection, Sphere * pSphere, CollisionData * pCD);
	bool IntersectFromOrigin(Vector3f * pRayDirection, Plane * pPlane, CollisionData * pCD);
	bool IntersectFromOrigin(Vector3f * pRayDirection, Box *pBox, CollisionData *pCD);
	//Misc
	float DistanceFalloffCosine(float Distance, float Domain);
	float SquaredDistance(Vector3f * A, Vector3f * B);
	float DegToRad(float d);
	float RadToDeg(float r);
	float Lerp(float a, float b, float s);

	//Constants
	static const float Pi =  3.14159265f;
}