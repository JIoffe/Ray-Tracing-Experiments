#include "FTCommons.h"

using namespace Gdiplus;
//Just throw in the texture con/decon here
Texture::Texture(void){
	pImageData = NULL;
}
Texture::~Texture(void){
}

cTextureManager::cTextureManager(void)
{
}


cTextureManager::~cTextureManager(void)
{
}
bool cTextureManager::AddTexture(const WCHAR * Filename){
	Bitmap bmp(Filename);
	BitmapData bmpData;
	int width = bmp.GetWidth();
	int height = bmp.GetHeight();
	Texture * pTex = new Texture();
	pTex->h = height;
	pTex->w = width;
	pTex->pImageData = new cColor[width * height];
	Rect rect(0,0, width, height);
	bmp.LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);
	int dataWidth = bmpData.Stride / 4; // 4 bpp
	UINT* pPixelData = (UINT*)bmpData.Scan0;
	for(int y = 0; y < width; y++){
		int RowSkip = y * dataWidth;
		for(int x = 0; x < width; x++){
			int i = RowSkip + x;
			 Color TexelColor(pPixelData[i]);
			 pTex->pImageData[i].r = TexelColor.GetR();
			 pTex->pImageData[i].g = TexelColor.GetG();
			 pTex->pImageData[i].b = TexelColor.GetB();
		}
	}
	bmp.UnlockBits(&bmpData);
	TextureList.push_back(*pTex);
	return false;
}
cColor cTextureManager::GetTexelColor(unsigned long uTexIndex, float u, float v){
	//NEarest neighbor... for now
	//We will also TILE
	float TileU = u - (int)u;
	float TileV = v - (int)v;
	int w = TextureList[uTexIndex].w;
	int X = abs(TileU * w);
	int Y = abs(TileV * TextureList[uTexIndex].h);
	
	return TextureList[uTexIndex].pImageData[Y*w + X];
}