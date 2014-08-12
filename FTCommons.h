#include <Windows.h>
#include <vector>
#include "cRayTracer.h"
#include <math.h>
#include <GdiPlus.h>  // once again, use the powers of GDI+ to do most of the work
#include <cstdio>

#include "FoofMath.h"

#include "cCamera.h"
//GDI+ libraries
#pragma comment (lib,"Gdiplus.lib")
