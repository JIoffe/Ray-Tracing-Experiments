#pragma once
#include "FTCommons.h"
struct cColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	cColor(void);
	~cColor(void);

	void Set(unsigned char R, unsigned char G, unsigned char B);
	cColor operator + (cColor input);
	cColor operator * (float input);
	void operator *= (float input);
	cColor operator / (float input);
	cColor operator * (cColor input);

};

