#include "FTCommons.h"

using namespace Gdiplus;
using namespace FoofMath;
using namespace std;

cRayTracer::cRayTracer(void)
{
	pBackBuffer = NULL;
}


cRayTracer::~cRayTracer(void)
{
}
void cRayTracer::Initialize(int w, int h){
	try{
		pBackBuffer = new Bitmap(w, h);
	}
	catch(...){
	}
	RenderTargetRect.X = 0;
	RenderTargetRect.Y = 0;
	RenderTargetRect.Width = w;
	RenderTargetRect.Height = h;

	if(w == h){
		fRatio = 1;
	}
	else{
		fRatio = (float) w / h;
	}


	//Setup some textures!
	TextureManager.AddTexture(L"FloorHerringbone0011_2_S.jpg");
	TextureManager.AddTexture(L"MarbleGreen0017_11_S.jpg");
	//Get some lights up in this bitch
	cLight * Light1 = new cLight();
	Light1->SetPosition(-30, 80, 6);
	Light1->SetRadius(400);
	LightList.push_back(Light1);

	cLight * Light2 = new cLight();
	Light2->SetPosition(50, 50, -12);
	Light2->SetRadius(400);
	LightList.push_back(Light2);
	return;
}
void cRayTracer::RenderScene(cCamera * pCamera){

	//SETUP SCENE
			Sphere MrSphere;
			float Radius = 15;
			MrSphere.SetRadius(Radius);
						//srand(GetTickCount());
						MrSphere.SetColor(rand() % 255, rand() % 255, rand() % 255);
						MrSphere.uFlags = SceneObject::IS_REFLECTIVE | SceneObject::HAS_DIFFUSE_TEXTURE;
						MrSphere.ReflectionFactor = 0.0f;
						MrSphere.fSpecularExponent = 64;
						MrSphere.uDiffuseTexIndex = 1;
						MrSphere.uvTexScale = 4;
						MrSphere.Origin.Set(0.0f, -10.2f + Radius, 42.0f);
						SceneList.push_back(&MrSphere);
				
		Plane TestPlane;
		TestPlane.D = 10.2f;
		TestPlane.Norm.Set(0,1,0);
		TestPlane.SetColor(70,80,255);
		TestPlane.ReflectionFactor = 0.6f;
		TestPlane.uFlags = SceneObject::HAS_DIFFUSE_TEXTURE | SceneObject::IS_REFLECTIVE;
		TestPlane.uDiffuseTexIndex = 0;
		TestPlane.fSpecularExponent = 4;
		TestPlane.uvTexScale = 0.05f;
		SceneList.push_back(&TestPlane);

				//	Graphics g(pBackBuffer);
				////Clrscn
				//g.Clear(Color(255,0,0,0));
	//Lock the bitmap to pixels we can write to
	BitmapData bmpData;

	if(pBackBuffer->LockBits(&RenderTargetRect, ImageLockModeWrite, PixelFormat32bppARGB, &bmpData) == Ok){
		UINT * pPixelData = (UINT*)bmpData.Scan0;
		UINT DataWidth = bmpData.Stride / 4;

		float FOV = pCamera->GetFov();

		float ProjPlaneH = 2 * tan(DegToRad(FOV/2));
		float ProjPlaneW = ProjPlaneH * fRatio;

		float ProjPlaneX = ProjPlaneW / 2;
		float ProjPlaneY = ProjPlaneH / 2;
		float RayStepX = ProjPlaneW / RenderTargetRect.Width;
		float RayStepY = ProjPlaneH / RenderTargetRect.Height;

		float CurrentRayY = ProjPlaneY;

		//cast a ray for each pixel in the bitmap
		for(UINT y = 0; y < RenderTargetRect.Height; y++){
			UINT Rowskip = y * DataWidth;
			float CurrentRayX = -ProjPlaneX;
			for(UINT x = 0; x < RenderTargetRect.Width; x++){
				CollisionData CD;
				CD.bHoldsCollision = false;
				CD.bHasReflectionVector = false;
				Ray PixelRay;
				PixelRay.Direction.x = CurrentRayX;
				PixelRay.Direction.y = CurrentRayY;
				PixelRay.Direction.z = 1;
				Normalize(&PixelRay.Direction);

				CurrentRayX += RayStepX;

				cColor PixelOutput;
				for(vector<SceneObject*>::iterator it = SceneList.begin(); it < SceneList.end(); it++){
					unsigned char uType = ((SceneObject*)*it)->GetType();
					switch(uType){
						case SceneObject::TYPE_PLANE:{
							IntersectFromOrigin(&PixelRay.Direction, (Plane*)*it, &CD);
							break;
						}
						case SceneObject::TYPE_SPHERE:{
							IntersectFromOrigin(&PixelRay.Direction, (Sphere*)*it, &CD);
							break;
						}
						default:
							break;
					}
				}

				if(CD.bHoldsCollision){
					//Get Color. Either texture, solid color, or both!
					PixelOutput = GetTexelColor(&CD);
					//See if we need to do any reflections/refractions.
					if(CD.pObject->uFlags & SceneObject::IS_REFLECTIVE){
						HandleReflections(&CD, &PixelRay.Direction, &PixelOutput);
					}
					if(CD.pObject->uFlags & SceneObject::IS_REFRACTIVE){
						int Count = 1;
						cColor colorR = TraceRefraction(&CD, &PixelRay.Direction, &Count);
						float Transparency = 0.9f;
							PixelOutput.r = Lerp(PixelOutput.r, colorR.r, Transparency);
							PixelOutput.g = Lerp(PixelOutput.g, colorR.g, Transparency);
							PixelOutput.b = Lerp(PixelOutput.b, colorR.b, Transparency);
					}
					//Perform Lighting
					float lighting = ComputeLighting(&CD, &PixelRay.Direction);
					PixelOutput *= lighting;
					//PixelOutput *= lighting;
					//PixelOutput *= lighting;
				}
				else{
					//We hit nothing, return BG Color
					PixelOutput = GetBGColor(&PixelRay.Direction);
				}
				Color PixelColor(PixelOutput.r, PixelOutput.g, PixelOutput.b);
				pPixelData[Rowskip + x] = (UINT)PixelColor.GetValue();				
			};
			CurrentRayY -= RayStepY;
			SendMessage(ProgressDlg, (WM_USER + 5), 0, 0); 
		}
		pBackBuffer->UnlockBits(&bmpData);
	}
	return;
}
Gdiplus::Bitmap * cRayTracer::GetRenderTarget(){
	return pBackBuffer;
}
void cRayTracer::HandleReflections(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewDirection, cColor * pColor){
	int Bounces = cRayTracer::MAX_BOUNCES;
	cColor colorR = TraceReflection(pCD, pViewDirection, &Bounces);
	float Reflect = pCD->pObject->ReflectionFactor;
	
	//Compute fresnel term - appear more reflective along edges.
	Vector3f Norm = pCD->Norm * -1; //Negate norm
	float Fresnel = 1.0f - Dot(pViewDirection, &Norm);
	Reflect *= Fresnel;
	pColor->r = Lerp(pColor->r, colorR.r, Reflect);
	pColor->g = Lerp(pColor->g, colorR.g, Reflect);
	pColor->b = Lerp(pColor->b, colorR.b, Reflect);
	return;
}
cColor cRayTracer::TraceRefraction(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewDirection, int *pBounceCounter){
	cColor colorR;
	CollisionData RefractCD;
	Ray TestRay;
	TestRay.Origin.Set(0,0,0);
	TestRay.Direction = *pViewDirection;
	RunThroughList(&TestRay, &RefractCD, pCD->pObject);
	if(RefractCD.bHoldsCollision){
		colorR = GetTexelColor(&RefractCD);
	}
	else{
		colorR = GetBGColor(pViewDirection);
	}
	return colorR;
}
cColor cRayTracer::TraceReflection(FoofMath::CollisionData *pCD, FoofMath::Vector3f * pViewDirection, int * pBounceCounter){
	CollisionData ReflectionCD;
	ReflectionCD.bHoldsCollision = false;
	ReflectionCD.bHasReflectionVector = false;
	Ray ReflectV;
	ReflectV.Direction = (pCD->Norm *2 * -Dot(pViewDirection, &pCD->Norm)) + *pViewDirection;
	ReflectV.Origin = pCD->IntersectionPoint;
	pCD->vReflection = ReflectV.Direction;
	pCD->bHasReflectionVector = true;
	RunThroughList(&ReflectV, &ReflectionCD, pCD->pObject);
	cColor colorR;
	if(ReflectionCD.bHoldsCollision){
		//Perform Lighting
		float lighting = ComputeLighting(&ReflectionCD, &ReflectV.Direction);
		colorR = GetTexelColor(&ReflectionCD);
		if(ReflectionCD.pObject->uFlags & SceneObject::IS_REFLECTIVE){
			if(*pBounceCounter > 0){
				//Oh snap.
				cColor subcolorR = TraceReflection(&ReflectionCD, &ReflectV.Direction, pBounceCounter);
				float Reflect = ReflectionCD.pObject->ReflectionFactor;
	
				//Compute fresnel term - appear more reflective along edges.
				Vector3f Norm = ReflectionCD.Norm * -1; //Negate norm
				float Fresnel = 1.0f - Dot(&ReflectV.Direction, &Norm);
				Reflect *= Fresnel;
				colorR.r = Lerp(colorR.r, subcolorR.r, Reflect);
				colorR.g = Lerp(colorR.g, subcolorR.g, Reflect);
				colorR.b = Lerp(colorR.b, subcolorR.b, Reflect);
				*pBounceCounter--;
			}
		}
		colorR *= lighting;
	}
	else{
		//We hit the sky. Return BG color;
		colorR = GetBGColor(pViewDirection);
	}
	return colorR;
}
bool cRayTracer::RunThroughList(Ray * pRay, FoofMath::CollisionData * pCD, FoofMath::SceneObject *pOrigin){
	for(vector<SceneObject*>::iterator it = SceneList.begin(); it < SceneList.end(); it++){
		if(((SceneObject*)*it) != pOrigin){
			unsigned char uType = ((SceneObject*)*it)->GetType();
			switch(uType){
				case SceneObject::TYPE_SPHERE:{
					if(Intersection(pRay, (Sphere*)*it, pCD)){
						if(!pCD){
							return true; //We're obviously only interested in ANY collision
						}
					}
					break;
				}
				case SceneObject::TYPE_PLANE:{
					if(Intersection(pRay, (Plane*)*it, pCD)){
						if(!pCD){
							return true; //We're obviously only interested in ANY collision
						}
					}
					break;
				}
				default:
					break;
			}
		}
	}
	return false;
}
float cRayTracer::ComputeLighting(FoofMath::CollisionData * pCD, FoofMath::Vector3f * pViewVector){
	float lightingtotal = 0;
	float Ambience = 0.15f;
	for(vector<cLight*>::iterator it = LightList.begin(); it < LightList.end(); it++){
		Vector3f LightV = ((cLight*)*it)->GetLightV(&pCD->IntersectionPoint);
		float lighting =  max(Dot(&pCD->Norm, Normalize(&LightV)), 0.0f);
		//Todo - make distance attenuation work.
		//lighting *= max(DistanceFalloffCosine(sqrtf(SquaredDistance(&pCD->IntersectionPoint, &LightDirection)), lightRadius), 0);
		//lighting = min(1.0f, (lighting + Ambience));
	
		if(pCD->pObject->fSpecularExponent > 0){
			if(pCD->bHasReflectionVector){
				float NdR = Dot(&LightV, &pCD->vReflection);
				if(NdR > 0){
					lighting += 2*powf(NdR, pCD->pObject->fSpecularExponent);
				}
			}
			else{
				if(pViewVector){
					Vector3f R = (pCD->Norm *2 * -Dot(pViewVector, &pCD->Norm)) + *pViewVector;
					float NdR = Dot(&pCD->Norm, &R);
					if(NdR > 0){
						float LdotR = Dot(&LightV, &R);
						if(LdotR > 0){
							lighting += powf(LdotR, pCD->pObject->fSpecularExponent);
						}
					}
				}
			}
		}
		Ray LRay; // lol, el ray
		LRay.Direction = LightV;
		LRay.Origin = pCD->IntersectionPoint;
		if(RunThroughList(&LRay, NULL, pCD->pObject)){
			lighting = 0;//Ambience;
		}
		lightingtotal += lighting;
	}
	lightingtotal /= LightList.size(); //Avg the light. We're limited and we don't want to overpower the display.
	lightingtotal += Ambience;
	return lightingtotal;
}
cColor cRayTracer::GetBGColor(FoofMath::Vector3f * pViewVector){
	cColor BG;
	Vector3f Up;
	Up.Set(0,1,0);
	float Upness = max(Dot(pViewVector, &Up), 0);
	BG.Set(Upness*100,Upness*100,Upness*255);
	return BG;
}
void cRayTracer::SaveImage(const char * cFileName){
	CLSID pngClsid;
	//get the encode mode of the BMP file
	GetEncoderClsid(L"image/png",&pngClsid);//helper function

	WCHAR wFileName[MAX_PATH]; //We need it in wide byte...
	// Convert to a wchar_t*
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wFileName, strlen(cFileName) + 1, cFileName, _TRUNCATE);
					
	this->pBackBuffer->Save(wFileName,&pngClsid);
	return;
}
void cRayTracer::Cleanup(){
	if(pBackBuffer){
		delete pBackBuffer;
	}
	//for(vector<SceneObject*>::iterator it = SceneList.begin(); it < SceneList.end(); it++){
	//	delete *it;
	//}
	//for(vector<cLight*>::iterator it = LightList.begin(); it < LightList.end(); it++){
	//	delete *it;
	//}
	return;
}
cColor cRayTracer::GetTexelColor(FoofMath::CollisionData * pCD){
	cColor tColor;
	if(pCD->pObject->uFlags & SceneObject::HAS_DIFFUSE_TEXTURE){
			tColor = TextureManager.GetTexelColor(pCD->pObject->uDiffuseTexIndex, pCD->u, pCD->v);
	}else{

			tColor.r = pCD->pObject->r;
			tColor.g = pCD->pObject->g;
			tColor.b = pCD->pObject->b;
	}
	return tColor;
}
