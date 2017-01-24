#include "mathutils.h"
#include <math.h>


float periodicmod(float x, float y)
{
	return fmod(fmod(x, y) + y, y);
}

float inrange(float x, float min, float max)
{
	return periodicmod(x - min, max - min) + min;
}

float saturate(float x, float min, float max)
{
	if (x < min)
		return min;
	else if (x > max)
		return max;
	else
		return x;
}
