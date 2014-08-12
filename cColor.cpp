#include "FTCommons.h"


cColor::cColor(void)
{
	r = 0;
	g = 0;
	b = 0;
}


cColor::~cColor(void)
{
}

void cColor::Set(unsigned char R, unsigned char G, unsigned char B){
	r = R;
	g = G;
	b = B;
	return;
}

// Ops automatically clamp to {0 - 255}
cColor cColor::operator+(cColor input){
	cColor temp;
	temp.r = r + input.r;
	temp.g = g + input.g;
	temp.b = b + input.b;
	return temp;
}
cColor cColor::operator*(cColor input){
	cColor temp;
	temp.r = r * input.r;
	temp.g = g * input.g;
	temp.b = b * input.b;
	return temp;
}
cColor cColor::operator/(float input){
	cColor temp;
	temp.r = r / input;
	temp.g = g / input;
	temp.b = b / input;
	return temp;
}
cColor cColor::operator*(float input){
	cColor temp;
	temp.r = min(255, max(this->r*input,0));
	temp.g = min(255, max(this->g*input,0));
	temp.b = min(255, max(this->b*input,0));
	return temp;
}
void cColor::operator*=(float input){
	this->r = min(255, max(this->r*input,0));
	this->g = min(255, max(this->g*input,0));
	this->b = min(255, max(this->b*input,0));
	return;
}