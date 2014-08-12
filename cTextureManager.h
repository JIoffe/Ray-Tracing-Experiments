#pragma once
#include "cColor.h"
#include "FTCommons.h"
struct Texture{
	unsigned long w;  // in pixels.
	unsigned long h; 
	cColor * pImageData;
	Texture(void);
	~Texture(void);
};
class cTextureManager
{
	std::vector<Texture> TextureList;
public:
	cColor GetTexelColor(unsigned long uTexIndex, float u, float v);
	bool AddTexture(const WCHAR * Filename); //AS usual we will use GDI+ to do the work for us.
	cTextureManager(void);
	~cTextureManager(void);
};

