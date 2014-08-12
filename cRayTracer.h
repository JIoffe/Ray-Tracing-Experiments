#pragma once
#include "cTextureManager.h"
#include "FTCommons.h"
#include "gdiSupplements.h"
#include "cLight.h"

class cRayTracer
{
	static const int MAX_BOUNCES = 4;
	Gdiplus::Bitmap * pBackBuffer;
	Gdiplus::Rect RenderTargetRect;
	float fRatio; //Ratio of Width - Height
	cTextureManager TextureManager;

	std::vector<FoofMath::SceneObject*> SceneList;
	std::vector<cLight*> LightList;
	//Private methods
	bool RunThroughList(FoofMath::Ray * pRay, FoofMath::CollisionData * pCD, FoofMath::SceneObject *pOrigin);
	float ComputeLighting(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewVector);
	//void HandleRefractions(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewDirection, cColor * pColor);
	void HandleReflections(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewDirection, cColor * pColor);
	cColor TraceReflection(FoofMath::CollisionData *pCD, FoofMath::Vector3f * pViewDirection, int * pBounceCounter);
	cColor TraceRefraction(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewDirection, int *pBounceCounter);
	cColor GetBGColor(FoofMath::Vector3f * pViewVector); //We might use the view vector to return the bg color.
	cColor GetTexelColor(FoofMath::CollisionData * pCD);
public:
	HWND ProgressDlg;
	void Initialize(int w, int h);
	void RenderScene(cCamera * pCamera);
	Gdiplus::Bitmap * GetRenderTarget();

	void SaveImage(const char * cFileName);

	void Cleanup();
	cRayTracer(void);
	~cRayTracer(void);
};

