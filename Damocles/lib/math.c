#include "../include/math.h"

static int randomSeed = 0;

int abs(int x){
	return (x< 0)? -x : x;
}


int random(int x){
	long int rndMult = -1794246735L;
	randomSeed += 1;
	randomSeed *= rndMult;
	return abs(randomSeed)%x;
}
